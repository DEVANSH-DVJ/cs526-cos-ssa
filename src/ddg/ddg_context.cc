#include "headers.hh"

#include <queue>
#include <string>

extern Program* program;

Context::Context(const std::string& proc) : proc {proc} {}

bool Context::operator==(const Context& other) const {
  return proc == other.proc && context == other.context;
}

bool Context::operator<(const Context& other) const {
  return proc == other.proc ? context < other.context : proc < other.proc;
}

std::string Context::to_string() {
  std::string res = "(" + proc + ", {";

  bool first_outer = true;
  for (auto pair : context) {
    if (first_outer) {
      first_outer = false;
    } else {
      res += ", ";
    }
    res += '{' + pair.first.var_name + '_' + std::to_string(pair.first.node) + " -> {";
    bool first_inner = true;
    for (Def def : pair.second) {
      if (first_inner) {
        first_inner = false;
      } else {
        res += ", ";
      }
      res += def.var_name + '_' + std::to_string(def.node);
    }
    res += "}}";
  }

  return res + "})";
}

Context* ContextTable::get_context(int context) {
    auto it = context_map.find(context);
    CHECK_INVARIANT(it != context_map.end(), "Context represented by integer does not exist");
    return &it->second;
}

int ContextTable::insert_context(const Context& context) {
  auto it = context_to_repr.find(context);
  if (it == context_to_repr.end()) {
    context_map[next_context] = context;
    context_to_repr[context][next_context] = 1;
    return next_context++;
  }
  auto repr = it->second.begin();
  ++repr->second;
  return repr->first;
}

bool ContextTable::update_context(int* repr, const Context& context) {
  auto it = context_map.find(*repr);
  CHECK_INVARIANT(it != context_map.end(), "Context represented by integer does not exist");

  if (it->second == context) {
    return false;
  }

  auto context_repr = context_to_repr[it->second].find(*repr);
  if (--context_repr->second == 0) {
    context_to_repr[it->second].erase(context_repr);
    if (context_to_repr[it->second].empty()) {
      context_to_repr.erase(context_to_repr.find(it->second));
    }

    context_map[*repr] = context;
    context_to_repr[context][*repr] = 1;
  } else {
    *repr = insert_context(context);
  }

  return true;
}

std::string ContextTable::to_string() {
  std::string res;
  for (auto pair : context_map) {
    res += "Context " + std::to_string(pair.first) + " = " + pair.second.to_string() + "\n";
  }
  return res;
}

std::set<QDef> reaching_q_defs(const std::set<QDef>& X) {
  std::set<QDef> res;
  std::set<QDef> seen = X;
  std::queue<QDef> worklist;
  for (QDef qdef : X) {
    worklist.push(qdef);
  }

  while (!worklist.empty()) {
    std::set<QDef> incoming = program->get_ddg_incoming(worklist.front());
    worklist.pop();
    for (QDef node : incoming) {
      if (seen.find(node) == seen.end()) {
        seen.insert(node);
        worklist.push(node);
        res.insert(node);
      }
    }
  }

  return res;
}

std::set<QDef> equivalent_nodes(Def x_n, const std::set<QDef>& X) {
  std::set<QDef> res;
  for (QDef y : X) {
    if (y.def == x_n) {
      res.insert(y);
    }
  }
  return res;
}

std::set<Def> remove_context(const std::set<QDef>& X) {
  std::set<Def> res;
  for (QDef x : X) {
    res.insert(x.def);
  }
  return res;
}

Context Context::gen_context(const std::string& proc, const std::set<QDef>& X) {
  Context context {proc};
  for (Def x_n : remove_context(X)) {
    context.context.insert({x_n, remove_context(reaching_q_defs(equivalent_nodes(x_n, X)))});
  }
  return context;
}
