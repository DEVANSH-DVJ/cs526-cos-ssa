#include "../headers.hh"

using namespace std;

CFG_Opd::CFG_Opd(CFG_OpdType type) {
  if (type != CFG_InputOpd && type != CFG_UsevarOpd) {
    CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH,
                    "CFG_InputOpd or CFG_UsevarOpd expected");
  }

  this->type = type;

  this->num_value = 0;
  this->var_name = "";
}

CFG_Opd::CFG_Opd(CFG_OpdType type, int num_value) {
  CHECK_INVARIANT(type == CFG_NumOpd, "CFG_NumOpd expected");

  this->type = type;

  this->num_value = num_value;
  this->var_name = "";
}

CFG_Opd::CFG_Opd(CFG_OpdType type, string var_name) {
  CHECK_INVARIANT(type == CFG_VarOpd, "CFG_VarOpd expected");

  this->type = type;

  this->num_value = 0;
  this->var_name = var_name;
}

CFG_Opd::~CFG_Opd() {}

CFG_OpdType CFG_Opd::get_type() { return this->type; }

int CFG_Opd::get_opd_value() {
  CHECK_INVARIANT(this->type == CFG_NumOpd, "CFG_NumOpd expected");

  return this->num_value;
}

string &CFG_Opd::get_opd_var() {
  CHECK_INVARIANT(this->type == CFG_VarOpd, "CFG_VarOpd expected");

  return this->var_name;
}

string CFG_Opd::str() {
  switch (type) {
  case (CFG_NumOpd):
    return to_string(this->num_value);
  case (CFG_VarOpd):
    return this->var_name;
  case (CFG_InputOpd):
    return "INPUT";
  case (CFG_UsevarOpd):
    return "USEVAR";
  }

  CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "CFG_OpdType unknown");

  return "";
}
