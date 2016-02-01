#ifndef SIDBISON_H
#define SIDBISON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>


char *str();                            /* Implements the str command */

char *step();                           /* Implements the step command */

char *steprule();                       /* Implements the steprule command */

char *read_from_ibison();               /*Examines iBison output and updates state accordingly*/

char *execute_command(char *command);   /*Executes a given command */

void quit();                            /*Executes the quit command*/

void finished_parsing();                /*Called when the entire input has been parsed*/

void next();                            /*Send a next comand to ibison */

char *crule();                          /* Implements the crule command */


#endif
