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

void set_context_to(llvm::GlobalVariable* cur_context, int new_context, llvm::Instruction* insert_before) {
  llvm::Type* int_type = llvm::IntegerType::get(module->getContext(), 32);
  llvm::Value* old_context = new llvm::LoadInst(int_type, cur_context, "", insert_before);
  new llvm::StoreInst(llvm::ConstantInt::get(int_type, new_context), cur_context, insert_before);
  new llvm::StoreInst(old_context, cur_context, insert_before->getNextNode());
}

void deconstruct_context_transition(llvm::CallInst* call, llvm::GlobalVariable* cur_context, const std::map<int, int>& transitions) {
  CHECK_INVARIANT(call != nullptr, "Expected a non null call inst");
  CHECK_INVARIANT(transitions.size() > 0, "Expected at least one context transition");

  if (transitions.size() == 1) {
    set_context_to(cur_context, transitions.begin()->second, call);
    return;
  }

  llvm::BasicBlock* callBB = call->getParent();
  llvm::Function* func = callBB->getParent();
  llvm::BasicBlock* chainBB = callBB->splitBasicBlockBefore(call);
  chainBB->getTerminator()->eraseFromParent();
  llvm::IRBuilder<> builder (chainBB);

  llvm::Type* int_type = llvm::IntegerType::get(module->getContext(), 32);
  llvm::Value* cur_context_value = builder.CreateLoad(int_type, cur_context);

  auto final_transition = --transitions.end();
  for (auto it = transitions.begin(); it != final_transition; ++it) {
    llvm::Value* cond = builder.CreateICmpEQ(cur_context_value, llvm::ConstantInt::get(int_type, it->first));

    llvm::BasicBlock* trueBB = llvm::BasicBlock::Create(module->getContext(), "true", func);
    llvm::BasicBlock* falseBB = llvm::BasicBlock::Create(module->getContext(), "false", func);
    builder.CreateCondBr(cond, trueBB, falseBB);

    builder.SetInsertPoint(trueBB);
    llvm::Instruction* br = builder.CreateBr(callBB);
    set_context_to(cur_context, it->second, br);
    
    builder.SetInsertPoint(falseBB);
  }

  llvm::Instruction* br = builder.CreateBr(callBB);
  set_context_to(cur_context, final_transition->second, br);
}

llvm::GlobalVariable* get_global(SSA_Opd* operand, std::map<std::string, llvm::GlobalVariable*>& qdef_globals) {
  return qdef_globals[operand->get_opd_var()];
}

