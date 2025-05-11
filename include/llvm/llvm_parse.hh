#ifndef _LLVM_PARSE_HH_
#define _LLVM_PARSE_HH_

#include <llvm/IR/Value.h>
#include <map>

// Sets up the Module input for other LLVM functions
void llvm_set_in(llvm::Module* module);
// Constructs the CFG from the LLVM IR
void llvm_parse();

#endif
