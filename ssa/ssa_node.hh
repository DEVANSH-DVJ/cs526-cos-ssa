#ifndef _SSA_NODE_HH_
#define _SSA_NODE_HH_

#include <string>

#include <map>

class SSA_Edge;
class SSA_Meta;

class SSA_Node;

typedef enum {
  SSA_StartNode,
  SSA_EndNode,
  SSA_CallNode,
  SSA_AssignNode,
} SSA_NodeType;

class SSA_Node {
  /* Always initialized: by constructor */
  SSA_NodeType type;
  int node_id;

  /* Always initialized: post parsing*/
  std::string parent_proc;
  std::map<int, SSA_Edge *> *in_edges;
  std::map<int, SSA_Edge *> *out_edges;

  /* Conditionally initialized: post parsing */
  std::string stmt;
  std::string callee_proc;
  std::map<int, SSA_Meta *> *metas;

public:
  /* Constructors and Destructor */
  SSA_Node(SSA_NodeType type, int node_id, std::string stmt);
  SSA_Node(SSA_NodeType type, int node_id, std::string stmt,
           std::string callee_proc);
  SSA_Node(SSA_NodeType type, int node_id);
  ~SSA_Node();

  /* Get functions */
  // Get the node type
  SSA_NodeType get_type();
  // Get the node id
  int get_node_id();
  // Get the parent procedure
  std::string &get_parent_proc();

  /* Update functions */
  // Set the parent procedure
  void set_parent_proc(std::string parent_proc);
  // Add an in edge
  void add_in_edge(SSA_Edge *edge);
  // Add an out edge
  void add_out_edge(SSA_Edge *edge);
  // Add a meta
  void add_meta(SSA_Meta *meta);

  /* Helper functions */
  // Visualize the node
  void visualize();
};

#endif
