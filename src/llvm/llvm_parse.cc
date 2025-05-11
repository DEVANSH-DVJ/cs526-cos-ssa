#include "headers.hh"

#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>

#include <memory>
#include <map>
#include <set>
#include <queue>

extern Program* program;
std::string ll_file;

llvm::Module* module = nullptr;

void llvm_set_in(llvm::Module* llvm_module) {
  module = llvm_module;
}

int node_num = 1;

// Return a string representation of a return value promoted to a global
std::string func_ret_to_global(llvm::Function* func) {
  return "$" + func->getName().str() + "_ret";
}

// Returns whether this function should be processed
bool is_usable_func(llvm::Function* func) {
  return !func->isDeclaration();
}

// Constructs the CFG for a given funtion
// Returns a map of basic blocks to sets of their successors
std::map<llvm::BasicBlock*, std::set<llvm::BasicBlock*>> construct_llvm_cfg(llvm::Function* func) {
  std::map<llvm::BasicBlock*, std::set<llvm::BasicBlock*>> cfg_transitions;
  std::queue<llvm::BasicBlock*> worklist;
  worklist.push(&func->getEntryBlock());
  cfg_transitions[&func->getEntryBlock()] = std::set<llvm::BasicBlock*>();

  // Traverse all basic blocks
  while (!worklist.empty()) {
    llvm::BasicBlock* bb = worklist.front();
    worklist.pop();

    // Add edges to successors, add successors to the worklist
    for (llvm::BasicBlock* successor : llvm::successors(bb)) {
      cfg_transitions[bb].insert(successor);

      if (cfg_transitions.find(successor) == cfg_transitions.end()) {
        worklist.push(successor);
        cfg_transitions[successor] = std::set<llvm::BasicBlock*>();
      }
    }
  }

  return cfg_transitions;
}

// A struct to track information about global variables
typedef struct GlobalInfo {
  std::set<llvm::GlobalVariable*> globals; // Globals that are suitable to process
  std::map<llvm::Instruction*, std::string> uses; // Instructions and the global they use
  std::map<llvm::Instruction*, std::string> defs; // Instructions and the global they def
  std::set<std::string> functions; // Functions that are suitable to process
} GlobalInfo;

// Initializes a GlobalInfo for the module
GlobalInfo get_globals(llvm::Module* module) {
  // Track which variables have their address taken; these are not suitable to process
  std::set<std::string> addr_taken;
  for (llvm::GlobalVariable& var : module->globals()) {
    if (var.getNumOperands() != 1) {
      // Only support scalars
      addr_taken.insert(var.getName().str());
      break;
    }
    bool found = false;
    // Check all uses for this variable
    for (llvm::Use& use : var.uses()) {
      llvm::User* user = use.getUser();
      if (llvm::LoadInst* load = llvm::dyn_cast<llvm::LoadInst>(user)) {
        if (&var != load->getPointerOperand()) {
          // If the address of this variable is not being loaded from, it is not suitable
          found = true;
          break;
        }
      } else if (llvm::StoreInst* load = llvm::dyn_cast<llvm::StoreInst>(user)) {
        if (&var != load->getPointerOperand()) {
          // If the address of this variable is not being stored to, it is not suitable
          found = true;
          break;
        }
      } else {
        // The address of this variable is involved in something that is not a load/store,
        // it is not suitable
        found = true;
        break;
      }
    }
    if (found) {
      addr_taken.insert(var.getName().str());
      break;
    }
  }

  // Process all functions in the module
  GlobalInfo globals;
  for (llvm::Function& func : *module) {
    if (!is_usable_func(&func)) {
      continue;
    }

    globals.functions.insert(func.getName().str());
    // Process all instructions in this basic block
    for (llvm::BasicBlock& bb : func) {
      for (llvm::Instruction& inst : bb) {
        if (inst.isVolatile()) {
          continue;
        }
        if (llvm::LoadInst* load = llvm::dyn_cast<llvm::LoadInst>(&inst)) {
          if (llvm::GlobalVariable* var = llvm::dyn_cast<llvm::GlobalVariable>(load->getPointerOperand())) {
            if (addr_taken.find(var->getName().str()) == addr_taken.end() && var->getNumOperands() == 1) {
              // This load is a use
              globals.globals.insert(var);
              globals.uses[&inst] = var->getName();
            }
          }
        } else if (llvm::StoreInst* store = llvm::dyn_cast<llvm::StoreInst>(&inst)) {
          if (llvm::GlobalVariable* var = llvm::dyn_cast<llvm::GlobalVariable>(store->getPointerOperand())) {
            if (addr_taken.find(var->getName().str()) == addr_taken.end() && var->getNumOperands() == 1) {
              // This store is a def
              globals.globals.insert(var);
              globals.defs[&inst] = var->getName();
            }
          }
        } else if (llvm::CallInst* call = llvm::dyn_cast<llvm::CallInst>(&inst)) {
          llvm::Function* called_func = call->getCalledFunction();
          if (called_func != nullptr && is_usable_func(called_func)) {
            // "Promote" returns to globals
            globals.uses[&inst] = func_ret_to_global(call->getCalledFunction());
          }
        } else if (llvm::isa<llvm::ReturnInst>(&inst) && func.getName().str() != "main") {
          // "Promote" returns to globals
          globals.defs[&inst] = func_ret_to_global(&func);
        }
      }
    }
  }
  return globals;
}

