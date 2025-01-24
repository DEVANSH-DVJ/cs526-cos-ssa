#ifndef _PROCEDURE_HH_
#define _PROCEDURE_HH_

#include <map>
#include <utility>

#include <string>

class CFG_Edge;
class CFG_Node;
class SSA_Edge;
class SSA_Node;

class Procedure;

class Procedure {
  /* Always initialized: by constructor */
  std::string name;

  /* CFG Graph */
  std::map<int, CFG_Node *> *cfg_nodes;
  std::map<std::pair<int, int>, CFG_Edge *> *cfg_edges;

  /* SSA Graph */
  std::map<int, SSA_Node *> *ssa_nodes;
  std::map<std::pair<int, int>, SSA_Edge *> *ssa_edges;

public:
  /* Constructors and Destructor */
  Procedure(std::string name);
  ~Procedure();

  /* Get functions */
  // Get procedure name
  std::string get_name();

  /* Update functions */
  // Add a CFG node
  void add_cfg_node(CFG_Node *node);
  // Add a CFG edge
  void add_cfg_edge(CFG_Edge *edge);
  // Add a SSA node
  void add_ssa_node(SSA_Node *node);
  // Add a SSA edge
  void add_ssa_edge(SSA_Edge *edge);

  /* Helper functions */
  // Visualize CFG graph
  void visualize_cfg();
  // Visualize SSA graph
  void visualize_ssa();
};

#endif
