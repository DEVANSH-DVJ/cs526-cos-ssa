#ifndef _DDG_COMPUTE_HH_
#define _DDG_COMPUTE_HH_

#include "ddg_types.hh"

#include <map>
#include <set>

// Constructs the DDG for the current partition from a CFG
void ddg_construct();
// Does constant propogation using the DDG for the current partition
// Returns a map of QDefs to their known values
std::map<QDef, int> ddg_propagate_constants();
// Simplifies the DDG by deduplicating equivalent QDefs for a single Def where possible
void ddg_reduce();
// Returns a set of QDefs that are no longer necessary to keep live (for dead code elimination)
std::set<QDef> ddg_detect_dead_qdefs();

#endif