// Takes an operand that belongs to something that will become a CFG node
// If this operand is used by nothing else, adds it to removable_uses
void mark_removable_operand(llvm::Value* value, std::set<llvm::Value*>& removable_uses) {
  llvm::LoadInst* load;
  if ((load = llvm::dyn_cast<llvm::LoadInst>(value)) && load->getNumUses() == 1) {
    removable_uses.insert(load);
  }
}

// Takes an instruction that belongs to something that will become a CFG node
// Calls mark_removable_operand on the instructions operands
void mark_removable_inst(llvm::Instruction* inst, std::set<llvm::Value*>& removable_uses) {
  CHECK_INVARIANT(inst->getNumOperands() == 2, "Expected instruction with two operands");
  
  mark_removable_operand(inst->getOperand(0), removable_uses);
  mark_removable_operand(inst->getOperand(1), removable_uses);
}

// Takes an operand that belongs to something that will become a CFG node
// If this operand is used by nothing else, it is erased unless if this is a CallInst,
// in which case it is tracked for future use
void erase_removable_operand(llvm::Value* value, int operand_num) {
  llvm::LoadInst* load;
  if ((load = llvm::dyn_cast<llvm::LoadInst>(value)) && load->getNumUses() == 1) {
    llvm::Type* int_type = llvm::IntegerType::get(module->getContext(), 32);
    load->replaceAllUsesWith(llvm::ConstantInt::get(int_type, 0));
    load->eraseFromParent();
  } else if (llvm::CallInst* call = llvm::dyn_cast<llvm::CallInst>(value)) {
    program->add_llvm_call_operand_at_node(node_num, operand_num, call);
  }
}

// Takes an instruction that belongs to something that will become a CFG node
// Calls erase_removable_operand on the instructions operands
// If this operand is used by nothing else, it is erased
void erase_removable_rhs(llvm::Instruction* inst) {
  erase_removable_operand(inst->getOperand(0), 0);
  erase_removable_operand(inst->getOperand(1), 1);
  if (inst->getNumUses() == 1) {
    llvm::Type* int_type = llvm::IntegerType::get(module->getContext(), 32);
    inst->replaceAllUsesWith(llvm::ConstantInt::get(int_type, 0));
    inst->eraseFromParent();
  }
}

