#ifndef _DFG_COMPUTE_HH_
#define _DFG_COMPUTE_HH_

#include "dfg_types.hh"

#include <map>
#include <set>

// Constructs the DFG for the current partition from a CFG
void dfg_construct();
// Does constant propogation using the DFG for the current partition
// Returns a map of QDefs to their known values
std::map<QDef, int> dfg_propagate_constants();
// Simplifies the DFG by deduplicating equivalent QDefs for a single Def where possible
void dfg_reduce();
// Returns a set of QDefs that are no longer necessary to keep live (for dead code elimination)
std::set<QDef> dfg_detect_dead_qdefs();

#endif
