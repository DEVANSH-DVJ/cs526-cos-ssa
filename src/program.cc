#include "program.hh"
#include "headers.hh"
#include "ssa/ssa_compute.hh"

#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/Module.h>

#include <queue>
#include <algorithm>

using namespace std;

#include "cfg/cfg.tab.hh"
#include "ssa/ssa.tab.hh"

extern void cfg_set_in(FILE *);
extern void cfg_set_out(FILE *);

extern void ssa_set_in(FILE *);
extern void ssa_set_out(FILE *);

extern void cfg_lex_destroy();
extern void ssa_lex_destroy();

extern fstream *dot_fd;

Program::Program(string tool, string input_name, bool single_partition, bool no_opt) {
  this->tool = tool;
  this->input_name = input_name;
  this->single_partition = single_partition;
  this->no_opt = no_opt;

  if (this->tool == "cfg" || this->tool == "dfg" || this->tool == "cfg-to-ssa") {
    string cfg_file = this->input_name + ".cfg";
    cfg_set_in(fopen(cfg_file.c_str(), "r"));
    cfg_set_out(fopen("/dev/null", "w"));
  } else if (this->tool == "ssa") {
    string ssa_file = this->input_name + ".ssa";
    ssa_set_in(fopen(ssa_file.c_str(), "r"));
    ssa_set_out(fopen("/dev/null", "w"));
  } else if (this->tool == "llvm" || this->tool == "all") {
    string ll_file = this->input_name + ".ll";
    string output_ll_file = this->input_name + ".out.ll";
    llvm_module = llvm::parseIRFile(ll_file, llvm_err, llvm_context);
    CHECK_INPUT_AND_ABORT(llvm_module != NULL, "LLVM IR file could not be parsed");
    llvm_set_in(llvm_module.get());
    llvm_set_out(output_ll_file);
  } else {
    CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Unknown input type");
  }

  this->procs = new list<Procedure *>();
  this->procedures = new map<string, Procedure *>();
  this->cfg_nodes = new map<int, CFG_Node *>();
  this->cfg_edges = new map<pair<int, int>, CFG_Edge *>();
  this->ssa_nodes = new map<int, SSA_Node *>();
  this->ssa_edges = new map<pair<int, int>, SSA_Edge *>();
}

Program::~Program() {
  for (map<string, Procedure *>::iterator it = this->procedures->begin();
       it != this->procedures->end(); ++it) {
    delete it->second;
  }
  delete this->procedures;
  delete this->procs;

  for (map<int, CFG_Node *>::iterator it = this->cfg_nodes->begin();
       it != this->cfg_nodes->end(); ++it) {
    delete it->second;
  }
  delete this->cfg_nodes;

  for (map<pair<int, int>, CFG_Edge *>::iterator it = this->cfg_edges->begin();
       it != this->cfg_edges->end(); ++it) {
    delete it->second;
  }
  delete this->cfg_edges;

  for (map<int, SSA_Node *>::iterator it = this->ssa_nodes->begin();
       it != this->ssa_nodes->end(); ++it) {
    delete it->second;
  }
  delete this->ssa_nodes;

  for (map<pair<int, int>, SSA_Edge *>::iterator it = this->ssa_edges->begin();
       it != this->ssa_edges->end(); ++it) {
    delete it->second;
  }
  delete this->ssa_edges;
}

map<string, Procedure *> *Program::get_procs() { return this->procedures; }

Procedure *Program::get_proc(string name) {
  if (this->procedures->find(name) == this->procedures->end()) {
    CHECK_INPUT_AND_ABORT(false, "Procedure " + name + " not found.");
  }
  return this->procedures->find(name)->second;
}

CFG_Node *Program::get_cfg_node(int node_id, bool abort_if_not_found) {
  if (this->cfg_nodes->find(node_id) == this->cfg_nodes->end()) {
    CHECK_INPUT_AND_ABORT(!abort_if_not_found,
                          "CFG node " + to_string(node_id) + " not found.");
    CHECK_INPUT_AND_ABORT(true, "");
    CHECK_INPUT_AND_ABORT(false, "");
    return NULL;
  }
  return this->cfg_nodes->find(node_id)->second;
}