// Returns whether this RHS operand can be represented as a CFG operand
// If true, repr is updated to be a CFG representation of operand
bool get_operand_repr(llvm::Value* operand, CFG_Opd** repr, const GlobalInfo& globals) {
  if (llvm::Instruction* inst = llvm::dyn_cast<llvm::Instruction>(operand)) {
    auto it = globals.uses.find(inst);
    if (it != globals.uses.end()) {
      *repr = new CFG_Opd(CFG_OpdType::CFG_VarOpd, it->second);
      return true;
    }
  } else if (llvm::ConstantInt* value = llvm::dyn_cast<llvm::ConstantInt>(operand)) {
    llvm::SmallVector<char> vec;
    value->getValue().toStringSigned(vec);
    *repr = new CFG_Opd(CFG_OpdType::CFG_NumOpd, std::atoi(std::string(vec.begin(), vec.end()).c_str()));
    return true;
  }

  return false;
}

// Stores the operation and RHS operands of a CFG node
struct RHS {
  std::string op;
  CFG_Opd* ropd1;
  CFG_Opd* ropd2; // May be nullptr
};

// Computes the RHS from a value that is stored to a global
// Marks operands that are removable
RHS get_assignment_value(llvm::Value* value, const GlobalInfo& globals, std::set<llvm::Value*>& removable_uses) {
  static const std::map<unsigned int, char> ops {
    {llvm::Instruction::Add, '+'},
    {llvm::Instruction::Sub, '-'},
    {llvm::Instruction::Mul, '*'},
    {llvm::Instruction::SDiv, '/'}
  };

  CFG_Opd* ropd1;
  if (get_operand_repr(value, &ropd1, globals)) {
    // Single operand case
    mark_removable_operand(value, removable_uses);
    return {"=", ropd1, nullptr};
  }
  if (llvm::Instruction* inst = llvm::dyn_cast<llvm::Instruction>(value)) {
    auto it = ops.find(inst->getOpcode());
    if (it != ops.end()) {
      if (inst->getNumOperands() == 2) {
        // Instruction that we can handle
        if (get_operand_repr(inst->getOperand(0), &ropd1, globals)) {
          CFG_Opd* ropd2;
          if (get_operand_repr(inst->getOperand(1), &ropd2, globals)) {
            // If we can recover both operands, return
            mark_removable_inst(inst, removable_uses);
            return {std::string(1, it->second), ropd1, ropd2};
          }
          delete ropd1;
        }
      }
    }
  }

  // Unable to recover, to the RHS is an InputOpd
  return {"=", new CFG_Opd(CFG_OpdType::CFG_InputOpd), nullptr};
}

