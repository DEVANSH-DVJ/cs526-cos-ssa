#include "../headers.hh"

using namespace std;

extern fstream *dot_fd;

SSA_Node::SSA_Node(SSA_NodeType type, int node_id, string stmt) {
  if (type != SSA_StartNode && type != SSA_EndNode) {
    CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH,
                    "SSA_StartNode or SSA_EndNode expected");
  }

  this->type = type;
  this->node_id = node_id;

  this->parent_proc = "";
  this->in_edges = new map<int, SSA_Edge *>();
  this->out_edges = new map<int, SSA_Edge *>();

  this->stmt = stmt;
  this->callee_proc = "";
  this->metas = NULL;
}

SSA_Node::SSA_Node(SSA_NodeType type, int node_id, string stmt,
                   string callee_proc) {
  if (type != SSA_CallNode) {
    CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "SSA_CallNode expected");
  }

  this->type = type;
  this->node_id = node_id;

  this->parent_proc = "";
  this->in_edges = new map<int, SSA_Edge *>();
  this->out_edges = new map<int, SSA_Edge *>();

  this->stmt = stmt;
  this->callee_proc = callee_proc;
  this->metas = NULL;
}

SSA_Node::SSA_Node(SSA_NodeType type, int node_id) {
  if (type != SSA_AssignNode) {
    CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "SSA_AssignNode expected");
  }

  this->type = type;
  this->node_id = node_id;

  this->parent_proc = "";
  this->in_edges = new map<int, SSA_Edge *>();
  this->out_edges = new map<int, SSA_Edge *>();

  this->stmt = "";
  this->callee_proc = "";
  this->metas = new map<int, SSA_Meta *>();
}

SSA_Node::~SSA_Node() {
  delete this->in_edges;
  delete this->out_edges;

  if (this->metas != NULL) {
    for (map<int, SSA_Meta *>::iterator it = this->metas->begin();
         it != this->metas->end(); ++it) {
      delete it->second;
    }
    delete this->metas;
  }
}

SSA_NodeType SSA_Node::get_type() { return this->type; }

int SSA_Node::get_node_id() { return this->node_id; }

string &SSA_Node::get_parent_proc() { return this->parent_proc; }

void SSA_Node::set_parent_proc(string parent_proc) {
  CHECK_INVARIANT(this->parent_proc == "", "Parent proc already set");
  this->parent_proc = parent_proc;
}

void SSA_Node::add_in_edge(SSA_Edge *edge) {
  CHECK_INVARIANT(edge != NULL, "Edge cannot be NULL");
  int from_node_id = edge->get_src()->get_node_id();
  CHECK_INVARIANT(this->in_edges->find(from_node_id) == this->in_edges->end(),
                  "Edge already exists");
  this->in_edges->insert(make_pair(from_node_id, edge));
}

void SSA_Node::add_out_edge(SSA_Edge *edge) {
  CHECK_INVARIANT(edge != NULL, "Edge cannot be NULL");
  int to_node_id = edge->get_dst()->get_node_id();
  CHECK_INVARIANT(this->out_edges->find(to_node_id) == this->out_edges->end(),
                  "Edge already exists");
  this->out_edges->insert(make_pair(to_node_id, edge));
}

void SSA_Node::add_meta(SSA_Meta *meta) {
  CHECK_INVARIANT(meta != NULL, "Meta cannot be NULL");
  int meta_id = meta->get_meta_num().second;
  CHECK_INVARIANT(this->type == SSA_AssignNode,
                  "Meta can only be added to Assignment node");
  CHECK_INVARIANT(this->metas->find(meta_id) == this->metas->end(),
                  "Meta already exists");
  this->metas->insert(make_pair(meta_id, meta));
}

void SSA_Node::visualize() {
  if (this->type != SSA_AssignNode) {
    *dot_fd << "\t\tnode_" << this->node_id << " [shape=box, xlabel=\""
            << this->node_id << "\", label=\"" << this->stmt << "\"];\n";
  } else {
    *dot_fd << "\t\tnode_" << this->node_id << " [shape=record, xlabel=\""
            << this->node_id << "\", label=\"";
    for (map<int, SSA_Meta *>::iterator it = this->metas->begin();
         it != this->metas->end(); ++it) {
      it->second->visualize();
      if (it != --this->metas->end()) {
        *dot_fd << " | ";
      }
    }
    *dot_fd << "\"];\n";
  }
}