SSA_Node *Program::get_ssa_node(int node_id, bool abort_if_not_found) {
  if (this->ssa_nodes->find(node_id) == this->ssa_nodes->end()) {
    CHECK_INPUT_AND_ABORT(!abort_if_not_found,
                          "SSA node " + to_string(node_id) + " not found.");
    return NULL;
  }
  return this->ssa_nodes->find(node_id)->second;
}

llvm::Value* Program::get_llvm_node(int node_id, bool abort_if_not_found) {
  auto it = llvm_nodes.find(node_id);
  if (it == llvm_nodes.end()) {
    CHECK_INPUT_AND_ABORT(!abort_if_not_found,
                          "LLVM node " + to_string(node_id) + " not found.");
    return NULL;
  }
  return it->second;
}

llvm::Type* Program::get_llvm_type(const std::string& var) {
  return llvm_global_types[var];
}

llvm::CallInst* Program::get_llvm_call_operand_at_node(int node_id, int operand_num) {
  return llvm_call_operands_for_node[std::make_pair(node_id, operand_num)];
}

void Program::add_proc(Procedure *proc) {
  CHECK_INVARIANT(proc != NULL, "Procedure cannot be NULL.");
  string name = proc->get_name();
  CHECK_INVARIANT(this->procedures->find(name) == this->procedures->end(),
                  "Procedure " + name + " already exists.");
  this->procedures->insert(make_pair(name, proc));
}

void Program::push_proc(Procedure *proc) {
  CHECK_INVARIANT(proc != NULL, "Procedure cannot be NULL.");
  string name = proc->get_name();
  for (list<Procedure *>::iterator it = this->procs->begin();
       it != this->procs->end(); ++it) {
    CHECK_INVARIANT((*it)->get_name() != name,
                    "Procedure " + name + " already exists.");
  }
  this->procs->push_back(proc);
}

void Program::add_cfg_node(CFG_Node *node) {
  CHECK_INVARIANT(node != NULL, "CFG node cannot be NULL.");
  int node_id = node->get_node_id();
  CHECK_INPUT_AND_ABORT(this->cfg_nodes->find(node_id) ==
                            this->cfg_nodes->end(),
                        "CFG node " + to_string(node_id) + " already exists.");
  this->cfg_nodes->insert(make_pair(node_id, node));
}

void Program::add_cfg_edge(CFG_Edge *edge) {
  CHECK_INVARIANT(edge != NULL, "CFG edge cannot be NULL.");
  pair<int, int> edge_id = edge->get_edge_id();
  CHECK_INPUT_AND_ABORT(this->cfg_edges->find(edge_id) ==
                            this->cfg_edges->end(),
                        "CFG edge (" + to_string(edge_id.first) + ", " +
                            to_string(edge_id.second) + ") already exists.");
  this->cfg_edges->insert(make_pair(edge_id, edge));
}

void Program::add_ssa_node(SSA_Node *node) {
  CHECK_INVARIANT(node != NULL, "SSA node cannot be NULL.");
  int node_id = node->get_node_id();
  CHECK_INPUT_AND_ABORT(this->ssa_nodes->find(node_id) ==
                            this->ssa_nodes->end(),
                        "SSA node " + to_string(node_id) + " already exists.");
  this->ssa_nodes->insert(make_pair(node_id, node));
}

void Program::add_ssa_edge(SSA_Edge *edge) {
  CHECK_INVARIANT(edge != NULL, "SSA edge cannot be NULL.");
  pair<int, int> edge_id = edge->get_edge_id();
  CHECK_INPUT_AND_ABORT(this->ssa_edges->find(edge_id) ==
                            this->ssa_edges->end(),
                        "SSA edge (" + to_string(edge_id.first) + ", " +
                            to_string(edge_id.second) + ") already exists.");
  this->ssa_edges->insert(make_pair(edge_id, edge));
}

void Program::map_node_to_llvm(int node, llvm::Value* value) {
  llvm_nodes[node] = value;
}

void Program::map_var_to_llvm_type(const std::string& var, llvm::Type* type) {
  llvm_global_types[var] = type;
}

void Program::add_llvm_call_operand_at_node(int node, int operand_num, llvm::CallInst* call) {
  llvm_call_operands_for_node[std::make_pair(node, operand_num)] = call;
}

void Program::parse_cfg_from_llvm() { llvm_parse(); }

void Program::parse_cfg() { cfg_parse(); cfg_lex_destroy(); }

void Program::parse_ssa() { ssa_parse(); ssa_lex_destroy(); }