// Returns a list of CFG nodes and their corresponding llvm values for a llvm basic block
std::vector<std::pair<CFG_Node*, llvm::Value*>> get_nodes_in_basic_block(const std::string& proc, llvm::BasicBlock* bb, const GlobalInfo& globals) {
  std::vector<std::pair<CFG_Node*, llvm::Value*>> res;
  std::set<llvm::Value*> removable_uses;

  if (bb->isEntryBlock()) {
    res.push_back(std::make_pair(new CFG_Node(CFG_NodeType::CFG_StartNode, 0, "START " + proc), nullptr));

    // Add in dummy assignments to indicate what the initial value of these variables is
    if (proc == "main") {
      for (llvm::GlobalVariable* var : globals.globals) {
        CFG_Opd* ropd1;
        if (get_operand_repr(var->getOperand(0), &ropd1, globals)) {
          res.push_back(std::make_pair(
            new CFG_Node(CFG_NodeType::CFG_AssignNode, 0, "=", new CFG_Opd(CFG_OpdType::CFG_VarOpd, var->getName().str()), ropd1, nullptr),
            nullptr));
        }
      }
    }
  }

  // Translate instructions into CFG nodes
  for (llvm::Instruction& inst : *bb) {
    auto it = globals.uses.find(&inst);
    if (it != globals.uses.end()) {
      if (llvm::CallInst* call = llvm::dyn_cast<llvm::CallInst>(&inst)) {
        auto it = globals.functions.find(call->getCalledFunction()->getName().str());
        if (it != globals.functions.end()) {
          res.push_back(std::make_pair(new CFG_Node(CFG_NodeType::CFG_CallNode, 0, "CALL " + *it, *it), call));
        }
      } else {
        // If this is actually not used by a USEVAR, this will be marked as erasible and removed later
        res.push_back(std::make_pair(
          new CFG_Node(CFG_NodeType::CFG_AssignNode, 0, "=", new CFG_Opd(CFG_OpdType::CFG_UsevarOpd), new CFG_Opd(CFG_OpdType::CFG_VarOpd, it->second), nullptr),
          &inst
        ));
      }
    } else {
      auto it = globals.defs.find(&inst);
      if (it != globals.defs.end()) {
        if (llvm::StoreInst* store = llvm::dyn_cast<llvm::StoreInst>(&inst)) {
          RHS rhs = get_assignment_value(store->getValueOperand(), globals, removable_uses);
          res.push_back(std::make_pair(
            new CFG_Node(CFG_NodeType::CFG_AssignNode, 0, rhs.op, new CFG_Opd(CFG_OpdType::CFG_VarOpd, it->second), rhs.ropd1, rhs.ropd2),
            store
          ));
        } else if (llvm::ReturnInst* ret = llvm::dyn_cast<llvm::ReturnInst>(&inst)) {
          if (ret->getNumOperands() == 1) {
            RHS rhs = get_assignment_value(ret->getOperand(0), globals, removable_uses);
            res.push_back(std::make_pair(
              new CFG_Node(CFG_NodeType::CFG_AssignNode, 0, rhs.op, new CFG_Opd(CFG_OpdType::CFG_VarOpd, it->second), rhs.ropd1, rhs.ropd2),
              ret
            ));
          }
        }
      } else if (llvm::CallInst* call = llvm::dyn_cast<llvm::CallInst>(&inst)) {
        llvm::Function* func = call->getCalledFunction();
        if (func != nullptr) {
          auto it = globals.functions.find(func->getName().str());
          if (it != globals.functions.end()) {
            res.push_back(std::make_pair(new CFG_Node(CFG_NodeType::CFG_CallNode, 0, "CALL " + *it, *it), call));
          }
        }
      }
    }
  }

  if (res.empty()) {
    res.push_back(std::make_pair(new CFG_Node(CFG_NodeType::CFG_EmptyNode, 0, ""), nullptr));
  }

  // Remove removable uses (of the form USEVAR = ...)
  std::vector<std::pair<CFG_Node*, llvm::Value*>> filtered_res;
  for (auto pair : res) {
    if (removable_uses.find(pair.second) == removable_uses.end()) {
      filtered_res.push_back(pair);
    } else {
      delete pair.first;
    }
  }

  return filtered_res;
}

// Represents a basic block and its corresponding CFG node - llvm value pairs
struct SimpleBasicBlock {
  llvm::BasicBlock* bb;
  std::vector<std::pair<CFG_Node*, llvm::Value*>> nodes;
};

// Represents a procedure, its CFG, and its basic blocks
struct SimpleProc {
  std::string proc_name;
  std::map<llvm::BasicBlock*, std::set<llvm::BasicBlock*>> cfg_transitions;
  std::vector<SimpleBasicBlock> basic_blocks;
};

// Converts an llvm function to a SimpleProc
SimpleProc convert_to_simple_proc(llvm::Function* func, const GlobalInfo& globals) {
  SimpleProc proc;
  proc.proc_name = func->getName().str();
  proc.cfg_transitions = construct_llvm_cfg(func);

  for (int i = 0; i < 2; ++i) {
    for (auto [bb, _] : proc.cfg_transitions) {
      if (bb->isEntryBlock() == (i == 0)) { // Ensures that the entry block gets added first
        proc.basic_blocks.push_back({bb, get_nodes_in_basic_block(proc.proc_name, bb, globals)});
      }
    }
  }

  return proc;
}

