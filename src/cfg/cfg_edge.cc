#include "../headers.hh"

using namespace std;

extern fstream *dot_fd;
extern Program *program;

CFG_Edge::CFG_Edge(int src_id, int dst_id) {
  this->src = program->get_cfg_node(src_id, true);
  this->dst = program->get_cfg_node(dst_id, true);
  this->edge_id = make_pair(src_id, dst_id);
}

CFG_Edge::~CFG_Edge() {}

pair<int, int> &CFG_Edge::get_edge_id() { return this->edge_id; }

CFG_Node *CFG_Edge::get_src() { return this->src; }

CFG_Node *CFG_Edge::get_dst() { return this->dst; }

void CFG_Edge::visualize() {
  *dot_fd << "\t\tnode_" << this->src->get_node_id() << " -> "
          << "node_" << this->dst->get_node_id() << ";\n";
}
