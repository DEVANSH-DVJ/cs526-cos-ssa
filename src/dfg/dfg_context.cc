#include "headers.hh"

#include <queue>
#include <string>

extern Program *program;

Context::Context(const std::string &proc) : proc{proc} {}

bool Context::operator==(const Context &other) const {
  return proc == other.proc && context == other.context;
}

bool Context::operator<(const Context &other) const {
  return proc == other.proc ? context < other.context : proc < other.proc;
}

std::string Context::to_string() const {
  std::string res = "(" + proc + ", {";

  bool first_outer = true;
  for (auto &[def, uses] : context) {
    if (first_outer) {
      first_outer = false;
    } else {
      res += ", ";
    }
    res += '{' + def.var_name + '_' + std::to_string(def.node) + " -> {";
    bool first_inner = true;
    for (Def use : uses) {
      if (first_inner) {
        first_inner = false;
      } else {
        res += ", ";
      }
      res += use.var_name + '_' + std::to_string(use.node);
    }
    res += "}}";
  }

  return res + "})";
}

Context *ContextTable::get_context(int context) {
  auto it = context_map.find(context);
  CHECK_INVARIANT(it != context_map.end(), "Context represented by integer does not exist");
  return &it->second;
}

int ContextTable::insert_context(const Context &context) {
  auto it = context_to_repr.find(context);
  if (it == context_to_repr.end()) {
    // Entirely new context
    context_map[next_context] = context;
    context_to_repr[context] = next_context;
    return next_context++;
  }
  // Return existing representation
  return it->second;
}

std::string ContextTable::to_string() const {
  std::string res;
  for (auto &[repr, context] : context_map) {
    res += "Context " + std::to_string(repr) + " = " + context.to_string() + "\n";
  }
  return res;
}

// Returns the set of transitively reaching qdefs given a set of live qdefs X
std::set<QDef> reaching_q_defs(const std::set<QDef> &X) {
  std::set<QDef> res;
  std::set<QDef> seen = X;
  std::queue<QDef> worklist;
  for (QDef qdef : X) {
    worklist.push(qdef);
  }

  // Loop over all reachible qdefs
  while (!worklist.empty()) {
    std::set<QDef> incoming = program->get_dfg_incoming(worklist.front());
    worklist.pop();
    for (QDef node : incoming) {
      if (seen.find(node) == seen.end()) {
        // Add reachible qdefs
        seen.insert(node);
        worklist.push(node);
        res.insert(node);
      }
    }
  }

  return res;
}

// Returns the qdefs in X that are defs of x_n
std::set<QDef> equivalent_nodes(Def x_n, const std::set<QDef> &X) {
  std::set<QDef> res;
  for (QDef y : X) {
    if (y.def == x_n) {
      res.insert(y);
    }
  }
  return res;
}

// Returns the set of defs removed by striping the context from qdefs in X
std::set<Def> remove_context(const std::set<QDef> &X) {
  std::set<Def> res;
  for (QDef x : X) {
    res.insert(x.def);
  }
  return res;
}

Context Context::gen_context(const std::string &proc, const std::set<QDef> &X) {
  Context context{proc};
  for (Def x_n : remove_context(X)) {
    context.context.insert({x_n, remove_context(reaching_q_defs(equivalent_nodes(x_n, X)))});
  }
  return context;
}
