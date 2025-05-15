#ifndef _DFG_TYPES_HH_
#define _DFG_TYPES_HH_

#include <string>

// Def represents a variable definition at a particular node
typedef struct Def {
  std::string var_name;
  int node;

  bool operator==(const Def& other) const;
  bool operator<(const Def& other) const;
} Def;

// QDef represents a Def in a particular context
typedef struct QDef {
  Def def;
  int context;

  bool operator==(const QDef& other) const;
  bool operator<(const QDef& other) const;
} QDef;

// QNode represents a node in a known context
typedef struct QNode {
  int node;
  int context;

  bool operator==(const QNode& other) const;
  bool operator<(const QNode& other) const;
} QNode;

#endif
