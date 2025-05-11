#ifndef _SSA_COMPUTE_HH_
#define _SSA_COMPUTE_HH_

// Initializes the SSA graph from the CFG
// Assignment nodes are not filled in
void ssa_init();
// Fills in assignment nodes by using the DDG for the current partition
void ssa_construct_partition();
// Finalizes the SSA graph by replacing empty assignments with empty nodes
void ssa_finalize();

#endif
