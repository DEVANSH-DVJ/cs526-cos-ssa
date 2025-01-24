#ifndef _SSA_META_HH_
#define _SSA_META_HH_

#include <list>
#include <utility>

class SSA_Stmt;

class SSA_Meta;

class SSA_Meta {
  /* Always initialized: by constructor */
  std::pair<int, int> meta_num;

  /* Always initialized: post parsing*/
  std::list<SSA_Stmt *> *stmts;

public:
  /* Constructors and Destructor */
  SSA_Meta(std::pair<int, int> meta_num, std::list<SSA_Stmt *> *stmts);
  ~SSA_Meta();

  /* Get functions */
  // Get the meta number
  std::pair<int, int> get_meta_num();
  // Get the list of statements
  std::list<SSA_Stmt *> *get_stmts();

  /* Helper functions */
  // Visualize the meta
  void visualize();
};

#endif
