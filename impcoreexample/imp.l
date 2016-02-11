%{
# include "imp.tab.h"
# undef yywrap
# define yywrap() 1

/* Need to include - in word */

%}

%option noyywrap

blank [ \t]
word [a-zA-Z][a-zA-Z0-9]*
int [0-9]+


%%
{blank}+
[\n]+
\(              return LPAREN;
\)              return RPAREN;
\+              return PLUS;
-               return MINUS;
\*              return MUL;
\/              return DIV;
"="             return EQ;
\<              return LT;
\>              return GT;
"val"           return VAL;
"define"        return DEFINE;
"use"           return USE;
"check-expect"  return CHECKEXPECT;
"set"           return SET;
"if"            return IF;
"while"         return WHILE;
"begin"         return BEGN;
"print"         return PRINT;
"check-err"     return CHECKERR;
{int}           {yylval = malloc(128); strcpy(yylval, yytext); return NUMERAL;}
{word}          {yylval = malloc(128); strcpy(yylval, yytext); return NAME;}
.               {fprintf(stderr, "Invalid token!\n");}

%%




