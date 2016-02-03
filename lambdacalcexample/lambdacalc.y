%{
/* Untyped lambda calculus */
void yyerror(const char *s);

%}

%token  IDENT
%token  CONSTANT
%token  LPAREN
%token  RPAREN
%token  LAMBDA
%token  DOT

%%

exp : var
    | func
    | app
    | CONSTANT

func : LAMBDA var DOT scope

app : LPAREN funcexp argexp  RPAREN

scope : exp

funcexp : exp

argexp : exp

var : IDENT


;

%%
void
yyerror(const char *s)
{
    fprintf(stderr, "Syntax error: %s\n", s);
}

