#ifndef _PROGRAM_HH_
#define _PROGRAM_HH_

#include "ddg/ddg_context.hh"
#include "cfg/cfg_opd.hh"
#include "ddg/ddg_types.hh"

#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/IR/InstrTypes.h>

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
  bool single_partition;
  bool no_opt;

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
    std::map<int, std::map<int, int>> context_transitions; // From node, from context, to context
    std::map<int, std::set<QNode>> reverse_context_transitions;
  };
  /* A vector of DDGs for each global partition */
  std::vector<DDG> ddgs;

  /* The global partitions */
  std::vector<std::set<std::string>> partitions;
  size_t cur_partition;

  /* SSA Graph */
  std::map<int, SSA_Node *> *ssa_nodes;
  std::map<std::pair<int, int>, SSA_Edge *> *ssa_edges;

  /* LLVM */
  llvm::SMDiagnostic err;
  llvm::LLVMContext context;
  std::unique_ptr<llvm::Module> llvm_module;
  std::map<int, llvm::Value*> llvm_nodes;
  std::map<std::pair<int, int>, llvm::CallInst*> llvm_call_operands_for_node;

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

  // Partition globals from the CFG
  // Partitions are defined as connected components
  // of variables that interact with each other
  // If single_partition = true, all globals will be placed into
  // the same partition instead
  void partition_globals();
public:
  /* Constructors and Destructor */
  Program(std::string tool, std::string input_file, bool single_partition, bool no_opt);
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
  llvm::CallInst* get_llvm_call_operand_at_node(int node_id, int operand_num);

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

  // Maps a node to a particular LLVM value
  void map_node_to_llvm(int node, llvm::Value* value);
  // Maps one of the operands of a node to a particular CallInst
  void add_llvm_call_operand_at_node(int node, int operand_num, llvm::CallInst* call);

  // Get all global variables
  std::set<std::string> get_globals();

  /* DDG function: these all operate on the DDG corresponding to the current partition */
  // DDG getters
  std::set<QDef> get_ddg_nodes();
  std::set<QDef> get_ddg_incoming(QDef node);
  std::set<QDef> get_ddg_outgoing(QDef node);
  // Creates a context transition in the DDG
  // Returns true if a new transition was created
  // or if an existing transition changed (the context was updated)
  bool create_ddg_transition(QNode from_qnode, const Context& to_context);
  // Get a from context, to context transition pair at a particular node
  // This may be an end iterator
  std::map<int, int>::iterator get_ddg_transition(QNode qnode);
  // Gets a from context, to context transition map at a particular node
  std::map<int, int>& get_ddg_transitions(int node);
  // Gets the end iterator for context transitions from a particular node
  std::map<int, int>::iterator ddg_transitions_end(int node);
  // DDG reverse transition getters
  std::map<int, std::set<QNode>>::iterator get_ddg_reverse_transitions(int context);
  std::map<int, std::set<QNode>>::iterator ddg_reverse_transitions_end();
  // Returns whether qdef has a known value
  // If true, value is updated to be that value
  bool get_ddg_propagated_value(QDef qdef, int* value);
  // Returns whether a qdef is dead and can be removed
  bool ddg_is_dead(QDef qdef);
  // Registers a context and returns its integer representation
  int insert_ddg_context(Context context);
  // Add / remove functions for the DDG
  void add_ddg_node(QDef node);
  void remove_ddg_node(QDef node);
  void add_ddg_edge(QDef src, QDef dest);
  void remove_ddg_edge(QDef src, QDef dest);

  // Functions for working with the different global partitions
  void set_cur_partition(int partition);
  int get_num_partitions();
  // Returns true if opd represents a variable in the current partition
  bool is_in_cur_partition(CFG_Opd* opd);
  // Returns true if this node should be processed by a different partition
  bool is_part_of_other_partition(int node);

  // Reads the LLVM file into a Module
  void llvm_init_module();

  // Run
  void run();
};

#endif
