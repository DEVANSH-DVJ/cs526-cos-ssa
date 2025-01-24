#include "../headers.hh"

using namespace std;

extern fstream *dot_fd;

CFG_Node::CFG_Node(CFG_NodeType type, int node_id, string stmt) {
  if (type != CFG_StartNode && type != CFG_EndNode) {
    CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH,
                    "CFG_StartNode or CFG_EndNode expected");
  }

  this->type = type;
  this->node_id = node_id;

  this->parent_proc = "";
  this->stmt = stmt;
  this->in_edges = new map<int, CFG_Edge *>();
  this->out_edges = new map<int, CFG_Edge *>();

  this->callee_proc = "";
  this->op = "";
  this->lopd = NULL;
  this->ropd1 = NULL;
  this->ropd2 = NULL;
}

CFG_Node::CFG_Node(CFG_NodeType type, int node_id, string stmt,
                   string callee_proc) {
  if (type != CFG_CallNode) {
    CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "CFG_CallNode expected");
  }

  this->type = type;
  this->node_id = node_id;

  this->parent_proc = "";
  this->stmt = stmt;
  this->in_edges = new map<int, CFG_Edge *>();
  this->out_edges = new map<int, CFG_Edge *>();

  this->callee_proc = callee_proc;
  this->op = "";
  this->lopd = NULL;
  this->ropd1 = NULL;
  this->ropd2 = NULL;
}

CFG_Node::CFG_Node(CFG_NodeType type, int node_id, string op, CFG_Opd *lopd,
                   CFG_Opd *ropd1, CFG_Opd *ropd2) {
  if (type != CFG_AssignNode) {
    CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "CFG_AssignNode expected");
  }

  this->type = type;
  this->node_id = node_id;

  this->parent_proc = "";
  this->in_edges = new map<int, CFG_Edge *>();
  this->out_edges = new map<int, CFG_Edge *>();

  this->callee_proc = "";
  this->op = op;
  this->lopd = lopd;
  this->ropd1 = ropd1;
  this->ropd2 = ropd2;

  if (ropd2 == NULL) {
    CHECK_INVARIANT(op == "=", "Assignment operator expected");
    this->stmt = lopd->str() + " = " + ropd1->str();
  } else {
    this->stmt =
        lopd->str() + " = " + ropd1->str() + " " + op + " " + ropd2->str();
  }
}

CFG_Node::~CFG_Node() {
  delete this->in_edges;
  delete this->out_edges;

  if (this->lopd != NULL)
    delete this->lopd;

  if (this->ropd1 != NULL)
    delete this->ropd1;

  if (this->ropd2 != NULL)
    delete this->ropd2;
}

CFG_NodeType CFG_Node::get_type() { return this->type; }

int CFG_Node::get_node_id() { return this->node_id; }

string &CFG_Node::get_parent_proc() { return this->parent_proc; }

void CFG_Node::set_parent_proc(string parent_proc) {
  CHECK_INVARIANT(this->parent_proc == "", "Parent proc already set");
  this->parent_proc = parent_proc;
}

void CFG_Node::add_in_edge(CFG_Edge *edge) {
  CHECK_INVARIANT(edge != NULL, "Edge cannot be NULL");
  int from_node_id = edge->get_src()->get_node_id();
  CHECK_INVARIANT(this->in_edges->find(from_node_id) == this->in_edges->end(),
                  "Edge already exists");
  this->in_edges->insert(make_pair(from_node_id, edge));
}

void CFG_Node::add_out_edge(CFG_Edge *edge) {
  CHECK_INVARIANT(edge != NULL, "Edge cannot be NULL");
  int to_node_id = edge->get_dst()->get_node_id();
  CHECK_INVARIANT(this->out_edges->find(to_node_id) == this->out_edges->end(),
                  "Edge already exists");
  this->out_edges->insert(make_pair(to_node_id, edge));
}

void CFG_Node::visualize() {
  *dot_fd << "\t\tnode_" << this->node_id << " [shape=box, xlabel=\""
          << this->node_id << "\", label=\"" << this->stmt << "\"];\n";
}
