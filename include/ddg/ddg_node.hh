#ifndef _DDG_NODE_HH_
#define _DDG_NODE_HH_

#include <string>

typedef enum {
  DDG_Var,
  DDG_Num,
  DDG_Input,
  DDG_Usevar,
} DDG_NodeType;

class DDG_Node {
public:
  DDG_NodeType type;
  int node_id;
  int meta_id;

  std::string var_name;
  int num_value;

  // DDG_Node(DDG_NodeType type, int node_id, int meta_id);
  // ~DDG_Node();
};

#endif
