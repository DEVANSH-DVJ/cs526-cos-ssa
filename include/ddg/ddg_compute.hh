#ifndef _DDG_COMPUTE_HH_
#define _DDG_COMPUTE_HH_

#include "ddg_types.hh"

#include <map>
#include <set>

void ddg_construct();
std::map<QDef, int> ddg_propagate_constants();
void ddg_reduce();
std::set<QDef> ddg_detect_dead_qdefs();

#endif