void Program::construct_dfg() { dfg_construct(); }

void Program::propagate_dfg_constants() { dfgs[cur_partition].propagated_values = dfg_propagate_constants(); }

void Program::reduce_dfg() {
  dfg_reduce();
}

void Program::detect_dead_dfg_qdefs() { dfgs[cur_partition].dead_qdefs = dfg_detect_dead_qdefs(); }

void Program::init_ssa() {
  ssa_init();
}

void Program::construct_ssa_partition() {
  ssa_construct_partition();
}

void Program::finalize_ssa() {
  ssa_finalize();
}

void Program::deconstruct_ssa() {
  ssa_deconstruct();
}

void Program::dump_cfg() {
  string cfg_file = input_name + ".cfg";

  CHECK_INVARIANT(dot_fd == NULL, "Dot file descriptor must be NULL.");
  dot_fd = new fstream(cfg_file.c_str(), ios::out | ios::trunc);

  bool first = true;
  for (Procedure* proc : *procs) {
    if (first) {
      first = false;
    } else {
      *dot_fd << ", ";
    }

    *dot_fd << proc->get_name();
  }
  *dot_fd << ";";

  for (Procedure* proc : *procs) {
    proc->dump_cfg();
  }

  dot_fd->close();
  delete dot_fd;
  dot_fd = NULL;
}

void Program::visualize_cfg() {
  string dot_file = input_name + ".cfg.dot";
  string png_file = input_name + ".cfg.png";

  CHECK_INVARIANT(dot_fd == NULL, "Dot file descriptor must be NULL.");
  dot_fd = new fstream(dot_file.c_str(), ios::out | ios::trunc);

  *dot_fd << "digraph G {\n";
  for (list<Procedure *>::iterator it = this->procs->begin();
       it != this->procs->end(); ++it) {
    (*it)->visualize_cfg();
  }
  *dot_fd << "}\n";

  dot_fd->close();
  delete dot_fd;
  dot_fd = NULL;

  if (system(("dot -Tpng " + dot_file + " -o " + png_file).c_str()) != 0) {
    CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Error generating png file\n");
  }
}

// Prints out a QDef in the form "var_node_context [= value]" to stdout
void print_qdef(QDef node, const std::map<QDef, int>& propagated_values) {
  std::cout << node.def.var_name + '_' + std::to_string(node.def.node) + '_' + std::to_string(node.context);
  auto it = propagated_values.find(node);
  if (it != propagated_values.end()) {
    std::cout << " = " << it->second;
  }
}

void Program::visualize_dfg() {
  DFG& dfg = dfgs[cur_partition];
  std::cout << dfg.context_table.to_string() << '\n';

  // Print out the contexts
  for (auto& [node, context_map] : dfg.context_transitions) {
    for (auto [to_context, from_context] : context_map) {
      std::cout << "Context transition at node " << node << ": " << to_context << " -> " << from_context << '\n';
    }
  }
  std::cout << '\n';

  std::vector<QDef> nodes (dfg.nodes.begin(), dfg.nodes.end());
  // Order qdefs for consistent printing
  std::sort(nodes.begin(), nodes.end(), [](QDef l, QDef r) {
    return l.def.node < r.def.node;
  });

  // Print out the qdefs and their dependencies
  for (QDef node : nodes) {
    print_qdef(node, dfg.propagated_values);
    std::cout << " <- ";
    bool first = true;
    for (QDef incoming : dfg.reverse_edges[node]) {
      if (first) {
        first = false;
      } else {
        std::cout << ", ";
      }
      print_qdef(incoming, dfg.propagated_values);
    }
    std::cout << '\n';
  }
}

void Program::dump_ssa() {
  string ssa_file = input_name + ".ssa";

  CHECK_INVARIANT(dot_fd == NULL, "Dot file descriptor must be NULL.");
  dot_fd = new fstream(ssa_file.c_str(), ios::out | ios::trunc);

  bool first = true;
  for (Procedure* proc : *procs) {
    if (first) {
      first = false;
    } else {
      *dot_fd << ", ";
    }

    *dot_fd << proc->get_name();
  }
  *dot_fd << ";";

  for (Procedure* proc : *procs) {
    proc->dump_ssa();
  }

  dot_fd->close();
  delete dot_fd;
  dot_fd = NULL;
}

