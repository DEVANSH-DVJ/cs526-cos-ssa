#include "../headers.hh"

using namespace std;

SSA_Opd::SSA_Opd(SSA_OpdType type, pair<int, int> meta_num) {
  if (type != SSA_InputOpd && type != SSA_UsevarOpd) {
    CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH,
                    "SSA_InputOpd or SSA_UsevarOpd expected");
  }

  this->type = type;
  this->meta_num = meta_num;
  if (type == SSA_InputOpd) {
    this->meta_num.second = 0;
  }

  this->num_value = 0;
  this->var_name = "";
}

SSA_Opd::SSA_Opd(SSA_OpdType type, int num_value) {
  CHECK_INVARIANT(type == SSA_NumOpd, "SSA_NumOpd expected");

  this->type = type;
  this->meta_num = make_pair(0, 0);

  this->num_value = num_value;
  this->var_name = "";
}

SSA_Opd::SSA_Opd(SSA_OpdType type, pair<int, int> meta_num, string var_name) {
  if (type != SSA_VarOpd && type != SSA_PhiOpd) {
    CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH,
                    "SSA_VarOpd or SSA_PhiOpd expected");
  }

  this->type = type;
  this->meta_num = meta_num;

  this->num_value = 0;
  this->var_name = var_name;
}

SSA_Opd::~SSA_Opd() {}

SSA_OpdType SSA_Opd::get_type() { return this->type; }

pair<int, int> SSA_Opd::get_meta_num() { return this->meta_num; }

int SSA_Opd::get_opd_value() { return this->num_value; }

string &SSA_Opd::get_opd_var() { return this->var_name; }

string SSA_Opd::str() {
  switch (type) {
  case (SSA_NumOpd):
    return to_string(this->num_value);
  case (SSA_VarOpd):
    return var_name + "_" + to_string(this->meta_num.first) + "_" +
           to_string(this->meta_num.second);
  case (SSA_PhiOpd):
    return var_name + "_" + to_string(this->meta_num.first) + "_" +
           to_string(this->meta_num.second) + "_PHI";
  case (SSA_InputOpd):
    return "INPUT_" + to_string(this->meta_num.first);
  case (SSA_UsevarOpd):
    return "USEVAR_" + to_string(this->meta_num.first) + "_" +
           to_string(this->meta_num.second);
  }

  CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "SSA_OpdType unknown");

  return "";
}
