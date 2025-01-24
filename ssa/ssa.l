%{

    #include <list>
    #include <string>

    #include "../headers.hh"

    using namespace std;

    #include "ssa.tab.hh"

%}

%option noyywrap
%option nounput
%option prefix="ssa_"

digit [0-9]
letter [a-zA-Z]
operator [-+*/]
ignore [ \t\n]

%%

  /* Ignore characters */

{ignore}* ;
"//".* ;

  /* Delimiters */

";" { return SSA_EOS; }
":" { return SSA_COLON; }
"->" { return SSA_ARROW; }
"," { return SSA_COMMA; }
"{" { return SSA_LCB; }
"}" { return SSA_RCB; }
"(" { return SSA_LRB; }
")" { return SSA_RRB; }
"_" { return SSA_UNDERSCORE; }

  /* Keywords */

"START" { return SSA_START; }
"END" { return SSA_END; }
"CALL" { return SSA_CALL; }
"INPUT" { return SSA_INPUT; }
"USEVAR" { return SSA_USEVAR; }
"PHI" { return SSA_PHI; }

  /* Identifiers */

"=" { return SSA_ASSIGN; }
{operator} { ssa_lval.name = new string(ssa_text); return SSA_OP; }

  /* Identifiers */

{letter}+ { ssa_lval.name = new string(ssa_text); return SSA_ID; }

  /* Constants */

{digit}+ { ssa_lval.value = atoi(ssa_text); return SSA_NUM; }

  /* Rest */

. { return SSA_UNKNOWN; }
