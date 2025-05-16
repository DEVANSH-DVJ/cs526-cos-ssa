#include "headers.hh"

extern Program *program;

void ssa_init() {
  // Create SSA nodes, leaving assignment nodes empty (no meta assignments)
  for (auto [proc_name, proc] : *program->get_procs()) {
    for (int node : proc->get_cfg_nodes()) {
      CFG_Node *cfg_node = program->get_cfg_node(node, true);

      SSA_Node *ssa_node;
      switch (cfg_node->get_type()) {
      case CFG_StartNode:
        ssa_node = new SSA_Node(SSA_StartNode, node, "START " + proc_name);
        break;
      case CFG_EndNode:
        ssa_node = new SSA_Node(SSA_EndNode, node, "END " + proc_name);
        break;
      case CFG_CallNode:
        ssa_node = new SSA_Node(SSA_CallNode, node, "CALL " + cfg_node->get_callee(),
                                cfg_node->get_callee());
        break;
      case CFG_AssignNode:
        ssa_node = new SSA_Node(SSA_AssignNode, node);
        break;
      case CFG_EmptyNode:
        ssa_node = new SSA_Node(SSA_EmptyNode, node);
        break;
      }

      ssa_node->set_parent_proc(cfg_node->get_parent_proc());
      program->add_ssa_node(ssa_node);
      proc->add_ssa_node(ssa_node);
    }
  }

  // Create SSA edges
  for (auto [_, proc] : *program->get_procs()) {
    for (int src : proc->get_cfg_nodes()) {
      for (int dst : program->get_cfg_node(src, true)->get_successors()) {
        SSA_Edge *edge = new SSA_Edge(src, dst);
        program->add_ssa_edge(edge);
        proc->add_ssa_edge(edge);
      }
    }
  }
}

// Converts a cfg_opd to a ssa_opd, accounting for whether the cfg_opd is part
// of the current partition (if it is a variable) Final versions maps variables
// to a known ssa_opd that should be used
SSA_Opd *cfg_to_ssa_opd(CFG_Opd *cfg_opd, std::pair<int, int> meta_num, bool is_def,
                        std::map<std::string, SSA_Opd *> &final_versions) {
  if (cfg_opd == nullptr) {
    return nullptr;
  }

  switch (cfg_opd->get_type()) {
  case CFG_OpdType::CFG_NumOpd:
    return new SSA_Opd(SSA_NumOpd, cfg_opd->get_opd_value());
  case CFG_OpdType::CFG_VarOpd: {
    if (!program->is_in_cur_partition(cfg_opd)) {
      return new SSA_Opd(is_def ? SSA_UsevarOpd : SSA_InputOpd, meta_num);
    }
    std::string var = cfg_opd->get_opd_var();
    auto it = final_versions.find(var);
    if (it != final_versions.end()) {
      return it->second;
    }
    return new SSA_Opd(SSA_VarOpd, meta_num, var);
  }
  case CFG_OpdType::CFG_InputOpd:
    return new SSA_Opd(SSA_InputOpd, meta_num);
  case CFG_OpdType::CFG_UsevarOpd:
    return new SSA_Opd(SSA_UsevarOpd, meta_num);
  }
  return nullptr;
}

void ssa_construct_partition() {
  for (QDef qdef : program->get_dfg_nodes()) {
    if (qdef.def.node == 0 || program->is_part_of_other_partition(qdef.def.node) ||
        program->dfg_is_dead(qdef)) {
      // Skip if this is an "uninitialized" qdef or belongs to another partition
      // or if the qdef is dead and thus does not need to be handled
      continue;
    }

    CFG_Node *cfg_node = program->get_cfg_node(qdef.def.node, true);
    std::list<SSA_Stmt *> *stmts = new std::list<SSA_Stmt *>();
    std::set<QDef> deps = program->get_dfg_incoming(qdef);
    std::map<string, std::list<std::pair<int, int>>> versions;
    std::map<string, SSA_Opd *> final_versions;
    for (QDef dep : deps) {
      versions[dep.def.var_name].push_back(std::make_pair(dep.def.node, dep.context));
    }

    SSA_Node *node = program->get_ssa_node(qdef.def.node, true);
    SSA_Opd *lopd = cfg_to_ssa_opd(
        cfg_node->get_lopd(), std::make_pair(qdef.def.node, qdef.context), true, final_versions);
    int value;
    if (program->get_dfg_propagated_value(qdef, &value)) {
      // Use the propagated value
      stmts->push_back(
          new SSA_Stmt(SSA_AssignStmt, "=", lopd, new SSA_Opd(SSA_NumOpd, value), nullptr));
      node->add_meta(new SSA_Meta(std::make_pair(qdef.def.node, qdef.context), stmts));
      continue;
    }

    // Add phi nodes
    for (auto &[use, meta_nums] : versions) {
      if (meta_nums.size() == 1) {
        final_versions[use] = new SSA_Opd(SSA_VarOpd, *meta_nums.begin(), use);
        continue;
      }
      CHECK_INVARIANT(meta_nums.size() >= 2, "Expected at least two incoming defs");

      std::list<SSA_Opd *> *ropds = new std::list<SSA_Opd *>();
      for (auto meta_num : meta_nums) {
        ropds->push_back(new SSA_Opd(SSA_VarOpd, meta_num, use));
      }

      stmts->push_back(new SSA_Stmt(
          SSA_PhiStmt, new SSA_Opd(SSA_PhiOpd, std::make_pair(qdef.def.node, qdef.context), use),
          ropds));
      final_versions[use] =
          new SSA_Opd(SSA_PhiOpd, std::make_pair(qdef.def.node, qdef.context), use);
    }

    // Create the actual assignment, and add this as a meta assignment to node
    auto pair = cfg_node->get_ropds();
    SSA_Opd *ropd1 = cfg_to_ssa_opd(pair.first, std::make_pair(qdef.def.node, qdef.context), false,
                                    final_versions);
    SSA_Opd *ropd2 = cfg_to_ssa_opd(pair.second, std::make_pair(qdef.def.node, qdef.context), false,
                                    final_versions);
    if (lopd->get_type() == SSA_UsevarOpd && ropd1->get_type() == SSA_InputOpd) {
      // This case will be resolved in a different partition
      delete lopd;
      delete ropd1;
      delete stmts;
      continue;
    }
    stmts->push_back(new SSA_Stmt(SSA_AssignStmt, cfg_node->get_op(), lopd, ropd1, ropd2));

    node->add_meta(new SSA_Meta(std::make_pair(qdef.def.node, qdef.context), stmts));
  }
}

void ssa_finalize() {
  for (auto [_, proc] : *program->get_procs()) {
    for (int node : proc->get_ssa_nodes()) {
      SSA_Node *ssa_node = program->get_ssa_node(node, true);
      if (ssa_node->get_type() == SSA_AssignNode && ssa_node->get_metas()->size() == 0) {
        ssa_node->make_empty();
      }
    }
  }
}
