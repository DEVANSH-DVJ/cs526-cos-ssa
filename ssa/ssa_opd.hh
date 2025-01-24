#ifndef _SSA_OPD_HH_
#define _SSA_OPD_HH_

#include <string>

#include <utility>

class SSA_Opd;

typedef enum {
  SSA_NumOpd,
  SSA_VarOpd,
  SSA_PhiOpd,
  SSA_InputOpd,
  SSA_UsevarOpd,
} SSA_OpdType;

class SSA_Opd {
  /* Always initialized: by constructor */
  SSA_OpdType type;
  std::pair<int, int> meta_num;

  /* Conditionally initialized: post parsing */
  int num_value;
  std::string var_name;

public:
  /* Constructors and Destructor */
  SSA_Opd(SSA_OpdType type, std::pair<int, int> meta_num);
  SSA_Opd(SSA_OpdType type, int num_value);
  SSA_Opd(SSA_OpdType type, std::pair<int, int> meta_num, std::string var_name);
  ~SSA_Opd();

  /* Get functions */
  // Get the node type
  SSA_OpdType get_type();
  // Get the node id
  std::pair<int, int> get_meta_num();
  // Get opd value if it is a number
  int get_opd_value();
  // Get opd value if it is a variable
  std::string &get_opd_var();

  /* Helper functions */
  // Return the string rep
  std::string str();
};

#endif
