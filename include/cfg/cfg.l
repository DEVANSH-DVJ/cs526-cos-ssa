%{

    #include "headers.hh"

    #include <string>

    #include <list>

    using namespace std;

    #include "cfg.tab.hh"

%}

%option noyywrap
%option noinput
%option nounput
%option prefix="cfg_"

digit [0-9]
letter [a-zA-Z]
alphanum [0-9a-zA-Z]
operator [-+*/]
ignore [ \t\n]

%%

  /* Ignore characters */

{ignore}* ;
"//".* ;

  /* Delimiters */

";" { return CFG_EOS; }
":" { return CFG_COLON; }
"->" { return CFG_ARROW; }
"," { return CFG_COMMA; }
"{" { return CFG_LCB; }
"}" { return CFG_RCB; }

  /* Keywords */

"START" { return CFG_START; }
"END" { return CFG_END; }
"CALL" { return CFG_CALL; }
"INPUT" { return CFG_INPUT; }
"USEVAR" { return CFG_USEVAR; }
"EMPTY" { return CFG_EMPTY_NODE; }

  /* Identifiers */

"=" { return CFG_ASSIGN; }
{operator} { cfg_lval.name = new string(cfg_text); return CFG_OP; }

  /* Constants */

("-")?{digit}+ { cfg_lval.value = atoi(cfg_text); return CFG_NUM; }

  /* Identifiers */

(("$")*("_")*(".")*{alphanum}("$")*("_")*(".")*)+ { cfg_lval.name = new string(cfg_text); return CFG_ID; }

  /* Rest */

. { return CFG_UNKNOWN; }
