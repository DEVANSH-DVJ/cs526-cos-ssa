#ifndef _DDG_CONTEXT_HH_
#define _DDG_CONTEXT_HH_

#include "ddg_types.hh"

#include <map>
#include <set>
#include <string>

class Context {
public:
  Context() = default;
  Context(const std::string& proc);
  static Context gen_context(const std::string& proc, const std::set<QDef>& X);

  bool operator==(const Context& other) const;
  bool operator<(const Context& other) const;

  std::string to_string();
private:
  std::string proc;
  std::set<std::pair<Def, std::set<Def>>> context;
};

class ContextTable {
public:
  Context* get_context(int context);

  int insert_context(const Context& context);

  bool update_context(int* repr, const Context& context);

  std::string to_string();
private:
  std::map<int, Context> context_map;
  std::map<Context, std::map<int, int>> context_to_repr;
  int next_context = 1;
};

#endif