llvm::Value* get_value(SSA_Opd* operand, int operand_num, int node_num, llvm::Instruction* insert_before,
                       std::map<std::string, llvm::GlobalVariable*>& qdef_globals) {
  llvm::Type* int_type = llvm::IntegerType::get(module->getContext(), 32);
  switch (operand->get_type()) {
    case SSA_VarOpd: {
      std::pair<int, int> meta = operand->get_meta_num();
      int value;
      if (program->get_ddg_propagated_value({{operand->get_opd_var(), meta.first}, meta.second}, &value)) {
        return llvm::ConstantInt::get(int_type, value);
      }
      if (llvm::CallInst* call = program->get_llvm_call_operand_at_node(node_num, operand_num)) {
        return call;
      }
      // Else fall down to the SSA_PhiOpd case
    }
    case SSA_PhiOpd:
      return new llvm::LoadInst(int_type, get_global(operand, qdef_globals), "", insert_before);
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

void create_assignment(std::list<SSA_Stmt*>* stmts, llvm::Instruction* insert_before, bool keepAsReg,
                       std::map<std::string, llvm::GlobalVariable*>& qdef_globals,
                       std::vector<std::pair<llvm::Value*, llvm::BasicBlock*>>& phi_node_incoming) {
  CHECK_INVARIANT(stmts->size() > 0, "Expected at least one statement for each metamorphic assignment");
  auto final_stmt = --stmts->end();
  CHECK_INVARIANT((*final_stmt)->get_type() == SSA_AssignStmt, "Expected final stmt to be an assign stmt");

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

  SSA_Opd* lhs = (*final_stmt)->get_lhs();
  if (lhs->get_type() == SSA_UsevarOpd || keepAsReg) {
    phi_node_incoming.push_back(std::make_pair(stored_value, insert_before->getParent()));
    return;
  }

  llvm::GlobalVariable* store_loc = get_global((*final_stmt)->get_lhs(), qdef_globals);

  new llvm::StoreInst(stored_value, store_loc, insert_before);
}

void deconstruct_metamorphic_assign(std::map<int, SSA_Meta*>* metas, llvm::Instruction* assign, llvm::GlobalVariable* cur_context,
                                    std::map<std::string, llvm::GlobalVariable*>& qdef_globals) {
  CHECK_INVARIANT(assign != nullptr, "Expected a non null load/store inst");
  CHECK_INVARIANT(metas->size() > 0, "Expected at least one meta assignment");

  bool defsReturnVariable = llvm::isa<llvm::ReturnInst>(assign);
  llvm::BasicBlock* assignBB = assign->getParent();
  std::vector<std::pair<llvm::Value*, llvm::BasicBlock*>> phi_node_incoming;
  if (metas->size() == 1) {
    create_assignment(metas->begin()->second->get_stmts(), assign, defsReturnVariable, qdef_globals, phi_node_incoming);
    if (llvm::isa<llvm::LoadInst>(assign)) {
      assign->replaceAllUsesWith(phi_node_incoming[0].first);
    }
    assign->eraseFromParent();
    if (defsReturnVariable) {
      llvm::IRBuilder<> builder (assignBB);
      builder.CreateRet(phi_node_incoming[0].first);
    }
    return;
  }

  llvm::Function* func = assignBB->getParent();
  llvm::BasicBlock* chainBB = assignBB->splitBasicBlockBefore(assign);
  chainBB->getTerminator()->eraseFromParent();
  llvm::IRBuilder<> builder (chainBB);

  llvm::Type* int_type = llvm::IntegerType::get(module->getContext(), 32);
  llvm::Value* cur_context_value = builder.CreateLoad(int_type, cur_context);

  auto final_meta = --metas->end();
  for (auto it = metas->begin(); it != final_meta; ++it) {
    llvm::Value* cond = builder.CreateICmpEQ(cur_context_value, llvm::ConstantInt::get(int_type, it->second->get_meta_num().second));

    llvm::BasicBlock* trueBB = llvm::BasicBlock::Create(module->getContext(), "true", func);
    llvm::BasicBlock* falseBB = llvm::BasicBlock::Create(module->getContext(), "false", func);
    builder.CreateCondBr(cond, trueBB, falseBB);

    builder.SetInsertPoint(trueBB);
    llvm::Instruction* br = builder.CreateBr(assignBB);

    create_assignment(it->second->get_stmts(), br, defsReturnVariable, qdef_globals, phi_node_incoming);
    if (defsReturnVariable) {
      br->eraseFromParent();
      builder.CreateRet(phi_node_incoming.back().first);
    }

    builder.SetInsertPoint(falseBB);
  }

  llvm::Instruction* br = builder.CreateBr(assignBB);
  create_assignment(final_meta->second->get_stmts(), br, defsReturnVariable, qdef_globals, phi_node_incoming);
  if (defsReturnVariable) {
    assign->eraseFromParent();
    builder.CreateRet(phi_node_incoming.back().first);
    return;
  }

  if (llvm::LoadInst* load = llvm::dyn_cast<llvm::LoadInst>(assign)) {
    llvm::PHINode* phi = llvm::PHINode::Create(int_type, phi_node_incoming.size());
    phi->insertBefore(assignBB->getFirstNonPHI());
    for (auto pair : phi_node_incoming) {
      phi->addIncoming(pair.first, pair.second);
    }
    load->replaceAllUsesWith(phi);
  }

  assign->eraseFromParent();
}

void deconstruct_single_partition(std::map<std::string, llvm::GlobalVariable*>& qdef_globals, const std::string& context_var_name) {
  std::set<std::string> funcUsesContext;
  for (auto pair : *program->get_procs()) {
    for (int node : pair.second->get_ssa_nodes()) {
      SSA_Node* ssa_node = program->get_ssa_node(node, true);
      if (ssa_node->get_type() == SSA_AssignNode && ssa_node->get_metas()->size() > 1) {
        funcUsesContext.insert(pair.first);
        break;
      }
    }
  }

  llvm::Type* int_type = llvm::IntegerType::get(module->getContext(), 32);
  llvm::GlobalVariable* cur_context = new llvm::GlobalVariable(int_type, false, llvm::GlobalValue::InternalLinkage,
                                                           llvm::ConstantInt::get(int_type, 0), context_var_name);
  module->insertGlobalVariable(cur_context);

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
        if (funcUsesContext.find(ssa_node->get_callee()) != funcUsesContext.end()) {
          deconstruct_context_transition(llvm::dyn_cast<llvm::CallInst>(value), cur_context, program->get_ddg_transitions(ssa_node->get_node_id()));
        }
        continue;
      }

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
