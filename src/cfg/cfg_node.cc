#include "headers.hh"

using namespace std;

extern fstream *dot_fd;

CFG_Node::CFG_Node(CFG_NodeType type, int node_id, string stmt) {
  if (type == CFG_EmptyNode) {
    stmt = "EMPTY";
  } else if (type != CFG_StartNode && type != CFG_EndNode) {
    CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "CFG_StartNode or CFG_EndNode expected");
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

CFG_Node::CFG_Node(CFG_NodeType type, int node_id, string stmt, string callee_proc) {
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

CFG_Node::CFG_Node(CFG_NodeType type, int node_id, string op, CFG_Opd *lopd, CFG_Opd *ropd1,
                   CFG_Opd *ropd2) {
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
    this->stmt = lopd->str() + " = " + ropd1->str() + " " + op + " " + ropd2->str();
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

void CFG_Node::set_node_id(int node_id) { this->node_id = node_id; }

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

std::set<std::string> CFG_Node::get_globals() {
  std::set<std::string> res;
  if (lopd && lopd->get_type() == CFG_OpdType::CFG_VarOpd) {
    res.insert(lopd->get_opd_var());
  }
  if (ropd1 && ropd1->get_type() == CFG_OpdType::CFG_VarOpd) {
    res.insert(ropd1->get_opd_var());
  }
  if (ropd2 && ropd2->get_type() == CFG_OpdType::CFG_VarOpd) {
    res.insert(ropd2->get_opd_var());
  }
  return res;
}

std::set<int> CFG_Node::get_predecessors() {
  std::set<int> res;
  for (auto pair : *in_edges) {
    res.insert(pair.first);
  }
  return res;
}

std::set<int> CFG_Node::get_successors() {
  std::set<int> res;
  for (auto pair : *out_edges) {
    res.insert(pair.first);
  }
  return res;
}

const std::string &CFG_Node::get_op() { return op; }

std::string USEVAR = "$USEVAR$";
const std::string &CFG_Node::get_def() {
  CHECK_INVARIANT(type == CFG_NodeType::CFG_AssignNode,
                  "Can not get def of non assignment statement");
  return lopd->get_type() == CFG_OpdType::CFG_UsevarOpd ? USEVAR : lopd->get_opd_var();
}

std::set<std::string> CFG_Node::get_uses() {
  CHECK_INVARIANT(type == CFG_NodeType::CFG_AssignNode,
                  "Can not get uses of non assignment statement");
  std::set<std::string> res;
  if (ropd1 && ropd1->get_type() == CFG_OpdType::CFG_VarOpd) {
    res.insert(ropd1->get_opd_var());
  }
  if (ropd2 && ropd2->get_type() == CFG_OpdType::CFG_VarOpd) {
    res.insert(ropd2->get_opd_var());
  }
  return res;
}

CFG_Opd *CFG_Node::get_lopd() {
  CHECK_INVARIANT(type == CFG_NodeType::CFG_AssignNode,
                  "Can not get def of non assignment statement");
  return lopd;
}

std::pair<CFG_Opd *, CFG_Opd *> CFG_Node::get_ropds() {
  CHECK_INVARIANT(type == CFG_NodeType::CFG_AssignNode,
                  "Can not get uses of non assignment statement");
  return std::make_pair(ropd1, ropd2);
}

std::vector<CFG_Opd *> CFG_Node::get_rhs_operands() {
  CHECK_INVARIANT(type == CFG_NodeType::CFG_AssignNode,
                  "Can not get uses of non assignment statement");
  std::vector<CFG_Opd *> res;
  if (ropd1) {
    res.push_back(ropd1);
  }
  if (ropd2) {
    res.push_back(ropd2);
  }
  return res;
}

const std::string &CFG_Node::get_callee() {
  CHECK_INVARIANT(type == CFG_NodeType::CFG_CallNode, "Can not get callee of non call statement");
  return callee_proc;
}

void CFG_Node::dump() { *dot_fd << stmt; }

void CFG_Node::visualize() {
  *dot_fd << "\t\tnode_" << this->node_id << " [shape=box, xlabel=\"" << this->node_id
          << "\", label=\"" << this->stmt << "\"];\n";
}