// A disjoint sets data structure for strings
class DisjointSets {
public:
  // Returns whether l and r are in the same set
  bool are_eq(const std::string& l, const std::string& r) const {
    return find_str(l) == find_str(r);
  }

  // Unions two sets
  void union_sets(const std::string& l, const std::string& r) {
    int a = find_str(l);
    int b = find_str(r);

    if (a == b) {
      return;
    }

    if (dset[b] < dset[a]) {
      std::swap(a, b);
    }
    dset[a] += dset[b];
    dset[b] = a;
  }

private:
  mutable std::map<std::string, int> globals; // Maps string to dset indices
  mutable std::vector<int> dset;

  // Converts a string to an index and calls find
  int find_str(const std::string& s) const {
    auto it = globals.find(s);
    if (it == globals.end()) {
      // New string, add a new set with one element
      globals[s] = dset.size();
      dset.push_back(-1);
      it = globals.find(s);
    }
    return find(it->second);
  }

  // Finds a representative for the set an index belongs to
  int find(int n) const {
    if (dset[n] < 0) {
      return n;
    }
    return dset[n] = find(dset[n]);
  }
};

// Given a map of globals and the number of interactions they are in, returns the global in the
// most interactions
std::string get_most_interactions(const std::map<std::string, int>& globals) {
  CHECK_INVARIANT(globals.size() > 0, "Cannot partition empty globals");
  std::string var;
  int max_interactions = -1;
  for (auto& [global, num_interactions] : globals) {
    if (num_interactions > max_interactions) {
      var = global;
      max_interactions = num_interactions;
    }
  }
  return var;
}

constexpr int MAX_PARTITION_SIZE = 10;
// Creates a partition from globals that have not yet been partitioned, and unions all of these
// globals into the same set in partitions
void create_partition(std::map<std::string, int>& globals,
                                       std::map<std::string, std::map<std::string, int>>& interactions,
                                       DisjointSets& partitions) {
  CHECK_INVARIANT(globals.size() > 0, "Cannot partition empty globals");

  auto cmp = [](const std::pair<std::string, int>& l, const std::pair<std::string, int>& r) {
    return l.second < r.second;
  };
  std::priority_queue<std::pair<std::string, int>, std::vector<std::pair<std::string, int>>, decltype(cmp)> queue(cmp);

  std::string first = get_most_interactions(globals);
  queue.push({first, 0});

  // Keep adding to the partition until the max partition size is reached or no more globals are reachible from first
  // Globals are added in the order of most interations with the rest of the partition
  std::map<std::string, int> seen_interactions;
  for (int i = 0; i < MAX_PARTITION_SIZE; ++i) {
    if (globals.empty()) {
      return;
    }

    std::string cur;
    bool found = false;
    // Find a global we have not seen yet
    while (!queue.empty()) {
      cur = queue.top().first;
      queue.pop();
      if (globals.find(cur) != globals.end()) {
        found = true;
        break;
      }
    }
    if (!found) {
      return;
    }

    partitions.union_sets(first, cur);
    globals.erase(globals.find(cur));

    // Update the remaining interactions and add variables to the queue
    for (auto [neighbor, num_interactions] : interactions[cur]) {
      globals[neighbor] -= num_interactions;
      interactions[neighbor].erase(interactions[neighbor].find(cur));
      seen_interactions[neighbor] += num_interactions;
      queue.push(std::make_pair(neighbor, seen_interactions[neighbor]));
    }
    interactions.erase(interactions.find(cur));
  }
}

