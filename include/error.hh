#ifndef _ERROR_HH_
#define _ERROR_HH_

#include <fstream>
#include <iostream>
#include <string>

using namespace std;

#define CONTROL_SHOULD_NOT_REACH false

void check_invariant_underlying_function(bool condition, string error_message,
                                         int line, string file);
void report_violation_and_abort(bool condition, string s);

#define CHECK_INVARIANT(x, error_message)                                      \
  check_invariant_underlying_function(x, error_message, __LINE__, __FILE__);

#define CHECK_INPUT_AND_ABORT(condition, error_message)                        \
  report_violation_and_abort(condition, error_message);

#endif
