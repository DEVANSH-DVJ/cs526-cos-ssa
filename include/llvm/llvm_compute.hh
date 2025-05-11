#ifndef _LLVM_COMPUTE_HH_
#define _LLVM_COMPUTE_HH_

#include <string>

// Lowers the SSA back to LLVM
void ssa_deconstruct();
// Sets the file for where the output LLVM IR will be dumped
void llvm_set_out(std::string file);
// Dumps the output LLVM IR into a file
void llvm_dump();

#endif