// Creates disjoint sets representing the global partitions given a list of SimpleProcs
DisjointSets partition_globals(const std::vector<SimpleProc>& procs) {
  std::map<std::string, int> globals;
  std::map<std::string, std::map<std::string, int>> interactions;

  // Construct the interactions graph by looping over each CFG node
  for (const SimpleProc& proc : procs) {
    for (const SimpleBasicBlock& bb : proc.basic_blocks) {
      for (auto [cfg_node, _] : bb.nodes) {
        if (cfg_node->get_type() != CFG_NodeType::CFG_AssignNode) {
          continue;
        }
        CFG_Opd* lopd = cfg_node->get_lopd();
        std::string def = "";
        if (lopd->get_type() == CFG_OpdType::CFG_VarOpd) {
          def = lopd->get_opd_var();
          if (globals.find(def) == globals.end()) {
            globals[def] = 0;
          }
        }
        for (CFG_Opd* ropd : cfg_node->get_rhs_operands()) {
          if (ropd->get_type() == CFG_OpdType::CFG_VarOpd) {
            const std::string& use = ropd->get_opd_var();
            if (def != "") {
              if (def != use) {
                // Update the number of interactions
                ++globals[def];
                ++globals[use];
                ++interactions[def][use];
                ++interactions[use][def];
              }
            } else if (globals.find(use) == globals.end()) {
              globals[use] = 0;
            }
          }
        }
      }
    }
  }

  // Construct the partitions
  DisjointSets partitions;
  while (!globals.empty()) {
    create_partition(globals, interactions, partitions);
  }

  return partitions;
}

// Given a CFG node and llvm value, split the node into smaller nodes (involving "USEVAR" and "INPUT")
// if it involves variables from different partitions
// Return the resulting CFG node - llvm value pairs
std::vector<std::pair<CFG_Node*, llvm::Value*>> split_node(CFG_Node* node, llvm::Value* value, const DisjointSets& partitions) {
  if (node->get_type() != CFG_NodeType::CFG_AssignNode) {
    return {std::make_pair(node, value)};
  }

  CFG_Opd* lopd = node->get_lopd();
  if (lopd->get_type() == CFG_OpdType::CFG_VarOpd) {
    // Make sure all uses belong to the same partition
    std::string def = lopd->get_opd_var();
    for (const std::string& use : node->get_uses()) {
      if (!partitions.are_eq(def, use)) {
        // Split the node
        std::vector<std::pair<CFG_Node*, llvm::Value*>> res;

        std::vector<CFG_Opd*> ropds = node->get_rhs_operands();
        std::vector<llvm::Value*> operands;
        // Add "USEVAR = use" nodes for each use
        for (size_t i = 0; i < ropds.size(); ++i) {
          if (ropds[i]->get_type() == CFG_OpdType::CFG_VarOpd) {
            res.push_back(std::make_pair(
              new CFG_Node(CFG_NodeType::CFG_AssignNode, 0, "=", new CFG_Opd(CFG_OpdType::CFG_UsevarOpd),
                           new CFG_Opd(CFG_OpdType::CFG_VarOpd, ropds[i]->get_opd_var()), nullptr),
              ropds.size() == 1 ? value : llvm::dyn_cast<llvm::Instruction>(llvm::dyn_cast<llvm::StoreInst>(value)->getValueOperand())->getOperand(i)
            ));
          }
        }

        // Add an "lopd = INPUT" node
        res.push_back(std::make_pair(
          new CFG_Node(CFG_NodeType::CFG_AssignNode, 0, "=", new CFG_Opd(CFG_OpdType::CFG_VarOpd, def),
                       new CFG_Opd(CFG_OpdType::CFG_InputOpd), nullptr),
          value
        ));

        delete node;
        return res;
      }
    }
    if (value != nullptr) {
      // If the node was not split and this node maps to a llvm value, erase the llvm value if it can be
      // recovered from the CFG node
      llvm::Value* operand = nullptr;
      if (llvm::StoreInst* store = llvm::dyn_cast<llvm::StoreInst>(value)) {
        operand = store->getValueOperand();
      } else if (llvm::ReturnInst* ret = llvm::dyn_cast<llvm::ReturnInst>(value)) {
        operand = ret->getOperand(0);
      } else {
        CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Expected a store or return instruction");
      }
      if (node->get_op() == "=") {
        if (node->get_rhs_operands()[0]->get_type() != CFG_OpdType::CFG_InputOpd) {
          erase_removable_operand(operand, 0);
        }
      } else {
        erase_removable_rhs(llvm::dyn_cast<llvm::Instruction>(operand));
      }
    }
  }

  // Node was not split
  return {std::make_pair(node, value)};
}

