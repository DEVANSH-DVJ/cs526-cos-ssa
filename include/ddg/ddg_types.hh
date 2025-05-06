#ifndef _DDG_TYPES_HH_
#define _DDG_TYPES_HH_

#include <optional>
#include <string>

typedef struct Def {
  std::string var_name;
  int node;

  bool operator==(const Def& other) const;
  bool operator<(const Def& other) const;
} Def;

typedef struct QDef {
  Def def;
  int context;

  bool operator==(const QDef& other) const;
  bool operator<(const QDef& other) const;
} QDef;

typedef struct QNode {
  int node;
  int context;

  bool operator==(const QNode& other) const;
  bool operator<(const QNode& other) const;
} QNode;

#endif
