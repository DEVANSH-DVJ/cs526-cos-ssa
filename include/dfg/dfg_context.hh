#ifndef _DFG_CONTEXT_HH_
#define _DFG_CONTEXT_HH_

#include "dfg_types.hh"

#include <map>
#include <set>
#include <string>

// Context represents a value based context in a particular procedure
// Context is only useful to compare if it is equal to other contexts
// and to get a string representation
class Context {
public:
  Context() = default;
  Context(const std::string& proc);
  // Creates a context for a procedure using a set of reaching QDefs
  static Context gen_context(const std::string& proc, const std::set<QDef>& X);

  bool operator==(const Context& other) const;
  bool operator<(const Context& other) const;

  std::string to_string() const;
private:
  std::string proc;
  std::set<std::pair<Def, std::set<Def>>> context;
};

// Stores Contexts and associates them with integer representations
class ContextTable {
public:
  // Get a context from its representation
  Context* get_context(int context);

  // Registers the context and returns an integer representation for the Context
  int insert_context(const Context& context);

  std::string to_string();
private:
  std::map<int, Context> context_map;
  // Maps contexts to their representation
  std::map<Context, int> context_to_repr;
  int next_context = 1;
};

#endif
