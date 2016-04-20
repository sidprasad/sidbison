%{                                            
# include "cparser.tab.h"
# undef yywrap
# define yywrap() 1
%}

%option noyywrap

int   [0-9]+
blank [ \t]

%%
{blank}+
[\n]+      
\+	   return OPERATOR_PLUS;
-	   return OPERATOR_MINUS;
\*	   return OPERATOR_MULT;
\/	   return OPERATOR_DIV;
\(	   return LPAREN;
\)	   return RPAREN;
{int}      return NUMBER;
.          {fprintf(stderr,"Invalid token!\n");}
%%
