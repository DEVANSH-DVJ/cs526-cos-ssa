#ifndef _ARGPARSE_HH_
#define _ARGPARSE_HH_

#include <argp.h>
#include <cstring>
#include <string>

#include <iostream>

using namespace std;

enum short_options {
  OPT_TYPE = 0x80,
};

enum tool_name { TOOL_CFG, TOOL_SSA, TOOL_UNKNOWN };

struct arguments {
  tool_name tool;
  string input_file;
};

/* Program documentation */
char doc[] = "CS-SSA";

/* Argument description */
char args_doc[] = "[FILE]";

/* Options visible to user */
struct argp_option options[] = {{"tool", OPT_TYPE, "TYPE", 0, "Tool Name"},
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
    } else {
      cout << "Unknown tool name: " << arg << endl;
      argp_usage(state);
    }
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
