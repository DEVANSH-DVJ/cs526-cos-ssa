#include "../headers.hh"

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

std::string func_ret_to_global(llvm::Function* func) {
  return "$" + func->getName().str() + "_ret";
}

bool is_usable_func(llvm::Function* func) {
  // Get other generated code (like templates), use return !func->isDeclaration();
  /*return !func->isDeclaration();*/
  return !(func->isDeclaration() || func->getName().starts_with("_Z") || func->getName().starts_with("__") || func->getName().starts_with("_GLOBAL__"));
}

std::map<llvm::BasicBlock*, std::set<llvm::BasicBlock*>> construct_llvm_cfg(llvm::Function* func) {
  std::map<llvm::BasicBlock*, std::set<llvm::BasicBlock*>> cfg_transitions;
  std::queue<llvm::BasicBlock*> worklist;
  worklist.push(&func->getEntryBlock());
  cfg_transitions[&func->getEntryBlock()] = std::set<llvm::BasicBlock*>();

  while (!worklist.empty()) {
    llvm::BasicBlock* bb = worklist.front();
    worklist.pop();

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

typedef struct GlobalInfo {
  std::set<llvm::GlobalVariable*> globals;
  std::map<llvm::Instruction*, std::string> loads;
  std::map<llvm::Instruction*, std::string> stores;
  std::set<std::string> functions;
} GlobalInfo;

GlobalInfo get_globals(llvm::Module* module) {
  std::set<std::string> addr_taken;
  // Probably a better way to check if a global's addr is taken
  for (llvm::GlobalVariable& var : module->globals()) {
    if (var.getNumOperands() != 1) {
      addr_taken.insert(var.getName().str());
      break;
    }
    bool found = false;
    for (llvm::Use& use : var.uses()) {
      llvm::User* user = use.getUser();
      if (llvm::LoadInst* load = llvm::dyn_cast<llvm::LoadInst>(user)) {
        if (&var != load->getPointerOperand()) {
          found = true;
          break;
        }
      } else if (llvm::StoreInst* load = llvm::dyn_cast<llvm::StoreInst>(user)) {
        if (&var != load->getPointerOperand()) {
          found = true;
          break;
        }
      } else {
        found = true;
        break;
      }
    }
    if (found) {
      addr_taken.insert(var.getName().str());
      break;
    }
  }

  GlobalInfo globals;
  for (llvm::Function& func : *module) {
    if (!is_usable_func(&func)) {
      continue;
    }

    globals.functions.insert(func.getName().str());
    for (llvm::BasicBlock& bb : func) {
      for (llvm::Instruction& inst : bb) {
        if (inst.isVolatile()) {
          continue;
        }
        if (llvm::LoadInst* load = llvm::dyn_cast<llvm::LoadInst>(&inst)) {
          if (llvm::GlobalVariable* var = llvm::dyn_cast<llvm::GlobalVariable>(load->getPointerOperand())) {
            if (addr_taken.find(var->getName().str()) == addr_taken.end() && var->getNumOperands() == 1) {
              globals.globals.insert(var);
              globals.loads[&inst] = var->getName();
            }
          }
        } else if (llvm::StoreInst* store = llvm::dyn_cast<llvm::StoreInst>(&inst)) {
          if (llvm::GlobalVariable* var = llvm::dyn_cast<llvm::GlobalVariable>(store->getPointerOperand())) {
            if (addr_taken.find(var->getName().str()) == addr_taken.end() && var->getNumOperands() == 1) {
              globals.globals.insert(var);
              globals.stores[&inst] = var->getName();
            }
          }
        // Handle non scalar types
        /*} else if (llvm::GetElementPtrInst* gep = llvm::dyn_cast<llvm::GetElementPtrInst>(&inst)) {*/
        /*  if (llvm::GlobalVariable* var = llvm::dyn_cast<llvm::GlobalVariable>(gep->getPointerOperand())) {*/
        /*    if (addr_taken.find(var->getName().str()) == addr_taken.end()) {*/
        /*      globals.globals.insert(var);*/
        /*      globals.loads[&inst] = var->getName();*/
        /*    }*/
        /*  }*/
        // TODO: handle parameters
        // "Promote" returns to globals
        } else if (llvm::CallInst* call = llvm::dyn_cast<llvm::CallInst>(&inst)) {
          if (is_usable_func(call->getCalledFunction())) {
            globals.loads[&inst] = func_ret_to_global(call->getCalledFunction());
          }
        } else if (llvm::isa<llvm::ReturnInst>(&inst) && func.getName().str() != "main") {
          globals.stores[&inst] = func_ret_to_global(&func);
        }
      }
    }
  }
  return globals;
}

void mark_removable_operand(llvm::Value* value, std::set<llvm::Value*>& removable_uses) {
  llvm::LoadInst* load;
  if ((load = llvm::dyn_cast<llvm::LoadInst>(value)) && load->getNumUses() == 1) {
    removable_uses.insert(load);
  }
}

void mark_removable_inst(llvm::Instruction* inst, std::set<llvm::Value*>& removable_uses) {
  CHECK_INVARIANT(inst->getNumOperands() == 2, "Expected instruction with two operands");
  
  mark_removable_operand(inst->getOperand(0), removable_uses);
  mark_removable_operand(inst->getOperand(1), removable_uses);
}

void erase_removable_operand(llvm::Value* value) {
  llvm::LoadInst* load;
  if ((load = llvm::dyn_cast<llvm::LoadInst>(value)) && load->getNumUses() == 1) {
    llvm::Type* int_type = llvm::IntegerType::get(module->getContext(), 32);
    load->replaceAllUsesWith(llvm::ConstantInt::get(int_type, 0));
    load->eraseFromParent();
  }
}

void erase_removable_rhs(llvm::Instruction* inst) {
  erase_removable_operand(inst->getOperand(0));
  erase_removable_operand(inst->getOperand(1));
  if (inst->getNumUses() == 1) {
    llvm::Type* int_type = llvm::IntegerType::get(module->getContext(), 32);
    inst->replaceAllUsesWith(llvm::ConstantInt::get(int_type, 0));
    inst->eraseFromParent();
  }
}

bool get_operand_repr(llvm::Value* operand, CFG_Opd** repr, const GlobalInfo& globals) {
  if (llvm::Instruction* inst = llvm::dyn_cast<llvm::Instruction>(operand)) {
    auto it = globals.loads.find(inst);
    if (it != globals.loads.end()) {
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

struct RHS {
  std::string op;
  CFG_Opd* ropd1;
  CFG_Opd* ropd2;
};

RHS get_assignment_value(llvm::Value* value, const GlobalInfo& globals, std::set<llvm::Value*>& removable_uses) {
  static const std::map<unsigned int, char> ops {
    {llvm::Instruction::Add, '+'},
    {llvm::Instruction::Sub, '-'},
    {llvm::Instruction::Mul, '*'},
    {llvm::Instruction::SDiv, '/'}
  };

  CFG_Opd* ropd1;
  if (get_operand_repr(value, &ropd1, globals)) {
    mark_removable_operand(value, removable_uses);
    return {"=", ropd1, nullptr};
  }
  if (llvm::Instruction* inst = llvm::dyn_cast<llvm::Instruction>(value)) {
    auto it = ops.find(inst->getOpcode());
    if (it != ops.end()) {
      if (inst->getNumOperands() == 2) {
        if (get_operand_repr(inst->getOperand(0), &ropd1, globals)) {
          CFG_Opd* ropd2;
          if (get_operand_repr(inst->getOperand(1), &ropd2, globals)) {
            mark_removable_inst(inst, removable_uses);
            return {std::string(1, it->second), ropd1, ropd2};
          }
          delete ropd1;
        }
      }
    }
  }

  return {"=", new CFG_Opd(CFG_OpdType::CFG_InputOpd), nullptr};
}

std::vector<std::pair<CFG_Node*, llvm::Value*>> get_nodes_in_basic_block(const std::string& proc, llvm::BasicBlock* bb, const GlobalInfo& globals) {
  std::vector<std::pair<CFG_Node*, llvm::Value*>> res;
  std::set<llvm::Value*> removable_uses;

  if (bb->isEntryBlock()) {
    res.push_back(std::make_pair(new CFG_Node(CFG_NodeType::CFG_StartNode, 0, "START " + proc), nullptr));

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

  for (llvm::Instruction& inst : *bb) {
    auto it = globals.loads.find(&inst);
    if (it != globals.loads.end()) {
      if (llvm::CallInst* call = llvm::dyn_cast<llvm::CallInst>(&inst)) {
        auto it = globals.functions.find(call->getCalledFunction()->getName().str());
        if (it != globals.functions.end()) {
          res.push_back(std::make_pair(new CFG_Node(CFG_NodeType::CFG_CallNode, 0, "CALL " + *it, *it), call));
        }
      } else {
        // If this is actually not used by a USEVAR, this will be removed later
        res.push_back(std::make_pair(
          new CFG_Node(CFG_NodeType::CFG_AssignNode, 0, "=", new CFG_Opd(CFG_OpdType::CFG_UsevarOpd), new CFG_Opd(CFG_OpdType::CFG_VarOpd, it->second), nullptr),
          &inst
        ));
      }
    } else {
      auto it = globals.stores.find(&inst);
      if (it != globals.stores.end()) {
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

struct SimpleBasicBlock {
  llvm::BasicBlock* bb;
  std::vector<std::pair<CFG_Node*, llvm::Value*>> nodes;
};

struct SimpleProc {
  std::string proc_name;
  std::map<llvm::BasicBlock*, std::set<llvm::BasicBlock*>> cfg_transitions;
  std::vector<SimpleBasicBlock> basic_blocks;
};

SimpleProc convert_to_simple_proc(llvm::Function* func, const GlobalInfo& globals) {
  SimpleProc proc;
  proc.proc_name = func->getName().str();
  proc.cfg_transitions = construct_llvm_cfg(func);

  for (auto pair : proc.cfg_transitions) {
    proc.basic_blocks.push_back({pair.first, get_nodes_in_basic_block(proc.proc_name, pair.first, globals)});
  }

  return proc;
}

class DisjointSets {
public:
  bool are_eq(const std::string& l, const std::string& r) const {
    return find_str(l) == find_str(r);
  }

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
  mutable std::map<std::string, int> globals;
  mutable std::vector<int> dset;

  int find_str(const std::string& s) const {
    auto it = globals.find(s);
    if (it == globals.end()) {
      globals[s] = dset.size();
      dset.push_back(-1);
      it = globals.find(s);
    }
    return find(it->second);
  }

  int find(int n) const {
    if (dset[n] < 0) {
      return n;
    }
    return dset[n] = find(dset[n]);
  }
};

std::string get_most_interactions(const std::map<std::string, int>& globals) {
  CHECK_INVARIANT(globals.size() > 0, "Cannot partition empty globals");
  std::string var;
  int max_interactions = -1;
  for (auto pair : globals) {
    if (pair.second > max_interactions) {
      var = pair.first;
      max_interactions = pair.second;
    }
  }
  return var;
}

constexpr int MAX_PARTITION_SIZE = 10;
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

  std::map<std::string, int> seen_interactions;
  for (int i = 0; i < MAX_PARTITION_SIZE; ++i) {
    if (globals.empty()) {
      return;
    }

    std::string cur;
    bool found = false;
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

    for (auto pair : interactions[cur]) {
      globals[pair.first] -= pair.second;
      interactions[pair.first].erase(interactions[pair.first].find(cur));
      seen_interactions[pair.first] += pair.second;
      queue.push(std::make_pair(pair.first, seen_interactions[pair.first]));
    }
    interactions.erase(interactions.find(cur));
  }
}

DisjointSets partition_globals(const std::vector<SimpleProc>& procs) {
  std::map<std::string, int> globals;
  std::map<std::string, std::map<std::string, int>> interactions;

  for (const SimpleProc& proc : procs) {
    for (const SimpleBasicBlock& bb : proc.basic_blocks) {
      for (auto pair : bb.nodes) {
        if (pair.first->get_type() != CFG_NodeType::CFG_AssignNode) {
          continue;
        }
        CFG_Opd* lopd = pair.first->get_lopd();
        std::string def = "";
        if (lopd->get_type() == CFG_OpdType::CFG_VarOpd) {
          def = lopd->get_opd_var();
          if (globals.find(def) == globals.end()) {
            globals[def] = 0;
          }
        }
        for (CFG_Opd* ropd : pair.first->get_rhs_operands()) {
          if (ropd->get_type() == CFG_OpdType::CFG_VarOpd) {
            const std::string& use = ropd->get_opd_var();
            if (def != "") {
              if (def != use) {
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

  DisjointSets partitions;
  while (!globals.empty()) {
    create_partition(globals, interactions, partitions);
  }

  return partitions;
}

std::vector<std::pair<CFG_Node*, llvm::Value*>> split_node(CFG_Node* node, llvm::Value* value, const DisjointSets& partitions) {
  if (node->get_type() != CFG_NodeType::CFG_AssignNode) {
    return {std::make_pair(node, value)};
  }

  CFG_Opd* lopd = node->get_lopd();
  if (lopd->get_type() == CFG_OpdType::CFG_VarOpd) {
    std::string def = lopd->get_opd_var();
    for (const std::string& use : node->get_uses()) {
      if (!partitions.are_eq(def, use)) {
        std::vector<std::pair<CFG_Node*, llvm::Value*>> res;

        std::vector<CFG_Opd*> ropds = node->get_rhs_operands();
        std::vector<llvm::Value*> operands;
        for (size_t i = 0; i < ropds.size(); ++i) {
          if (ropds[i]->get_type() == CFG_OpdType::CFG_VarOpd) {
            res.push_back(std::make_pair(
              new CFG_Node(CFG_NodeType::CFG_AssignNode, 0, "=", new CFG_Opd(CFG_OpdType::CFG_UsevarOpd),
                           new CFG_Opd(CFG_OpdType::CFG_VarOpd, ropds[i]->get_opd_var()), nullptr),
              ropds.size() == 1 ? value : llvm::dyn_cast<llvm::Instruction>(llvm::dyn_cast<llvm::StoreInst>(value)->getValueOperand())->getOperand(i)
            ));
          }
        }

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
      llvm::Value* operand = nullptr;
      if (llvm::StoreInst* store = llvm::dyn_cast<llvm::StoreInst>(value)) {
        operand = store->getValueOperand();
      } else if (llvm::ReturnInst* ret = llvm::dyn_cast<llvm::ReturnInst>(value)) {
        operand = ret->getOperand(0);
      } else {
        CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Expected a store or return instruction");
      }
      if (node->get_op() == "=") {
        erase_removable_operand(value);
      } else {
        erase_removable_rhs(llvm::dyn_cast<llvm::Instruction>(operand));
      }
    }
  }

  return {std::make_pair(node, value)};
}

void convert_to_cfg(SimpleProc& simple_proc, const DisjointSets& partitions, std::map<int, llvm::Value*>& node_to_llvm) {
  std::map<llvm::BasicBlock*, std::pair<int, int>> basic_blocks;

  Procedure* proc = new Procedure(simple_proc.proc_name);
  for (SimpleBasicBlock& bb : simple_proc.basic_blocks) {
    int start_node_num = node_num;
    for (auto node : bb.nodes) {
      for (auto pair : split_node(node.first, node.second, partitions)) {
        pair.first->set_node_id(node_num++);
        pair.first->set_parent_proc(simple_proc.proc_name);
        program->add_cfg_node(pair.first);
        proc->add_cfg_node(pair.first);
        node_to_llvm[node_to_llvm.size() + 1] = pair.second;
      }
    }
    basic_blocks[bb.bb] = {start_node_num, node_num - 1};

    for (int i = start_node_num; i < node_num - 1; ++i) {
      CFG_Edge* edge = new CFG_Edge(i, i + 1);
      edge->get_src()->add_out_edge(edge);
      edge->get_dst()->add_in_edge(edge);
      program->add_cfg_edge(edge);
      proc->add_cfg_edge(edge);
    }
  }

  CFG_Node* end_node = new CFG_Node(CFG_NodeType::CFG_EndNode, node_num, "END " + simple_proc.proc_name);
  end_node->set_parent_proc(simple_proc.proc_name);
  program->add_cfg_node(end_node);
  proc->add_cfg_node(end_node);
  node_to_llvm[node_to_llvm.size() + 1] = nullptr;

  for (auto pair : simple_proc.cfg_transitions) {
    int bb1_end = basic_blocks[pair.first].second;
    for (llvm::BasicBlock* outgoing : pair.second) {
      int bb2_start = basic_blocks[outgoing].first;
      CFG_Edge* edge = new CFG_Edge(bb1_end, bb2_start);
      edge->get_src()->add_out_edge(edge);
      edge->get_dst()->add_in_edge(edge);
      program->add_cfg_edge(edge);
      proc->add_cfg_edge(edge);
    }
    if (pair.second.empty()) {
      CFG_Edge* edge = new CFG_Edge(bb1_end, node_num); // Transition to END node
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

std::map<int, llvm::Value*> llvm_parse() {
  GlobalInfo globals = get_globals(module);
  std::vector<SimpleProc> procs;
  for (llvm::Function& func : *module) {
    if (!is_usable_func(&func)) {
      continue;
    }

    procs.push_back(convert_to_simple_proc(&func, globals));
  }

  DisjointSets partitions = partition_globals(procs);
  std::map<int, llvm::Value*> node_to_llvm;
  for (SimpleProc& proc : procs) {
    convert_to_cfg(proc, partitions, node_to_llvm);
  }

  return node_to_llvm;
}

