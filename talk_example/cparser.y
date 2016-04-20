%{
/* A simple calculator */
void yyerror(const char *s);
%}
%token        NUMBER
%token 	      OPERATOR_PLUS
%token	      OPERATOR_MINUS
%token	      OPERATOR_MULT
%token 	      OPERATOR_DIV
%token	      LPAREN
%token 	      RPAREN
%left OPERATOR_PLUS OPERATOR_MINUS
%left OPERATOR_MULT OPERATOR_DIV
%%
exp: exp OPERATOR_PLUS exp
   | exp OPERATOR_MINUS exp
   | exp OPERATOR_MULT exp 
   | exp OPERATOR_DIV exp
   | LPAREN NUMBER RPAREN
   | NUMBER
;
%%
void
yyerror(cosnt char *s)
{
  fprintf(stderr,"Syntax error: %s\n",s);
}