void Program::visualize_ssa() {
  string dot_file = input_name + ".ssa.dot";
  string png_file = input_name + ".ssa.png";

  CHECK_INVARIANT(dot_fd == NULL, "Dot file descriptor must be NULL.");
  dot_fd = new fstream(dot_file.c_str(), ios::out | ios::trunc);

  *dot_fd << "digraph G {\n";
  for (list<Procedure *>::iterator it = this->procs->begin();
       it != this->procs->end(); ++it) {
    (*it)->visualize_ssa();
  }
  *dot_fd << "}\n";

  dot_fd->close();
  delete dot_fd;

  if (system(("dot -Tpng " + dot_file + " -o " + png_file).c_str()) != 0) {
    CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Error generating png file\n");
  }
}

void Program::dump_llvm() {
  llvm_dump();
}

// Creates a subgroup of globals that forms a connected component
// in the interactions graph
std::set<std::string> create_partition(std::set<std::string>& globals,
                                       std::map<std::string, std::set<std::string>>& interactions) {
  CHECK_INVARIANT(globals.size() > 0, "Cannot partition empty globals");
  std::set<std::string> partition;
  std::queue<std::string> worklist;

  auto it = globals.begin();
  worklist.push(*it);
  partition.insert(*it);
  globals.erase(it);
  // Loop over all reachible nodes and add them to the partition
  while (!worklist.empty()) {
    std::string cur = worklist.front();
    worklist.pop();
    for (const std::string& neighbor : interactions[cur]) {
      auto it = globals.find(neighbor);
      if (it != globals.end()) {
        worklist.push(neighbor);
        partition.insert(neighbor);
        globals.erase(it);
      }
    }
  }

  return partition;
}

void Program::partition_globals() {
  std::set<std::string> globals;
  std::map<std::string, std::set<std::string>> interactions;
  // Create a graph of globals where an edge represents a dependency between two globals
  for (auto [_, cfg_node] : *cfg_nodes) {
    if (cfg_node->get_type() != CFG_NodeType::CFG_AssignNode) {
      continue;
    }
    CFG_Opd* lopd = cfg_node->get_lopd();
    std::string def = "";
    if (lopd->get_type() == CFG_OpdType::CFG_VarOpd) {
      def = lopd->get_opd_var();
      if (globals.find(def) == globals.end()) {
        globals.insert(def);
      }
    }
    for (const std::string& use : cfg_node->get_uses()) {
      globals.insert(use);
      if (def != "" && def != use) {
        if (def == use) {
          continue;
        }
        interactions[def].insert(use);
        interactions[use].insert(def);
      }
    }
  }

  if (single_partition) {
    partitions.push_back(globals);
    dfgs.resize(1);
    return;
  }

  // Construct the partitions from the interactions graph
  while (!globals.empty()) {
    partitions.push_back(create_partition(globals, interactions));
  }
  // Create a DFG for each partition
  dfgs.resize(partitions.size());
}

void Program::set_cur_partition(int partition) {
  cur_partition = partition;
}

int Program::get_num_partitions() {
  return partitions.size();
}

bool Program::is_in_cur_partition(CFG_Opd* opd) {
  return opd->get_type() == CFG_OpdType::CFG_VarOpd
      && partitions[cur_partition].find(opd->get_opd_var()) != partitions[cur_partition].end();
}

bool Program::is_part_of_other_partition(int node) {
  if (node == 0) {
    return false;
  }
  CFG_Node* cfg_node = get_cfg_node(node, true);
  if (cfg_node->get_type() != CFG_AssignNode) {
    // Non assign nodes can be processed by any partition
    return false;
  }
  if (cfg_node->get_lopd()->get_type() == CFG_OpdType::CFG_VarOpd) {
    // Return whether this global variable is not part of this partition
    return partitions[cur_partition].find(cfg_node->get_def()) == partitions[cur_partition].end();
  }
  // This is a USEVAR: return whether it depends on globals in a different partition
  for (const std::string& def : cfg_node->get_uses()) {
    if (partitions[cur_partition].find(def) == partitions[cur_partition].end()) {
      return true;
    }
  }
  return false;
}

std::set<std::string> Program::get_globals() {
  std::set<std::string> res;
  for (Procedure* proc : *procs) {
    for (std::string global : proc->get_globals()) {
      res.insert(global);
    }
  }
  return res;
}

std::set<QDef> Program::get_dfg_nodes() {
  return dfgs[cur_partition].nodes;
}

