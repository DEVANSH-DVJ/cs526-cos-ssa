#include "ssa_compute.hh"
#include "../headers.hh"

extern Program* program;

void ssa_init() {
  // Create SSA nodes
  for (auto pair : *program->get_procs()) {
    for (int node : pair.second->get_cfg_nodes()) {
      CFG_Node* cfg_node = program->get_cfg_node(node, true);

      SSA_Node* ssa_node;
      switch (cfg_node->get_type()) {
        case CFG_StartNode:
          ssa_node = new SSA_Node(SSA_StartNode, node, "START " + pair.first);
          break;
        case CFG_EndNode:
          ssa_node = new SSA_Node(SSA_EndNode, node, "END " + pair.first);
          break;
        case CFG_CallNode:
          ssa_node = new SSA_Node(SSA_CallNode, node, "CALL " + cfg_node->get_callee(), cfg_node->get_callee());
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
      pair.second->add_ssa_node(ssa_node);
    }
  }

  // Create SSA edges
  for (auto pair : *program->get_procs()) {
    for (int src : pair.second->get_cfg_nodes()) {
      for (int dst : program->get_cfg_node(src, true)->get_successors()) {
        SSA_Edge* edge = new SSA_Edge(src, dst);
        program->add_ssa_edge(edge);
        pair.second->add_ssa_edge(edge);
      }
    }
  }
}

SSA_Opd* cfg_to_ssa_opd(CFG_Opd* cfg_opd, std::pair<int, int> meta_num, bool is_def, std::map<std::string, SSA_Opd*>& final_versions) {
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
  for (QDef qdef : program->get_ddg_nodes()) {
    if (qdef.def.node == 0 || program->is_part_of_other_partition(qdef.def.node) || program->ddg_is_dead(qdef)) {
      continue;
    }

    CFG_Node* cfg_node = program->get_cfg_node(qdef.def.node, true);
    std::list<SSA_Stmt*>* stmts = new std::list<SSA_Stmt*>();
    std::set<QDef> deps = program->get_ddg_incoming(qdef);
    std::map<string, std::list<std::pair<int, int>>> versions;
    std::map<string, SSA_Opd*> final_versions;
    for (QDef dep : deps) {
      versions[dep.def.var_name].push_back(std::make_pair(dep.def.node, dep.context));
    }

    SSA_Node* node = program->get_ssa_node(qdef.def.node, true);
    SSA_Opd* lopd = cfg_to_ssa_opd(cfg_node->get_lopd(), std::make_pair(qdef.def.node, qdef.context), true, final_versions);
    int value;
    if (program->get_ddg_propagated_value(qdef, &value)) {
      // Use the propagated value
      stmts->push_back(new SSA_Stmt(SSA_AssignStmt, "=", lopd, new SSA_Opd(SSA_NumOpd, value), nullptr));
      node->add_meta(new SSA_Meta(std::make_pair(qdef.def.node, qdef.context), stmts));
      continue;
    }

    // Add phi nodes
    for (auto pair : versions) {
      if (pair.second.size() == 1) {
        final_versions[pair.first] = new SSA_Opd(SSA_VarOpd, *pair.second.begin(), pair.first);
        continue;
      }
      CHECK_INVARIANT(pair.second.size() >= 2, "Expected at least two incoming defs");

      std::list<SSA_Opd*>* ropds = new std::list<SSA_Opd*>();
      for (auto meta_num : pair.second) {
        ropds->push_back(new SSA_Opd(SSA_VarOpd, meta_num, pair.first));
      }

      stmts->push_back(new SSA_Stmt(SSA_PhiStmt, new SSA_Opd(SSA_PhiOpd, std::make_pair(qdef.def.node, qdef.context), pair.first), ropds));
      final_versions[pair.first] = new SSA_Opd(SSA_PhiOpd, std::make_pair(qdef.def.node, qdef.context), pair.first);
    }

    auto pair = cfg_node->get_ropds();
    SSA_Opd* ropd1 = cfg_to_ssa_opd(pair.first, std::make_pair(qdef.def.node, qdef.context), false, final_versions);
    SSA_Opd* ropd2 = cfg_to_ssa_opd(pair.second, std::make_pair(qdef.def.node, qdef.context), false, final_versions);
    if (lopd->get_type() == SSA_UsevarOpd && ropd1->get_type() == SSA_InputOpd) {
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
  for (auto pair : *program->get_procs()) {
    for (int node : pair.second->get_ssa_nodes()) {
      SSA_Node* ssa_node = program->get_ssa_node(node, true);
      if (ssa_node->get_type() == SSA_AssignNode && ssa_node->get_metas()->size() == 0) {
        ssa_node->make_empty();
      }
    }
  }
}
