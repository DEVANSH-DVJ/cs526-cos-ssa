#ifndef _PROGRAM_HH_
#define _PROGRAM_HH_

#include "ddg/ddg_context.hh"
#include "cfg/cfg_opd.hh"
#include "ddg/ddg_types.hh"

#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/SourceMgr.h>

#include <list>
#include <map>
#include <set>
#include <string>
#include <utility>

class Procedure;
class CFG_Edge;
class CFG_Node;
typedef struct QDef QDef;
class ContextTable;
class SSA_Edge;
class SSA_Node;

class Program {
  /* State */
  std::string tool;
  std::string input_name;

  /* Procedures */
  std::list<Procedure *> *procs;
  std::map<std::string, Procedure *> *procedures;

  /* CFG Graph */
  std::map<int, CFG_Node *> *cfg_nodes;
  std::map<std::pair<int, int>, CFG_Edge *> *cfg_edges;

  /* DDG */
  struct DDG {
    std::set<QDef> nodes;
    ContextTable context_table;
    std::map<QDef, std::set<QDef>> edges;
    std::map<QDef, std::set<QDef>> reverse_edges;
    std::map<QDef, int> propagated_values;
    std::set<QDef> dead_qdefs;
    std::map<int, std::map<int, int>> context_transitions;
    std::map<int, std::set<QNode>> reverse_context_transitions;
  };
  std::vector<DDG> ddgs;

  std::vector<std::set<std::string>> partitions;
  size_t cur_partition;

  /* SSA Graph */
  std::map<int, SSA_Node *> *ssa_nodes;
  std::map<std::pair<int, int>, SSA_Edge *> *ssa_edges;

  /* LLVM */
  std::unique_ptr<llvm::Module> llvm_module;
  llvm::LLVMContext context;
  llvm::SMDiagnostic err;
  std::map<int, llvm::Value*> llvm_nodes;

  /* Helper functions */
  // Parse CFG graph from a file
  void parse_cfg();
  // Parse LLVM IR into a CFG
  void parse_cfg_from_llvm();
  // Parse SSA graph
  void parse_ssa();
  // Construct DDG from the CFG graph
  void construct_ddg();
  // Do constant propagation on the DDG
  void propagate_ddg_constants();
  // Reduce reduncancy in the DDG
  void reduce_ddg();
  // Do dead code elimination on the DDG
  void detect_dead_ddg_qdefs();
  // Initialized the SSA graph
  void init_ssa();
  // Construct the SSA graph from the current DDG
  void construct_ssa_partition();
  // Finalizes the SSA graph by converting empty assignments to empty nodes
  void finalize_ssa();
  // Deconstruct the SSA graph into LLVM IR
  void deconstruct_ssa();
  // Dump the CFG to a file
  void dump_cfg();
  // Visualize CFG graph
  void visualize_cfg();
  // Visualize the DDG
  void visualize_ddg();
  // Dump the SSA graph to a file
  void dump_ssa();
  // Visualize SSA graph
  void visualize_ssa();
  // Dump the LLVM IR to a file
  void dump_llvm();

  void partition_globals(bool single_partition = false);
public:
  /* Constructors and Destructor */
  Program(std::string tool, std::string input_file);
  ~Program();

  /* Get functions */
  // Get procedures
  std::map<std::string, Procedure *> *get_procs();
  // Get procedure by name
  Procedure *get_proc(std::string name);
  // Get CFG node by id
  CFG_Node *get_cfg_node(int node_id, bool abort_if_not_found);
  // Get SSA node by id
  SSA_Node *get_ssa_node(int node_id, bool abort_if_not_found);
  // Get LLVM node by id
  llvm::Value* get_llvm_node(int node_id, bool abort_if_not_found);

  /* Update functions */
  // Add a procedure
  void add_proc(Procedure *proc);
  // Push a procedure in the list
  void push_proc(Procedure *proc);
  // Add a CFG node
  void add_cfg_node(CFG_Node *node);
  // Add a CFG edge
  void add_cfg_edge(CFG_Edge *edge);
  // Add a SSA node
  void add_ssa_node(SSA_Node *node);
  // Add a SSA edge
  void add_ssa_edge(SSA_Edge *edge);

  std::set<std::string> get_globals();
  std::set<QDef> get_ddg_nodes();
  std::set<QDef> get_ddg_incoming(QDef node);
  std::set<QDef> get_ddg_outgoing(QDef node);
  bool create_ddg_transition(QNode from_qnode, const Context& to_context);
  std::map<int, int>::iterator get_ddg_transition(QNode qnode);
  std::map<int, int>& get_ddg_transitions(int node);
  std::map<int, int>::iterator ddg_transitions_end(int node);
  std::map<int, std::set<QNode>>::iterator get_ddg_reverse_transitions(int context);
  std::map<int, std::set<QNode>>::iterator ddg_reverse_transitions_end();
  bool get_ddg_propagated_value(QDef qdef, int* value);
  bool ddg_is_dead(QDef qdef);
  int insert_ddg_context(Context context);
  void add_ddg_node(QDef node);
  void remove_ddg_node(QDef node);
  void add_ddg_edge(QDef src, QDef dest);
  void remove_ddg_edge(QDef src, QDef dest);

  void set_cur_partition(int partition);
  int get_num_partitions();
  bool is_in_cur_partition(CFG_Opd* opd);
  bool is_part_of_other_partition(int node);

  void llvm_init_module();

  // Cleanup
  void cleanup();

  // Run
  void run();
};

#endif