// Converts a Simple proc to a CFG given the CFG partitions
void convert_to_cfg(SimpleProc& simple_proc, const DisjointSets& partitions) {
  // Maps basic blocks to their first and last node id in the CFG
  std::map<llvm::BasicBlock*, std::pair<int, int>> basic_blocks;

  Procedure* proc = new Procedure(simple_proc.proc_name);
  // Add the instructions for each basic block to proc and add the straight line
  // node transitions within each basic block
  for (SimpleBasicBlock& bb : simple_proc.basic_blocks) {
    int start_node_num = node_num;
    for (auto node : bb.nodes) {
      for (auto [cfg_node, value] : split_node(node.first, node.second, partitions)) {
        // Add node
        cfg_node->set_node_id(node_num);
        cfg_node->set_parent_proc(simple_proc.proc_name);
        program->add_cfg_node(cfg_node);
        proc->add_cfg_node(cfg_node);
        program->map_node_to_llvm(node_num, value);
        ++node_num;
      }
    }
    basic_blocks[bb.bb] = {start_node_num, node_num - 1};

    for (int i = start_node_num; i < node_num - 1; ++i) {
      // Add transition
      CFG_Edge* edge = new CFG_Edge(i, i + 1);
      edge->get_src()->add_out_edge(edge);
      edge->get_dst()->add_in_edge(edge);
      program->add_cfg_edge(edge);
      proc->add_cfg_edge(edge);
    }
  }

  // Add the end node
  CFG_Node* end_node = new CFG_Node(CFG_NodeType::CFG_EndNode, node_num, "END " + simple_proc.proc_name);
  end_node->set_parent_proc(simple_proc.proc_name);
  program->add_cfg_node(end_node);
  proc->add_cfg_node(end_node);
  program->map_node_to_llvm(node_num, nullptr);

  for (auto [bb_src, bb_dsts] : simple_proc.cfg_transitions) {
    int bb1_end = basic_blocks[bb_src].second;
    for (llvm::BasicBlock* outgoing : bb_dsts) {
      // Fill transitions between basic blocks
      int bb2_start = basic_blocks[outgoing].first;
      CFG_Edge* edge = new CFG_Edge(bb1_end, bb2_start);
      edge->get_src()->add_out_edge(edge);
      edge->get_dst()->add_in_edge(edge);
      program->add_cfg_edge(edge);
      proc->add_cfg_edge(edge);
    }
    if (bb_dsts.empty()) {
      // Transition to the end node
      CFG_Edge* edge = new CFG_Edge(bb1_end, node_num);
      edge->get_src()->add_out_edge(edge);
      edge->get_dst()->add_in_edge(edge);
      program->add_cfg_edge(edge);
      proc->add_cfg_edge(edge);
    }
  }

  ++node_num; // END node
  program->add_proc(proc);
  program->push_proc(proc);
}

void llvm_parse() {
  GlobalInfo globals = get_globals(module);
  std::vector<SimpleProc> procs;
  for (llvm::Function& func : *module) {
    if (!is_usable_func(&func)) {
      continue;
    }

    procs.push_back(convert_to_simple_proc(&func, globals));
  }

  DisjointSets partitions = partition_globals(procs);
  for (SimpleProc& proc : procs) {
    convert_to_cfg(proc, partitions);
  }
}
