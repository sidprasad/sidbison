%{
# include "pcf.tab.h"
# undef yywrap
# define yywrap() 1

%}

%option noyywrap

blank [ \t]
word [a-zA-Z][a-zA-Z0-9]*


%%
{blank}+
[\n]+
0           return ZERO;
:           return COLON;
"true"      return TRUE;
"false"     return FALSE;
\.          return DOT;
\(          return LPAREN;
\)          return RPAREN;
"fix"       return FIX;
"zero"      return ZEROFUNC;
"succ"      return SUCC;
"pred"      return PRED;
"if"        return IF;
"then"      return THEN;
"else"      return ELSE;
"fn"        return FN;
"nat"       return NAT;
"bool"      return BOOL;
"->"        return ARROW;
{word}      return IDEN;
.           {fprintf(stderr, "Invalid token!\n");}

%%




