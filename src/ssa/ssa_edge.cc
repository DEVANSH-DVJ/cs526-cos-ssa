#include "../headers.hh"

using namespace std;

extern fstream *dot_fd;
extern Program *program;

SSA_Edge::SSA_Edge(int src_id, int dst_id) {
  this->src = program->get_ssa_node(src_id, true);
  this->dst = program->get_ssa_node(dst_id, true);
  this->edge_id = make_pair(src_id, dst_id);
}

SSA_Edge::~SSA_Edge() {}

pair<int, int> &SSA_Edge::get_edge_id() { return this->edge_id; }

SSA_Node *SSA_Edge::get_src() { return this->src; }

SSA_Node *SSA_Edge::get_dst() { return this->dst; }

void SSA_Edge::visualize() {
  *dot_fd << "\t\tnode_" << this->src->get_node_id() << " -> "
          << "node_" << this->dst->get_node_id() << ";\n";
}
