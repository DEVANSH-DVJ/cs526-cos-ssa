#ifndef _CFG_NODE_HH_
#define _CFG_NODE_HH_

#include <string>
#include <vector>

#include <map>
#include <set>

class CFG_Edge;
class CFG_Opd;

class CFG_Node;

typedef enum {
  CFG_StartNode,
  CFG_EndNode,
  CFG_CallNode,
  CFG_AssignNode,
  CFG_EmptyNode
} CFG_NodeType;

class CFG_Node {
  /* Always initialized: by constructor */
  CFG_NodeType type;
  int node_id;

  /* Always initialized: post parsing*/
  std::string parent_proc;
  std::string stmt;
  std::map<int, CFG_Edge *> *in_edges;
  std::map<int, CFG_Edge *> *out_edges;

  /* Conditionally initialized: post parsing */
  std::string callee_proc;
  std::string op;
  CFG_Opd *lopd;
  CFG_Opd *ropd1;
  CFG_Opd *ropd2;

public:
  /* Constructors and Destructor */
  CFG_Node(CFG_NodeType type, int node_id, std::string stmt);
  CFG_Node(CFG_NodeType type, int node_id, std::string stmt,
           std::string callee_proc);
  CFG_Node(CFG_NodeType type, int node_id, std::string op, CFG_Opd *lopd,
           CFG_Opd *ropd1, CFG_Opd *ropd2);
  ~CFG_Node();

  /* Get functions */
  // Get the node type
  CFG_NodeType get_type();
  // Get the node id
  int get_node_id();
  // Get the parent procedure
  std::string &get_parent_proc();

  /* Update functions */
  // Set the node id
  void set_node_id(int node_id);
  // Set the parent procedure
  void set_parent_proc(std::string parent_proc);
  // Add an in edge
  void add_in_edge(CFG_Edge *edge);
  // Add an out edge
  void add_out_edge(CFG_Edge *edge);

  // Returns the global variables involved at this node
  std::set<std::string> get_globals();
  // Get predecessor and successor node ids
  std::set<int> get_predecessors();
  std::set<int> get_successors();

  const std::string& get_op();
  // Gets the LHS in a string representation
  const std::string& get_def();
  // Returns a set of strings representing variables on the RHS
  std::set<std::string> get_uses();
  CFG_Opd* get_lopd();
  std::pair<CFG_Opd*, CFG_Opd*> get_ropds();
  // Returns all operands on the right hand side (at most 2)
  std::vector<CFG_Opd*> get_rhs_operands();
  // Get the called function if this is a call node
  const std::string& get_callee();

  /* Helper functions */
  // Dump the node
  void dump();
  // Visualize the node
  void visualize();
};

#endif
