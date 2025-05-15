#include "headers.hh"

#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/InstrTypes.h>

std::string output_file;
void llvm_set_out(std::string file) {
  output_file = file;
}

extern llvm::Module* module;
extern Program* program;

constexpr const char* CUR_CONTEXT_NAME = "__cos_ssa_current_context_";

// Insert instructions to update cur_context to the correct context before a call, and restore it after the call
// transitions maps the current context to what the context should be changed to for the call
void deconstruct_context_transition(llvm::CallInst* call, llvm::GlobalVariable* cur_context, const std::map<int, int>& transitions) {
  CHECK_INVARIANT(call != nullptr, "Expected a non null call inst");
  CHECK_INVARIANT(transitions.size() > 0, "Expected at least one context transition");

  // Get the old context and restore it after the call
  llvm::Type* int_type = llvm::IntegerType::get(module->getContext(), 32);
  llvm::Value* old_context = new llvm::LoadInst(int_type, cur_context, "", call);
  new llvm::StoreInst(old_context, cur_context, call->getNextNode());
  if (transitions.size() == 1) {
    // Set the new context
    new llvm::StoreInst(llvm::ConstantInt::get(int_type, transitions.begin()->second), cur_context, call);
    return;
  }

  llvm::BasicBlock* callBB = call->getParent();
  llvm::Function* func = callBB->getParent();
  llvm::BasicBlock* chainBB = callBB->splitBasicBlockBefore(call);
  chainBB->getTerminator()->eraseFromParent();
  llvm::IRBuilder<> builder (chainBB);

  // For each possible current context, set cur_context to the context that will be transitioned to
  auto final_transition = --transitions.end();
  for (auto it = transitions.begin(); it != final_transition; ++it) {
    llvm::Value* cond = builder.CreateICmpEQ(old_context, llvm::ConstantInt::get(int_type, it->first));

    llvm::BasicBlock* trueBB = llvm::BasicBlock::Create(module->getContext(), "true", func);
    llvm::BasicBlock* falseBB = llvm::BasicBlock::Create(module->getContext(), "false", func);
    builder.CreateCondBr(cond, trueBB, falseBB);

    builder.SetInsertPoint(trueBB);
    builder.CreateStore(llvm::ConstantInt::get(int_type, it->second), cur_context);
    builder.CreateBr(callBB);
    
    builder.SetInsertPoint(falseBB);
  }

  builder.CreateStore(llvm::ConstantInt::get(int_type, final_transition->second), cur_context);
  builder.CreateBr(callBB);
}

// Returns a llvm value representing the operand
// If instructions are created to accomplish this, they are inserted before insert_before
llvm::Value* get_value(SSA_Opd* operand, int operand_num, int node_num, llvm::Instruction* insert_before,
                       std::map<std::string, llvm::GlobalVariable*>& qdef_globals) {
  llvm::Type* int_type = llvm::IntegerType::get(module->getContext(), 32);
  switch (operand->get_type()) {
    case SSA_VarOpd: {
      // Check if there is a known value or if this it a promoted return value
      std::pair<int, int> meta = operand->get_meta_num();
      int value;
      if (program->get_dfg_propagated_value({{operand->get_opd_var(), meta.first}, meta.second}, &value)) {
        return llvm::ConstantInt::get(int_type, value);
      }
      // Else fall down to the SSA_PhiOpd case
    }
    case SSA_PhiOpd:
      // Check if this corresponds to a ret var
      if (llvm::CallInst* call = program->get_llvm_call_operand_at_node(node_num, operand_num)) {
        return call;
      }
      return new llvm::LoadInst(program->get_llvm_type(operand->get_opd_var()),
                                qdef_globals[operand->get_opd_var()], "", insert_before);
    case SSA_NumOpd:
      return llvm::ConstantInt::get(int_type, operand->get_opd_value());
    case SSA_InputOpd: {
      llvm::Value* value = program->get_llvm_node(operand->get_meta_num().first, true);
      if (llvm::StoreInst* store = llvm::dyn_cast<llvm::StoreInst>(value)) {
        return store->getValueOperand();
      } else if (llvm::ReturnInst* ret = llvm::dyn_cast<llvm::ReturnInst>(value)) {
        return ret->getOperand(0);
      }
      CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Expected store or return inst");
    }
    default:
      CHECK_INVARIANT(false, "Expected a variable or number operand");
      return nullptr;
  }
}

