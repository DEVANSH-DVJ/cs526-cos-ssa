%{

    #include <list>
    #include <string>

    #include "../headers.hh"

    using namespace std;

    #include "cfg.tab.hh"

%}

%option noyywrap
%option nounput
%option prefix="cfg_"

digit [0-9]
letter [a-zA-Z]
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

  /* Identifiers */

"=" { return CFG_ASSIGN; }
{operator} { cfg_lval.name = new string(cfg_text); return CFG_OP; }

  /* Identifiers */

{letter}+ { cfg_lval.name = new string(cfg_text); return CFG_ID; }

  /* Constants */

{digit}+ { cfg_lval.value = atoi(cfg_text); return CFG_NUM; }

  /* Rest */

. { return CFG_UNKNOWN; }
