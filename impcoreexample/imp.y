%{
/* Leaving unit-test out of def */
#include "heading.h"
#include <stdio.h>
void yyerror(const char *s);
int yylex(void);
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


program : 
        | def program

def : LPAREN VAL variablename exp RPAREN                                         | exp     
    | LPAREN DEFINE functionname formals exp RPAREN
    | LPAREN USE filename RPAREN                     
    | unittest  


unittest : LPAREN CHECKEXPECT exp exp RPAREN     
          | LPAREN CHECKERR exp RPAREN    

exp : literal                             
    | variablename                        
    | LPAREN SET variablename exp RPAREN  
    | LPAREN IF exp exp exp RPAREN          
    | LPAREN WHILE exp exp RPAREN         
    | LPAREN BEGN expstar                           
    | LPAREN function expstar                       



expstar : RPAREN                                    
        | exp expstar                                
                                                     

formals : LPAREN variablenamestar                  

variablenamestar : RPAREN                           
                  | variablename variablenamestar 


literal : NUMERAL           

function : functionname           
         | primitive              

primitive : PLUS                  
          | MINUS                                 
          | MUL                    
          | DIV                    
          | EQ                     
          | LT                     
          | GT                     
          | PRINT                  

variablename : NAME 
functionname : NAME 
filename : NAME  


;

%%
void
yyerror(const char *s)
{
    fprintf(stderr, "Syntax error: %s\n", s);
}