// Decomposes stmts into an assignment
// If this is a USEVAR or keep_as_reg == true, updated RHS with the result 
// Otherwise, stores the result to the LHS
// Any created instructions are inserted before insert_before
void create_assignment(std::list<SSA_Stmt*>* stmts, llvm::Instruction* insert_before, bool keep_as_reg,
                       std::map<std::string, llvm::GlobalVariable*>& qdef_globals,
                       llvm::Value** rhs) {
  CHECK_INVARIANT(stmts->size() > 0, "Expected at least one statement for each metamorphic assignment");
  auto final_stmt = --stmts->end();
  CHECK_INVARIANT((*final_stmt)->get_type() == SSA_AssignStmt, "Expected final stmt to be an assign stmt");

  // Phi nodes are implicitly decomposed, so only the final stmt needs to be handled
  std::vector<SSA_Opd*> operands = (*final_stmt)->get_rhs();
  std::string op = (*final_stmt)->get_op();
  int node_num = (*final_stmt)->get_lhs()->get_meta_num().first;
  llvm::Value* stored_value;
  if (op == "=") {
    CHECK_INVARIANT(operands.size() == 1, "Expected 1 operand");
    stored_value = get_value(operands[0], 0, node_num, insert_before, qdef_globals);
  } else {
    llvm::Value* v1 = get_value(operands[0], 0, node_num, insert_before, qdef_globals);
    llvm::Value* v2 = get_value(operands[1], 1, node_num, insert_before, qdef_globals);
    if (op == "+") {
      llvm::Instruction* inst = llvm::BinaryOperator::CreateAdd(v1, v2);
      inst->insertBefore(insert_before);
      stored_value = inst;
    } else if (op == "-") {
      llvm::Instruction* inst = llvm::BinaryOperator::CreateSub(v1, v2);
      inst->insertBefore(insert_before);
      stored_value = inst;
    } else if (op == "*") {
      llvm::Instruction* inst = llvm::BinaryOperator::CreateMul(v1, v2);
      inst->insertBefore(insert_before);
      stored_value = inst;
    } else if (op == "/") {
      llvm::Instruction* inst = llvm::BinaryOperator::CreateSDiv(v1, v2);
      inst->insertBefore(insert_before);
      stored_value = inst;
    } else {
      CHECK_INVARIANT(false, "Control should not reach");
    }
  }

  // Update RHS or create a store
  SSA_Opd* lhs = (*final_stmt)->get_lhs();
  if (lhs->get_type() == SSA_UsevarOpd || keep_as_reg) {
    *rhs = stored_value;
    return;
  }

  llvm::GlobalVariable* store_loc = qdef_globals[(*final_stmt)->get_lhs()->get_opd_var()];
  new llvm::StoreInst(stored_value, store_loc, insert_before);
}

// Deconstruct a group of metamorphic assignments representing assign
void deconstruct_metamorphic_assign(std::map<int, SSA_Meta*>* metas, llvm::Instruction* assign, llvm::GlobalVariable* cur_context,
                                    std::map<std::string, llvm::GlobalVariable*>& qdef_globals) {
  CHECK_INVARIANT(assign != nullptr, "Expected a non null load/store inst");
  CHECK_INVARIANT(metas->size() > 0, "Expected at least one meta assignment");

  bool defs_return_variable = llvm::isa<llvm::ReturnInst>(assign);
  llvm::BasicBlock* assignBB = assign->getParent();
  llvm::Value* rhs;
  if (metas->size() == 1) {
    // Non metamorphic assignment case
    create_assignment(metas->begin()->second->get_stmts(), assign, defs_return_variable, qdef_globals, &rhs);
    if (llvm::isa<llvm::LoadInst>(assign)) {
      assign->replaceAllUsesWith(rhs);
    }
    if (llvm::isa<llvm::CallInst>(assign)) {
      if (assign != rhs) {
        assign->replaceAllUsesWith(rhs);
      }
    } else {
      assign->eraseFromParent();
    }
    if (defs_return_variable) {
      llvm::IRBuilder<> builder (assignBB);
      builder.CreateRet(rhs);
    }
    return;
  }

  llvm::Function* func = assignBB->getParent();
  llvm::BasicBlock* chainBB = assignBB->splitBasicBlockBefore(assign);
  chainBB->getTerminator()->eraseFromParent();
  llvm::IRBuilder<> builder (chainBB);

  llvm::Type* int_type = llvm::IntegerType::get(module->getContext(), 32);
  llvm::Value* cur_context_value = builder.CreateLoad(int_type, cur_context);

  // For each metamorphic assignment, create the corresponding assignment depending on the current context
  auto final_meta = --metas->end();
  for (auto it = metas->begin(); it != final_meta; ++it) {
    llvm::Value* cond = builder.CreateICmpEQ(cur_context_value, llvm::ConstantInt::get(int_type, it->second->get_meta_num().second));

    llvm::BasicBlock* trueBB = llvm::BasicBlock::Create(module->getContext(), "true", func);
    llvm::BasicBlock* falseBB = llvm::BasicBlock::Create(module->getContext(), "false", func);
    builder.CreateCondBr(cond, trueBB, falseBB);

    builder.SetInsertPoint(trueBB);
    llvm::Instruction* br = builder.CreateBr(assignBB);

    create_assignment(it->second->get_stmts(), br, defs_return_variable, qdef_globals, &rhs);
    if (defs_return_variable) {
      br->eraseFromParent();
      builder.CreateRet(rhs);
    }

    builder.SetInsertPoint(falseBB);
  }

  llvm::Instruction* br = builder.CreateBr(assignBB);
  create_assignment(final_meta->second->get_stmts(), br, defs_return_variable, qdef_globals, &rhs);
  if (defs_return_variable) {
    assign->eraseFromParent();
    builder.CreateRet(rhs);
    return;
  }

  assign->eraseFromParent();
}

