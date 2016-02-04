%{

/* A simple PCF grammar */

%}

%token ZERO
%token TRUE
%token FALSE
%token IDEN
%token FIX
%token ZEROFUNC
%token LPAREN
%token RPAREN
%token SUCC
%token PRED
%token IF
%token THEN
%token ELSE
%token FN
%token COLON
%token DOT
%token NAT
%token ARROW
%token BOOL

%left ARROW
 
%%


m : ZERO
  | TRUE
  | FALSE
  | var
  | zerofunc
  | succ
  | pred
  | ifelse
  | fun
  | app
  | fix

fix : FIX LPAREN m RPAREN

app : LPAREN callfunc argfunc RPAREN

callfunc : m
argfunc : m


tau : NAT
    | BOOL
    | funtau
    | LPAREN funtau  RPAREN

funtau : tau ARROW tau


fun : FN var COLON tau DOT m

succ : SUCC LPAREN m RPAREN

pred : PRED LPAREN m RPAREN

zerofunc : ZEROFUNC LPAREN m RPAREN

var : IDEN

ifelse : IF m THEN m ELSE m

%%
/*

app : funcexp argexp

succ : SUCC LPAREN m RPAREN
pred : PRED LPAREN m RPAREN
fix : FIX LPAREN m RPAREN

funcexp : m
argexp : m

fix : LPAREN m RPAREN

func : FN var COLON tau DOT m

*/
void yyerror (const char *s)
{
fprintf(stderr, "Syntax error: %s\n", s);
}



