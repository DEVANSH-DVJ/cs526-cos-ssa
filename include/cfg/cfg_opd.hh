#ifndef _CFG_OPD_HH_
#define _CFG_OPD_HH_

#include <string>

class CFG_Opd;

typedef enum {
  CFG_NumOpd,
  CFG_VarOpd,
  CFG_InputOpd,
  CFG_UsevarOpd,
} CFG_OpdType;

class CFG_Opd {
  /* Always initialized: by constructor */
  CFG_OpdType type;

  /* Conditionally initialized: post parsing */
  int num_value;
  std::string var_name;

public:
  /* Constructors and Destructor */
  CFG_Opd(CFG_OpdType type);
  CFG_Opd(CFG_OpdType type, int num_value);
  CFG_Opd(CFG_OpdType type, std::string var_name);
  ~CFG_Opd();

  /* Get functions */
  // Get the node type
  CFG_OpdType get_type();
  // Get opd value if it is a number
  int get_opd_value();
  // Get opd value if it is a variable
  std::string &get_opd_var();

  /* Helper functions */
  // Return the string rep
  std::string str();
};

#endif
