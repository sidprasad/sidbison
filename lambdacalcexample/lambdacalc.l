%{
# include "lambdacalc.tab.h"
# undef yywrap
# define yywrap() 1

%}

%option noyywrap

blank [ \t]
word [a-zA-Z][a-zA-Z0-9]*
int [0-9]+


%%
{blank}+
[\n]+
{int}       return CONSTANT;
{word}      return IDENT;
\.          return DOT;
\\          return LAMBDA;
\(          return LPAREN;
\)          return RPAREN;
.           {fprintf(stderr, "Invalid token!\n");}

%%




