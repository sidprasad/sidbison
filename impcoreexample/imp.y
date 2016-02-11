%{
/* Leaving unit-test out of def */

void yyerror(const char *s);

%}


%token  VAL
%token  DEFINE
%token  LPAREN
%token  RPAREN
%token  USE
%token  CHECKEXPECT
%token  SET
%token  IF
%token  WHILE
%token  BEGN
%token  PLUS
%token  MINUS
%token  MUL
%token  DIV
%token  EQ
%token  LT
%token  GT
%token  PRINT
%token  CHECKERR
%token  NUMERAL
%token  NAME
%%

def : LPAREN VAL variablename exp RPAREN            {$$ = malloc(8192); 
                                                     sprintf($$,"(val %s %s)\n", $3, $4);}
    | exp                                           {$$ = malloc(8192); 
                                                     sprintf($$,"%s\n", $1);}

    | LPAREN DEFINE functionname formals exp RPAREN {$$ = malloc(8192); 
                                                     sprintf($$,"(define %s %s %s)\n", $3, $4, $5);}

    | LPAREN USE filename RPAREN                    {$$ = malloc(8192); 
                                                     sprintf($$,"(use %s)\n", $3);}

    | unittest                                      {$$ = malloc(8192); 
                                                     sprintf($$,"%s\n", $1);}


unittest : LPAREN CHECKEXPECT exp exp RPAREN        {$$ = malloc(8192); 
                                                     sprintf($$,"(check-expect %s %s)", $3, $4);}
          | LPAREN CHECKERR exp RPAREN              {$$ = malloc(8192); 
                                                     sprintf($$,"(check-err %s)", $3);}

exp : literal                                       {$$ = malloc(256);strcpy($$,$1);}
    | variablename                                  {$$ = malloc(256);strcpy($$,$1);}
    | LPAREN SET variablename exp RPAREN            {$$ = malloc(8192); 
                                                     sprintf($$,"(set %s %s)", $3, $4);}

    | LPAREN IF exp exp exp RPAREN                  {$$ = malloc(8192); 
                                                     sprintf($$,"(if %s %s %s)", $3, $4, $5);}
    | LPAREN WHILE exp exp RPAREN                   {$$ = malloc(8192); 
                                                     sprintf($$,"(while %s %s)", $3, $4);}
    | LPAREN BEGN expstar                           {$$ = malloc(8192); 
                                                     sprintf($$,"(being %s", $3);}
    | LPAREN function expstar                        {$$ = malloc(8192); 
                                                     sprintf($$,"( %s %s", $3, $4);}


expstar : RPAREN                                    {$$ = malloc(4); 
                                                     sprintf($$,"(");}
        | exp expstar                               {$$ = malloc(8192); 
                                                     sprintf($$,"%s %s", $1, $2);}

formals : LPAREN variablenamestar                   {$$ = malloc(1024); 
                                                     sprintf($$,"(%s", $2);}

variablenamestar : RPAREN                           {$$ = malloc(4); 
                                                     sprintf($$,"(");}

                 | variablename variablenamestar    {$$ = malloc(8192); 
                                                     sprintf($$,"%s %s", $1, $2);}


literal : NUMERAL                                   {$$ = malloc(64); sprintf($$,$1);}


function : functionname                             {$$ = malloc(64); 
                                                     sprintf($$,"%s", $1);}

         | primitive                                {$$ = malloc(8); 
                                                     sprintf($$,"%s", $1);}


primitive : PLUS                                    {$$ = malloc(8);
                                                     sprintf($$, "+");}
          | MINUS                                   {$$ = malloc(8);
                                                     sprintf($$, "-");}
          | MUL                                     {$$ = malloc(8);
                                                     sprintf($$, "*");}
          | DIV                                     {$$ = malloc(8);
                                                     sprintf($$, "/");}
          | EQ                                      {$$ = malloc(8);
                                                     sprintf($$, "=");}
          | LT                                      {$$ = malloc(8);
                                                     sprintf($$, "<");}
          | GT                                      {$$ = malloc(8);
                                                     sprintf($$, ">");}
          | PRINT                                   {$$ = malloc(8);
                                                     sprintf($$, "print");}

variablename : NAME                                 {$$ = malloc(64); sprintf($$,$1);}
functionname : NAME                                 {$$ = malloc(64); sprintf($$,$1);}
filename : NAME                                     {$$ = malloc(64); sprintf($$,$1);}


;

%%
void
yyerror(const char *s)
{
    fprintf(stderr, "Syntax error: %s\n", s);
}

