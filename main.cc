#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdlib.h>

#include "argparse.hh"

#include <list>
#include <string>

#include "headers.hh"

using namespace std;

Program *program;

fstream *dot_fd;

int main(int argc, char **argv) {

  // Default arguments
  struct arguments arguments;

  arguments.tool = TOOL_UNKNOWN;
  arguments.input_file = "";

  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  string input_file = arguments.input_file;
  switch (arguments.tool) {
  case TOOL_CFG:
    CHECK_INPUT_AND_ABORT(input_file.substr(input_file.length() - 4) == ".cfg",
                          "Input file must be a .cfg file");
    program = new Program("cfg", input_file.substr(0, input_file.length() - 4));
    program->run();
    break;
  case TOOL_SSA:
    CHECK_INPUT_AND_ABORT(input_file.substr(input_file.length() - 4) == ".ssa",
                          "Input file must be a .ssa file");
    program = new Program("ssa", input_file.substr(0, input_file.length() - 4));
    program->run();
    break;
  default:
    CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Unknown tool");
  }

  exit(0);
}
