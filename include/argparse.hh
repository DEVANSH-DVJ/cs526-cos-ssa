#ifndef _ARGPARSE_HH_
#define _ARGPARSE_HH_

#include <argp.h>
#include <cstring>
#include <string>

#include <iostream>

using namespace std;

enum short_options {
  OPT_TYPE = 0x80,
  SINGLE_PARTITION_FLAG = 0x81,
  NO_OPT_FLAG = 0x82,
};

enum tool_name { TOOL_CFG, TOOL_SSA, TOOL_DDG, TOOL_CFG_TO_SSA, TOOL_LLVM, TOOL_ALL, TOOL_UNKNOWN };

struct arguments {
  tool_name tool;
  string input_file;
  bool single_partition;
  bool no_opt;
};

/* Program documentation */
char doc[] = "CS-SSA";

/* Argument description */
char args_doc[] = "[FILE]";

/* Options visible to user */
struct argp_option options[] = {{"tool", OPT_TYPE, "TYPE", 0, "Tool Name"},
                                {"single-partition", SINGLE_PARTITION_FLAG, NULL, 0, "Use a single partition of globals"},
                                {"no-opt", NO_OPT_FLAG, NULL, 0, "Skip optimizations to the DDG"},
                                {0}};

error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = (struct arguments *)state->input;

  switch (key) {
  case OPT_TYPE:
    if (arg == NULL) {
      cout << "Need to specify tool name" << endl;
      argp_usage(state);
    } else if (strcasecmp(arg, "cfg") == 0) {
      arguments->tool = TOOL_CFG;
    } else if (strcasecmp(arg, "ssa") == 0) {
      arguments->tool = TOOL_SSA;
    } else if (strcasecmp(arg, "ddg") == 0) {
      arguments->tool = TOOL_DDG;
    } else if (strcasecmp(arg, "cfg-to-ssa") == 0) {
      arguments->tool = TOOL_CFG_TO_SSA;
    } else if (strcasecmp(arg, "llvm") == 0) {
      arguments->tool = TOOL_LLVM;
    } else if (strcasecmp(arg, "all") == 0) {
      arguments->tool = TOOL_ALL;
    } else {
      cout << "Unknown tool name: " << arg << endl;
      argp_usage(state);
    }
    break;

  case SINGLE_PARTITION_FLAG:
    arguments->single_partition = true;
    break;

  case NO_OPT_FLAG:
    arguments->no_opt = true;
    break;

  case ARGP_KEY_ARG:
    if (state->arg_num > 0)
      argp_usage(state); // too many args
    arguments->input_file = arg;
    break;

  case ARGP_KEY_NO_ARGS:
    argp_usage(state); // no args
    break;

  case ARGP_KEY_END:
    if (arguments->tool == TOOL_UNKNOWN) {
      cout << "Need to specify tool name" << endl;
      argp_usage(state);
    }
    break;

  default:
    return ARGP_ERR_UNKNOWN;
  }

  return 0;
}

struct argp argp = {options, parse_opt, args_doc, doc};

#endif