std::set<QDef> Program::get_dfg_incoming(QDef node) {
  return dfgs[cur_partition].reverse_edges[node];
}

std::set<QDef> Program::get_dfg_outgoing(QDef node) {
  return dfgs[cur_partition].edges[node];
}

bool Program::create_dfg_transition(QNode from_qnode, const Context& to_context) {
  DFG& dfg = dfgs[cur_partition];
  auto it = dfg.context_transitions[from_qnode.node].find(from_qnode.context);
  int new_context = dfg.context_table.insert_context(to_context);
  if (it != dfg.context_transitions[from_qnode.node].end()) {
    // We are updating an existing transition
    if (new_context != it->second) {
      dfg.context_transitions[from_qnode.node][from_qnode.context] = new_context;
      // Get rid of any existing reverse transition and set the new reverse transition
      auto transitionIt = dfg.reverse_context_transitions[it->second].find(from_qnode);
      if (transitionIt != dfg.reverse_context_transitions[it->second].end()) {
        dfg.reverse_context_transitions[it->second].erase(dfg.reverse_context_transitions[it->second].find(from_qnode));
      }
      dfg.reverse_context_transitions[new_context].insert(from_qnode);
      return true;
    }
    return false;
  }

  // This is a new context transition, so add it to the context table
  dfg.context_transitions[from_qnode.node][from_qnode.context] = new_context;
  dfg.reverse_context_transitions[new_context].insert(from_qnode);
  return true;
}

std::map<int, int>::iterator Program::get_dfg_transition(QNode from_qnode) {
  return dfgs[cur_partition].context_transitions[from_qnode.node].find(from_qnode.context);
}

std::map<int, int>& Program::get_dfg_transitions(int node) {
  return dfgs[cur_partition].context_transitions[node];
}

std::map<int, int>::iterator Program::dfg_transitions_end(int node) {
  return dfgs[cur_partition].context_transitions[node].end();
}

std::map<int, std::set<QNode>>::iterator Program::get_dfg_reverse_transitions(int to_context) {
  return dfgs[cur_partition].reverse_context_transitions.find(to_context);
}

  std::map<int, std::set<QNode>>::iterator Program::dfg_reverse_transitions_end() {
  return dfgs[cur_partition].reverse_context_transitions.end();
}

bool Program::get_dfg_propagated_value(QDef qdef, int* value) {
  DFG& dfg = dfgs[cur_partition];
  auto it = dfg.propagated_values.find(qdef);
  if (it != dfg.propagated_values.end()) {
    *value = it->second;
    return true;
  }
  return false;
}

bool Program::dfg_is_dead(QDef qdef) {
  DFG& dfg = dfgs[cur_partition];
  return dfg.dead_qdefs.find(qdef) != dfg.dead_qdefs.end();
}

int Program::insert_dfg_context(Context context) {
  return dfgs[cur_partition].context_table.insert_context(context);
}

void Program::add_dfg_node(QDef node) {
  dfgs[cur_partition].nodes.insert(node);
}

void Program::remove_dfg_node(QDef node) {
  DFG& dfg = dfgs[cur_partition];
  CHECK_INVARIANT(dfg.nodes.find(node) != dfg.nodes.end(), "QDef is not an existing node");

  dfg.nodes.erase(dfg.nodes.find(node));
  // Erase any edges using this node as a src
  for (QDef dest : dfg.edges[node]) {
    dfg.reverse_edges[dest].erase(dfg.reverse_edges[dest].find(node));
  }
  dfg.edges.erase(dfg.edges.find(node));
  // Erase any edges using this node as a dest
  for (QDef src : dfg.reverse_edges[node]) {
    dfg.edges[src].erase(dfg.edges[src].find(node));
  }
  dfg.reverse_edges.erase(dfg.reverse_edges.find(node));
}

void Program::add_dfg_edge(QDef src, QDef dest) {
  DFG& dfg = dfgs[cur_partition];
  dfg.nodes.insert(src);
  dfg.nodes.insert(dest);
  dfg.edges[src].insert(dest);
  dfg.reverse_edges[dest].insert(src);
}

void Program::remove_dfg_edge(QDef src, QDef dest) {
  DFG& dfg = dfgs[cur_partition];
  dfg.edges[src].erase(dfg.edges[src].find(dest));
  dfg.reverse_edges[dest].erase(dfg.reverse_edges[dest].find(src));
}

