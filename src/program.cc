#include "headers.hh"

using namespace std;

#include "cfg/cfg.tab.hh"
#include "ssa/ssa.tab.hh"

extern void cfg_set_in(FILE *);
extern void cfg_set_out(FILE *);

extern void ssa_set_in(FILE *);
extern void ssa_set_out(FILE *);

extern fstream *dot_fd;

Program::Program(string tool, string input_name) {
  this->tool = tool;
  this->input_name = input_name;

  if (this->tool == "cfg") {
    string cfg_file = this->input_name + ".cfg";
    cfg_set_in(fopen(cfg_file.c_str(), "r"));
    cfg_set_out(fopen("/dev/null", "w"));
  } else if (this->tool == "ssa") {
    string ssa_file = this->input_name + ".ssa";
    ssa_set_in(fopen(ssa_file.c_str(), "r"));
    ssa_set_out(fopen("/dev/null", "w"));
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

void Program::parse_cfg() { cfg_parse(); }

void Program::parse_ssa() { ssa_parse(); }

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

  if (system(("dot -Tpng " + dot_file + " -o " + png_file).c_str()) != 0) {
    CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Error generating png file\n");
  }
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

void Program::cleanup() {}

void Program::run() {
  if (this->tool == "cfg") {
    this->parse_cfg();
    this->visualize_cfg();
  } else if (this->tool == "ssa") {
    this->parse_ssa();
    this->visualize_ssa();
  } else {
    CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Unknown input type");
  }
}