// Lower the SSA back to LLVM for a single partition
void deconstruct_single_partition(std::map<std::string, llvm::GlobalVariable*>& qdef_globals, const std::string& context_var_name) {
  // Determine which functions actually have metamorphic assignments and thus require context transitions when called
  std::set<std::string> func_uses_context;
  for (auto pair : *program->get_procs()) {
    for (int node : pair.second->get_ssa_nodes()) {
      SSA_Node* ssa_node = program->get_ssa_node(node, true);
      if (ssa_node->get_type() == SSA_AssignNode && ssa_node->get_metas()->size() > 1) {
        func_uses_context.insert(pair.first);
        break;
      }
    }
  }

  llvm::Type* int_type = llvm::IntegerType::get(module->getContext(), 32);
  llvm::GlobalVariable* cur_context = new llvm::GlobalVariable(*module, int_type, false, llvm::GlobalValue::InternalLinkage,
                                                           llvm::ConstantInt::get(int_type, 0), context_var_name);

  // Deconstruct each SSA node in the current partition
  for (auto pair : *program->get_procs()) {
    for (int node : pair.second->get_ssa_nodes()) {
      if (program->is_part_of_other_partition(node)) {
        continue;
      }

      SSA_Node* ssa_node = program->get_ssa_node(node, true);
      llvm::Value* value = program->get_llvm_node(node, true);
      if (value == nullptr) {
        continue;
      }

      if (ssa_node->get_type() == SSA_NodeType::SSA_EmptyNode) {
        // Dead node case
        if (llvm::isa<llvm::ReturnInst>(value)) {
          continue;
        }
        if (llvm::Instruction* inst = llvm::dyn_cast<llvm::Instruction>(value)) {
          inst->replaceAllUsesWith(llvm::ConstantInt::get(int_type, 0));
          inst->eraseFromParent();
        }
        continue;
      }

      if (ssa_node->get_type() == SSA_NodeType::SSA_CallNode) {
        // Handle context transitions
        if (func_uses_context.find(ssa_node->get_callee()) != func_uses_context.end()) {
          deconstruct_context_transition(llvm::dyn_cast<llvm::CallInst>(value), cur_context, program->get_dfg_transitions(ssa_node->get_node_id()));
        }
        continue;
      }

      // Handle metamorphic assignments
      CHECK_INVARIANT(ssa_node->get_type() == SSA_NodeType::SSA_AssignNode, "Expected assign node");
      std::map<int, SSA_Meta*>* metas = ssa_node->get_metas();
      deconstruct_metamorphic_assign(metas, llvm::dyn_cast<llvm::Instruction>(value), cur_context, qdef_globals);
    }
  }

  if (cur_context->getNumUses() == 0) {
    cur_context->eraseFromParent();
  }
}

void ssa_deconstruct() {
  std::map<std::string, llvm::GlobalVariable*> qdef_globals;
  for (llvm::GlobalVariable& global : module->globals()) {
    qdef_globals[global.getName().str()] = &global;
  }

  // Deconstruct each partition with its own context variable
  int num_partitions = program->get_num_partitions();
  for (int cur_partition = 0; cur_partition < num_partitions; ++cur_partition) {
    program->set_cur_partition(cur_partition);
    deconstruct_single_partition(qdef_globals, CUR_CONTEXT_NAME + std::to_string(cur_partition));
  }
}

void llvm_dump() {
  std::error_code error;
  llvm::raw_fd_ostream output = llvm::raw_fd_ostream(output_file, error);
  if (error) {
    llvm::errs() << "Failed to open LLVM IR output file: " << error.message() << '\n';
    CHECK_INVARIANT(false, "");
  }
  module->print(output, nullptr);
}