void Program::run(bool debug) {
  if (this->tool == "cfg") {
    this->parse_cfg();
    this->visualize_cfg();
  } else if (this->tool == "ssa") {
    this->parse_ssa();
    this->visualize_ssa();
  } else if (this->tool == "dfg") {
    this->parse_cfg();
    this->partition_globals();
    cur_partition = 0;
    this->construct_dfg();

    if (!this->no_opt) {
      this->propagate_dfg_constants();
      this->reduce_dfg();
    }

    this->visualize_dfg();
  } else if (this->tool == "cfg-to-ssa") {
    this->parse_cfg();
    this->partition_globals();

    this->init_ssa();
    for (cur_partition = 0; cur_partition < partitions.size(); ++cur_partition) {
      this->construct_dfg();

      if (!this->no_opt) {
        this->propagate_dfg_constants();
        this->reduce_dfg();
        this->detect_dead_dfg_qdefs();
      }

      this->construct_ssa_partition();
    }
    this->finalize_ssa();

    this->dump_ssa();
  } else if (this->tool == "llvm") {
    this->parse_cfg_from_llvm();
    this->dump_cfg();
  } else if (this->tool == "all") {
    this->parse_cfg_from_llvm();
    this->partition_globals();

    if (debug) {
      // this->dump_cfg();
      std::cout << "#Procedures: " << this->procs->size() << '\n';
      std::cout << "CFG nodes: " << cfg_nodes->size() << '\n';
      std::cout << "Partitions: " << partitions.size() << '\n';
      int total = 0;
      int max = 0;
      for (const auto &partition : partitions) {
        total += partition.size();
        if (partition.size() > max) {
          max = partition.size();
        }
      }
      std::cout << "Max partition size: " << max << '\n';
      std::cout << "Total size: " << total << '\n';
      std::cout << "Total globals: " << get_globals().size() << '\n';
    }

    this->init_ssa();
    // For each partition, construct its DFG and use that DFG to fill in
    // the corresponding parts of the SSA graph
    for (cur_partition = 0; cur_partition < partitions.size(); ++cur_partition) {
      this->construct_dfg();

      if (!this->no_opt) {
        this->propagate_dfg_constants();
        this->reduce_dfg();
        this->detect_dead_dfg_qdefs();
      }

      this->construct_ssa_partition();
    }
    this->finalize_ssa();

    if (debug) {
      int total_dfg[4] = {0, 0, 0, 0};
      int max_dfg[4] = {0, 0, 0, 0};
      for (const auto &dfg : dfgs) {
        // std::cout << dfg.context_table.to_string() << '\n';
        // DFG size
        total_dfg[0] += dfg.nodes.size();
        if (dfg.nodes.size() > max_dfg[0]) {
          max_dfg[0] = dfg.nodes.size();
        }
        // Number of contexts
        total_dfg[1] += dfg.context_table.next_context - 1;
        if (dfg.context_table.next_context - 1 > max_dfg[1]) {
          max_dfg[1] = dfg.context_table.next_context - 1;
        }
        // Number of propagated values
        total_dfg[2] += dfg.propagated_values.size();
        if (dfg.propagated_values.size() > max_dfg[2]) {
          max_dfg[2] = dfg.propagated_values.size();
        }
        // Number of dead QDefs
        total_dfg[3] += dfg.dead_qdefs.size();
        if (dfg.dead_qdefs.size() > max_dfg[3]) {
          max_dfg[3] = dfg.dead_qdefs.size();
        }
      }
      std::cout << "Number of DFGs: " << dfgs.size() << '\n';
      std::cout << "Max DFG size: " << max_dfg[0] << '\n';
      std::cout << "Total DFG size: " << total_dfg[0] << '\n';
      std::cout << "Max number of contexts: " << max_dfg[1] << '\n';
      std::cout << "Total number of contexts: " << total_dfg[1] << '\n';
      std::cout << "Max number of propagated values: " << max_dfg[2] << '\n';
      std::cout << "Total number of propagated values: " << total_dfg[2] << '\n';
      std::cout << "Max number of dead QDefs: " << max_dfg[3] << '\n';
      std::cout << "Total number of dead QDefs: " << total_dfg[3] << '\n';
      // this->dump_ssa();
    }
    
    this->deconstruct_ssa();
    this->dump_llvm();
  } else {
    CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Unknown input type");
  }
}
