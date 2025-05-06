#include "../headers.hh"
#include "ddg_types.hh"

#include <map>
#include <queue>
#include <set>
#include <vector>

extern Program* program;

extern std::string USEVAR;
QDef gen_qdef(CFG_Node* cfg_node, QNode qnode) {
  const std::string& def = cfg_node->get_def();
  if (!program->is_in_cur_partition(cfg_node->get_lopd())) {
    return {{USEVAR, qnode.node}, 1};
  } else if (cfg_node->get_rhs_operands()[0]->get_type() == CFG_OpdType::CFG_InputOpd) {
    return {{def, qnode.node}, 1};
  }
  return {{def, qnode.node}, qnode.context};
}

void ddg_construct() {
  int default_context = program->insert_ddg_context({"main"});

  int start_main = program->get_proc("main")->get_start_node();
  std::map<QNode, std::set<QDef>> rd_in;
  rd_in[{start_main, default_context}] = std::set<QDef>();
  for (const std::string& var_name : program->get_globals()) {
    CFG_Opd opd = CFG_Opd(CFG_OpdType::CFG_VarOpd, var_name);
    if (program->is_in_cur_partition(&opd)) {
      rd_in[{start_main, default_context}].insert({var_name, 0, default_context});
      program->add_ddg_node({var_name, 0, default_context});
    }
  }
  std::map<QNode, std::set<QDef>> rd_out;

  std::queue<QNode> worklist;
  worklist.push({start_main, default_context});
  std::set<QNode> in_list;
  in_list.insert({start_main, default_context});

  while (!worklist.empty()) {
    QNode cur_qnode = worklist.front();
    worklist.pop();
    in_list.erase(in_list.find(cur_qnode));

    std::set<QDef> orig_rd_out = rd_out[cur_qnode];

    CFG_Node* node = program->get_cfg_node(cur_qnode.node, true);
    if (node->get_type() == CFG_NodeType::CFG_StartNode) {
      auto it = program->get_ddg_reverse_transitions(cur_qnode.context);
      if (it != program->ddg_reverse_transitions_end()) {
        for (QNode qnode : it->second) {
          for (QDef qdef : rd_in[qnode]) {
            rd_in[cur_qnode].insert(qdef);
          }
        }
      }
    } else {
      for (int pred : node->get_predecessors()) {
        for (QDef qdef : rd_out[{pred, cur_qnode.context}]) {
          rd_in[cur_qnode].insert(qdef);
        }
      }
    }

    if (node->get_type() == CFG_NodeType::CFG_AssignNode) {
      std::set<std::string> uses = node->get_uses();
      QDef new_qdef = gen_qdef(node, cur_qnode);
      program->add_ddg_node(new_qdef);
      for (QDef qdef : rd_in[cur_qnode]) {
        if (uses.find(qdef.def.var_name) != uses.end()) {
          program->add_ddg_edge(qdef, new_qdef);
        }
      }
    }

    bool updated_transition = false;
    if (node->get_type() == CFG_NodeType::CFG_CallNode) {
      auto it = program->get_ddg_transition(cur_qnode);
      if (it != program->ddg_transitions_end(cur_qnode.node)) {
        int end_node = program->get_proc(node->get_callee())->get_end_node();
        for (QDef qdef : rd_out[{end_node, it->second}]) {
          rd_out[cur_qnode].insert(qdef);
        }
      }

      updated_transition = program->create_ddg_transition(cur_qnode, Context::gen_context(node->get_callee(), rd_in[cur_qnode]));
    } else {
      rd_out[cur_qnode] = rd_in[cur_qnode];
      if (node->get_type() == CFG_NodeType::CFG_AssignNode && program->is_in_cur_partition(node->get_lopd())) {
        std::string killed_var = node->get_def();
        // Applies rd_kill
        std::vector<QDef> to_remove;
        for (QDef qdef : rd_out[cur_qnode]) {
          if (qdef.def.var_name == killed_var) {
            to_remove.push_back(qdef);
          }
        }
        for (QDef qdef : to_remove) {
          rd_out[cur_qnode].erase(rd_out[cur_qnode].find(qdef));
        }

        // Applies rd_gen
        if (node->get_lopd()->get_type() != CFG_OpdType::CFG_UsevarOpd) {
          rd_out[cur_qnode].insert(gen_qdef(node, cur_qnode));
        }
      }
    }

    if (updated_transition) {
      // Update at start of call
      int start_node = program->get_proc(node->get_callee())->get_start_node();
      auto it = program->get_ddg_transition(cur_qnode);
      if (it != program->ddg_transitions_end(cur_qnode.node)) {
        if (in_list.find({start_node, it->second}) == in_list.end()) {
          worklist.push({start_node, it->second});
          in_list.insert({start_node, it->second});
        }
      }
    }

    if (rd_out[cur_qnode] != orig_rd_out) {
      if (node->get_type() == CFG_NodeType::CFG_EndNode) {
        // Update at return from call
        auto it = program->get_ddg_reverse_transitions(cur_qnode.context);
        if (it != program->ddg_reverse_transitions_end()) {
          for (QNode qnode : it->second) {
            if (in_list.find(qnode) == in_list.end()) {
              worklist.push(qnode);
              in_list.insert(qnode);
            }
          }
        }
      } else {
        for (int succ : node->get_successors()) {
          if (in_list.find({succ, cur_qnode.context}) == in_list.end()) {
            worklist.push({succ, cur_qnode.context});
            in_list.insert({succ, cur_qnode.context});
          }
        }
      }
    }
  }
}

