#include "headers.hh"

bool Def::operator==(const Def &other) const {
  return var_name == other.var_name && node == other.node;
}

bool Def::operator<(const Def &other) const {
  return node == other.node ? var_name < other.var_name
                            : node < other.node; // at most one def per node
}

bool QDef::operator==(const QDef &other) const {
  return def == other.def && context == other.context;
}

bool QDef::operator<(const QDef &other) const {
  return def == other.def ? context < other.context : def < other.def;
}

bool QNode::operator==(const QNode &other) const {
  return node == other.node && context == other.context;
}

bool QNode::operator<(const QNode &other) const {
  return node == other.node ? context < other.context : node < other.node;
}
