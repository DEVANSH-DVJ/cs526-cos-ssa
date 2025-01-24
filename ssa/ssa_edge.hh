#ifndef _SSA_EDGE_HH_
#define _SSA_EDGE_HH_

#include <utility>

class SSA_Node;

class SSA_Edge;

class SSA_Edge {
  /* Always initialized: by constructor */
  std::pair<int, int> edge_id;
  SSA_Node *src;
  SSA_Node *dst;

public:
  /* Constructors and Destructor */
  SSA_Edge(int src_id, int dst_id);
  ~SSA_Edge();

  /* Get functions */
  // Get the edge id
  std::pair<int, int> &get_edge_id();
  // Get the source node
  SSA_Node *get_src();
  // Get the destination node
  SSA_Node *get_dst();

  /* Helper functions */
  // Visualize the edge
  void visualize();
};

#endif
