#include "headers.hh"

using namespace std;

extern fstream *dot_fd;

Procedure::Procedure(string name) {
  this->name = name;
  this->cfg_nodes = new map<int, CFG_Node *>();
  this->cfg_edges = new map<pair<int, int>, CFG_Edge *>();
  this->ssa_nodes = new map<int, SSA_Node *>();
  this->ssa_edges = new map<pair<int, int>, SSA_Edge *>();
}

Procedure::~Procedure() {
  delete this->cfg_nodes;
  delete this->cfg_edges;
  delete this->ssa_nodes;
  delete this->ssa_edges;
}

string Procedure::get_name() { return this->name; }

void Procedure::add_cfg_node(CFG_Node *node) {
  CHECK_INVARIANT(node != NULL, "CFG node cannot be NULL.");
  int node_id = node->get_node_id();
  CHECK_INVARIANT(this->cfg_nodes->find(node_id) == this->cfg_nodes->end(),
                  "CFG node " + to_string(node_id) + " already exists.");
  this->cfg_nodes->insert(make_pair(node_id, node));
}

void Procedure::add_cfg_edge(CFG_Edge *edge) {
  CHECK_INVARIANT(edge != NULL, "CFG edge cannot be NULL.");
  pair<int, int> edge_id = edge->get_edge_id();
  CHECK_INVARIANT(this->cfg_edges->find(edge_id) == this->cfg_edges->end(),
                  "CFG edge (" + to_string(edge_id.first) + ", " +
                      to_string(edge_id.second) + ") already exists.");
  this->cfg_edges->insert(make_pair(edge_id, edge));
}

void Procedure::add_ssa_node(SSA_Node *node) {
  CHECK_INVARIANT(node != NULL, "SSA node cannot be NULL.");
  int node_id = node->get_node_id();
  CHECK_INVARIANT(this->ssa_nodes->find(node_id) == this->ssa_nodes->end(),
                  "SSA node " + to_string(node_id) + " already exists.");
  this->ssa_nodes->insert(make_pair(node_id, node));
}

void Procedure::add_ssa_edge(SSA_Edge *edge) {
  CHECK_INVARIANT(edge != NULL, "SSA edge cannot be NULL.");
  pair<int, int> edge_id = edge->get_edge_id();
  CHECK_INVARIANT(this->ssa_edges->find(edge_id) == this->ssa_edges->end(),
                  "SSA edge (" + to_string(edge_id.first) + ", " +
                      to_string(edge_id.second) + ") already exists.");
  this->ssa_edges->insert(make_pair(edge_id, edge));
}

void Procedure::visualize_cfg() {
  *dot_fd << "\n\tsubgraph cluster_" << this->name << " {\n";
  *dot_fd << "\t\tlabel = \"" << this->name << "\";\n";
  *dot_fd << "\t\tmargin = 25;\n";
  for (map<int, CFG_Node *>::iterator it = this->cfg_nodes->begin();
       it != this->cfg_nodes->end(); ++it) {
    it->second->visualize();
  }
  for (map<pair<int, int>, CFG_Edge *>::iterator it = this->cfg_edges->begin();
       it != this->cfg_edges->end(); ++it) {
    it->second->visualize();
  }
  *dot_fd << "\t}\n";
}

void Procedure::visualize_ssa() {
  *dot_fd << "\n\tsubgraph cluster_" << this->name << " {\n";
  *dot_fd << "\t\tlabel = \"" << this->name << "\";\n";
  *dot_fd << "\t\tmargin = 25;\n";
  for (map<int, SSA_Node *>::iterator it = this->ssa_nodes->begin();
       it != this->ssa_nodes->end(); ++it) {
    it->second->visualize();
  }
  for (map<pair<int, int>, SSA_Edge *>::iterator it = this->ssa_edges->begin();
       it != this->ssa_edges->end(); ++it) {
    it->second->visualize();
  }
  *dot_fd << "\t}\n";
}
