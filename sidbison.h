#ifndef SIDBISON_H
#define SIDBISON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>


char *str();

char *step();

char *steprule();

char *read_from_ibison();

char *execute_command(char *command);

void quit();

void finished_parsing();

void next();

char *crule();


#endif