bool get_operand_value(CFG_Opd* opd, int* opd_value, QDef qdef, std::map<QDef, int>& propagated_values) {
  switch (opd->get_type()) {
    case CFG_OpdType::CFG_NumOpd:
      *opd_value = opd->get_opd_value();
      return true;
    case CFG_OpdType::CFG_VarOpd: {
      bool found = false;
      for (QDef dependency : program->get_ddg_incoming(qdef)) {
        if (dependency.def.var_name == opd->get_opd_var()) {
          auto it = propagated_values.find(dependency);
          if (it == propagated_values.end()) {
            return false;
          }
          if (found && *opd_value != it->second) {
            return false;
          }
          found = true;
          *opd_value = it->second;
        }
      }

      if (found) {
        for (QDef incoming : program->get_ddg_incoming(qdef)) {
          if (incoming.def.var_name == opd->get_opd_var()) {
            program->remove_ddg_edge(incoming, qdef);
          }
        }
      }

      return found;
    }
    default: return false;
  }
}

bool propagate_value(QDef qdef, std::map<QDef, int>& propagated_values) {
  if (qdef.def.node == 0 || propagated_values.find(qdef) != propagated_values.end()) {
    return false;
  }

  CFG_Node* node = program->get_cfg_node(qdef.def.node, true);
  std::vector<CFG_Opd*> operands = node->get_rhs_operands();
  std::string op = node->get_op();
  if (op == "=") {
    CHECK_INVARIANT(operands.size() == 1, "Expected 1 operand");
    int value;
    if (get_operand_value(operands[0], &value, qdef, propagated_values)) {
      propagated_values[qdef] = value;
      return true;
    }
    return false;
  }

  CHECK_INVARIANT(operands.size() == 2, "Expected 2 operands");
  int v1, v2;
  bool f1 = get_operand_value(operands[0], &v1, qdef, propagated_values);
  bool f2 = get_operand_value(operands[1], &v2, qdef, propagated_values);
  if (f1 && f2) {
    if (op == "+") {
      propagated_values[qdef] = v1 + v2;
      return true;
    } else if (op == "-") {
      propagated_values[qdef] = v1 - v2;
      return true;
    } else if (op == "*") {
      propagated_values[qdef] = v1 * v2;
      return true;
    } else if (op == "/") {
      propagated_values[qdef] = v1 / v2;
      return true;
    }
  }

  return false;
}

std::map<QDef, int> ddg_propagate_constants() {
  std::map<QDef, int> propagated_values; // Also serves as an "in_list"
  std::queue<QDef> worklist;
  for (QDef qdef : program->get_ddg_nodes()) {
    if (!program->is_part_of_other_partition(qdef.def.node) && propagate_value(qdef, propagated_values)) {
      worklist.push(qdef);
    }
  }

  while (!worklist.empty()) {
    QDef qdef = worklist.front();
    worklist.pop();

    for (QDef outgoing : program->get_ddg_outgoing(qdef)) {
      if (!program->is_part_of_other_partition(outgoing.def.node) && propagate_value(outgoing, propagated_values)) {
        worklist.push(outgoing);
      }
    }
  }

  return propagated_values;
}

