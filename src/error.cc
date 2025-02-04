#include "headers.hh"
#include "stacktrace.h"

extern Program *program;

void check_invariant_underlying_function(bool condition, string error_message,
                                         int line, string file) {
  if (!condition) {
    cerr << "\nInternal error: " << error_message << " (Invariant at line "
         << line << ", file " << file << ").\n";
    print_stacktrace();
    program->cleanup();
  }
}

void report_violation_and_abort(bool condition, string error_message) {
  if (!condition) {
    cerr << "Semantic Error: " << error_message << "\n";
    program->cleanup();
  }
}
