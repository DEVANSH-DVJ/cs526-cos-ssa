#ifndef _DDG_CONTEXT_HH_
#define _DDG_CONTEXT_HH_

#include "ddg_types.hh"

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

  std::string to_string();
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

  // Given an integer representation and a possibly new version of the context, updates the context
  // Returns true if the context changed, in which case repr will be set to the new representation
  // (repr may remain unchanged)
  bool update_context(int* repr, const Context& context);

  std::string to_string();
private:
  std::map<int, Context> context_map;
  // Maps contexts to their representation; representations are mapped to their reference counts
  std::map<Context, std::map<int, int>> context_to_repr;
  int next_context = 1;
};

#endif