bool tryReduce(Def def, const std::set<int>& contexts) {
  std::map<Def, std::pair<std::set<int>, int>> prev_versions;
  bool first = true;
  for (int context : contexts) {
    int value;
    if (program->get_ddg_propagated_value({def, context}, &value)) {
      if (!prev_versions.empty()) {
        return false;
      }
      first = false;
      continue;
    }

    std::set<QDef> other = program->get_ddg_incoming({def, context});
    std::map<Def, std::pair<std::set<int>, int>> versions;
    for (QDef use : other) {
      auto it = versions.find(use.def);
      if (it == versions.end()) {
        if (program->get_ddg_propagated_value(use, &value)) {
          versions[use.def] = std::make_pair(std::set<int>(), value);
        } else {
          versions[use.def].first.insert(use.context);
        }
      } else {
        bool res;
        if ((res = program->get_ddg_propagated_value(use, &value)) != it->second.first.empty()) {
          return false;
        }
        if (res) {
          if (it->second.second != value) {
            return false;
          }
        } else {
          it->second.first.insert(use.context);
        }
      }
    }

    if (first) {
      first = false;
      prev_versions = versions;
    } else if (prev_versions != versions) {
      return false;
    }
  }

  std::set<QDef> uses;
  std::set<QDef> usedBy;
  first = true;
  for (int context : contexts) {
    if (first) {
      for (QDef src : program->get_ddg_incoming({def, context})) {
        uses.insert(src);
      }
      first = false;
    }
    for (QDef dest : program->get_ddg_outgoing({def, context})) {
      usedBy.insert(dest);
    }
    program->remove_ddg_node({def, context});
  }

  for (QDef src : uses) {
    program->add_ddg_edge(src, {def, 1});
  }
  for (QDef dest : usedBy) {
    program->add_ddg_edge({def, 1}, dest);
  }
  return true;
}

void ddg_reduce() {
  std::map<Def, std::set<int>> qdefs;
  for (QDef qdef : program->get_ddg_nodes()) {
    qdefs[qdef.def].insert(qdef.context);
  }

  std::queue<Def> worklist;
  std::set<Def> reduced;
  for (auto pair : qdefs) {
    if (pair.second.size() == 1) {
      reduced.insert(pair.first);
    } else if (tryReduce(pair.first, pair.second)) {
      worklist.push(pair.first);
      reduced.insert(pair.first);
    }
  }

  while (!worklist.empty()) {
    Def def = worklist.front();
    worklist.pop();
    if (reduced.find(def) != reduced.end()) {
      continue;
    }
    for (QDef use : program->get_ddg_outgoing({def, 1})) {
      if (reduced.find({use.def}) == reduced.end()) {
        if (tryReduce(use.def, qdefs[use.def])) {
          worklist.push(use.def);
          reduced.insert(use.def);
        }
      }
    }
  }
}

std::set<QDef> ddg_detect_dead_qdefs() {
  std::set<QDef> dead_qdefs = program->get_ddg_nodes();
  std::queue<QDef> worklist;
  for (QDef qdef : program->get_ddg_nodes()) {
    if (qdef.def.var_name == USEVAR) {
      dead_qdefs.erase(dead_qdefs.find(qdef));
      for (QDef use : program->get_ddg_incoming(qdef)) {
        auto it = dead_qdefs.find(use);
        if (it != dead_qdefs.end()) {
          dead_qdefs.erase(it);
          worklist.push(use);
        }
      }
    }
  }

  while (!worklist.empty()) {
    QDef qdef = worklist.front();
    worklist.pop();
    for (QDef use : program->get_ddg_incoming(qdef)) {
      auto it = dead_qdefs.find(use);
      if (it != dead_qdefs.end()) {
        dead_qdefs.erase(it);
        worklist.push(use);
      }
    }
  }

  return dead_qdefs;
}
