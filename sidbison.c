/* Senior Thesis
 * iBison: Siddhartha Prasad, January 2016. 
 * 
 *  Compile With: gcc -g sidbison.c -lnsl -o sidbison
 *
 *  Run with bison specification and lexer shared object as input
 *
 *
 *  TODO:
 *      - ibison option things should be recorded too or removed (maybe only as a backdoor)
 * 
*/
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 



/** Interprocess communication */

FILE *child_in;     /* Communication with ibison */
FILE* intermediate; /* Necessary for interprocess communication */
int inter_fd;       /* intermediate file descriptor */
int int_pid;        /* iBison process id */

/*******************************/

/** Internal bookkeeping **/
char *test_file;        /* Path to input file being debugged */
short isbison;          /* 1 if sending input directly to ibison */
char *bspec;            /* Path to bison spec*/
char *lexobj;           /* Path to lexer object */

char *flags = "-i";     /* Flags sent to iBison*/
char *bison = "ibison"; /* Path to iBison */
int in_crule;           /* 1 if the crule command is being executed */
int num_next;           /* Holds the number of steps taken in iBison */
short eof;              /* Holds 1 if eof OR end token are encountered */
/*************************/

/***** Current iBison information */
int parser_state;   /* Current parser state number */
char *c_token;      /*current token encountered*/
char *tkn_stk;      /* Token stack */
char *state_stk;    /*State stk*/
unsigned short red; /* Holds true if a reduce action has just taken place */
char *last_reduced; /* Holds the last reduced rule in the specification */
char *rule_pos;
/*********************************/

char *str();
char *step();
char *steprule();
char* read_from_ibison();
char* execute_command(char* command);


void quit() {

    printf("Quitting\n");
    kill(int_pid, 9);
    system("rm intermediate");
    exit(0);

}

void finished_parsing() {

    if(in_crule) {
        if(last_reduced){
            free(last_reduced);
        }
        last_reduced = malloc(128);
        sprintf(last_reduced, "Hit EOF before crule could be identified.\n");
 
    } else {
        printf("Finished parsing, string is accepted\n");
        quit();
    }

}

void next() {

    fwrite("next\n", strlen("next\n"), 1, child_in);
    fflush(child_in);
    if(!in_crule) {
        num_next++;
    }
}

char *step() {

    
    char * oldtoken = malloc(256);
    
    if(c_token)
        strcpy(oldtoken, c_token);
    else
        oldtoken[0] = '\0';

    next();
    read_from_ibison();

    /*Three possible cases are reduce, encountering a new token, or a shift*/
    if(red) {
        /*Reduce*/
        red = 0;
        sprintf(oldtoken, "A new rule was parsed: %s\n",last_reduced);
        system("truncate -s 0 intermediate");
    } else if (strcmp(oldtoken, c_token) != 0) {
        /* Encountered a new token*/
        sprintf(oldtoken, "A new token was encountered: %s\n", c_token);
    } else {
        /* Shift */
        sprintf(oldtoken, "The current token was added to the parsed string\n");
    }
    /* Reusing the oldtoken variable here */
    return oldtoken;
}


char *crule() {

    system("truncate -s 0 intermediate");
    int pid;
    FILE *tmp = child_in;
    int fd_[2]; 
    pipe(fd_);

    char *old_current_token = NULL;
    if (pid = fork()) {
       

        if(c_token) {
            old_current_token = malloc(256);
            strcpy(old_current_token, c_token);
        }

        child_in = fdopen(fd_[1], "w");
        close(fd_[0]);
        
        size_t n = 400;
        char *command = malloc(512);
        
        in_crule = 1;

        int i;
        
        char *startupcommands = malloc(1024);
        sprintf(startupcommands, "lexer %s\n%s", lexobj, test_file);
        fwrite( startupcommands, strlen(startupcommands),1, child_in);

        fflush(child_in);

        for(i = 0; i < num_next; i++) {
            next();
            read_from_ibison();
        }
      
        int old_len = strlen(state_stk);
        int old_ps = parser_state;        
        short len_changed;
        short len_equal_and_state_changed;
        
        do {
            steprule();
            len_changed = strlen(state_stk) < old_len;
            len_equal_and_state_changed = (strlen(state_stk) == old_len)
                                        && (old_ps != parser_state);
        } while(!len_changed && !len_equal_and_state_changed && !eof);
        /* Better readability w/o DeMorgan simplification */
        eof = 0; /* Reset eof if encountered */

    } else {
 
        close(2);
        dup(inter_fd);
        close(1);
        dup(inter_fd);
        close(0);
        dup(fd_[0]);
        close(inter_fd);
        close(fd_[1]);
        close(fd_[0]);
        execl(bison, bison, flags,  bspec, NULL);       
    }
    in_crule = 0;

    if(c_token)
        free(c_token);
    c_token = old_current_token;

    child_in = tmp;
    kill(pid,9);
    char *to_return = malloc(512);
    sprintf(to_return, "crule: %s", last_reduced);
    system("truncate -s 0 intermediate");
    str(); /* Restore state*/
    return to_return;
}

char *steprule() {

    system("truncate -s 0 intermediate");
    red = 0;
    while(!red && !eof) {
        next();
        read_from_ibison();
    }

    red = 0;
    char *msg = malloc(32);
    strcpy(msg, "Stepped to next rule\n");
    return msg;
}


char *ctkn() {

    char *to_return = malloc(128);
    sprintf(to_return,"ctkn:%s\n", c_token);
    return to_return;

}

char *str() {
    fwrite("stack\n", strlen("stack\n"), 1, child_in);
    fflush(child_in);
    read_from_ibison();
    char *to_return = malloc(512);
    sprintf(to_return, "str: %s\n",tkn_stk); 
    return to_return;
}

char *rulepos(){

 /* Not implemented yet
 *
 * Find current state
 * Find crule
 * Look for instances of crule in the current state
 *
 * */
    
    char *state = malloc(32);
    if(parser_state < 0) {
        strcpy(state, "No rule being parsed yet\n");
        return state;    
    }
    sprintf(state, "state %d\n", parser_state);
    
    fwrite(state, strlen(state), 1, child_in);
    fflush(child_in);
    read_from_ibison();
    system("truncate -s 0 intermediate");
    char* curr_rule = crule();
    char *to_return = malloc(1024);
    sprintf(to_return, "%sWhere %s", rule_pos, curr_rule);
    return to_return;
}

char *br() {

    char token [256];
    char final [300];
    printf("Character: ");
    fflush(stdout);
    fgets(token, sizeof(token), stdin);
    

    while(strcmp(token, c_token) != 0) {

        next();
        read_from_ibison();
    }
    char *to_return = malloc(128);
    sprintf(to_return, "br: Broken at %s\n", c_token);
    return to_return;
}


/* This is a parsing function */
char *read_from_ibison() 
{
    FILE *inter_in;
    int fd[2];
    int temp_pid;

    pipe(fd);
    
    if(temp_pid = fork()) {
        char *out = NULL;
        char temp [512];
        size_t n = 0;
        char *state_response = malloc(32);
        sprintf(state_response, "state %d:\n", parser_state);
        int i;
        char tkn_name[128];
        inter_in = fdopen(fd[0], "r");
        close(fd[1]);

        while(getline(&out, &n, inter_in) != -1) {
            if(isbison) {
                printf("(ibison) %s\n",out);
            }
            if(strcmp(out, state_response) == 0) { /*TODO*/
               free(out);

               if(!rule_pos) {
                    free(rule_pos);
                } 
               rule_pos = malloc(1024);

               out = NULL;
               while(getline(&out, &n, inter_in) == 1){
                    free(out);
                    out = NULL;
               }
                strcat(rule_pos,"Possible rule positions are:\n");
               do {
                     strcat(rule_pos, out); //Not ireading anything here
                     free(out);
                     out = NULL;
               } while(getline(&out, &n, inter_in) >= 1);

            } else if(sscanf(out,
              "Reading a token...next token is: %s %s\n",temp, tkn_name) == 2) {
            
               if(c_token)
                    free(c_token);
               
                if(strstr(tkn_name, "end")) {
                    eof = 1;    
                    finished_parsing();

                }
               c_token = malloc(512);
               strcpy(c_token, temp);

            } else if(sscanf(out, "Reduced %s\n",temp) == 1) {
               red = 1;
               free(out);
               out = NULL;
               getline(&out, &n, inter_in);
               if(last_reduced)
                    free(last_reduced);
               last_reduced = malloc(512);
               strcpy(last_reduced, out);
            
            } else if(!in_crule && 
              sscanf(out, "Token is shifted. Entering state %d\n",
              &parser_state) == 1) {

            } else if (strcmp(out, "Stacks:(states, tokens)\n") == 0) {
              free(out);
              out = NULL;
              getline(&out, &n, inter_in);
              
              if(state_stk)
                    free(state_stk);
              state_stk = malloc(512);
              strcpy(state_stk, out);

              char *last_space = NULL;
              int len = strlen(out);
              for(i = 0; i < len-1; i++) {
                if((out[i] == ' ') && (out[i+1] != '\n')){
                    last_space = out + i; /* Nasty pointer arithmetic */
                 }
              }

              sscanf(last_space, " %d\n", &parser_state);
              free(out);
              out = NULL;
              getline(&out, &n, inter_in);

              if(tkn_stk)
                free(tkn_stk);
              tkn_stk = malloc(512);
              if(strlen(out) > 1) {
                strcpy(tkn_stk, out);

             
            } else {
                strcpy(tkn_stk, "ERR: Nothing read yet\n");
            }

              free(out);
              out = NULL;
            }
        }
        free(state_response);
        return out; 

      } else {
            close(1);
            dup(fd[1]);
            close(fd[1]);
            close(fd[0]);
            execl("/bin/cat", "/bin/cat",  "intermediate", NULL);
     }
}



int main(int argc, char *argv[])
{
    int fd_[2];
    parser_state = -1;
    c_token = NULL;
    tkn_stk = NULL;
    last_reduced = NULL;
    rule_pos = NULL;
    state_stk = NULL;

    pipe(fd_);
    in_crule = 0;
    if (argc < 3) {
        fprintf(stderr,"ERROR: Bison specification and lexer shared object must be provided\n");
        exit(1);
    }

    lexobj = malloc(512);
    bspec = malloc(512);
    strcpy(lexobj, argv[2]);
    strcpy(bspec, argv[1]);

    intermediate = fopen("intermediate", "a+");
    setvbuf(intermediate, NULL, _IONBF, BUFSIZ);
    inter_fd = fileno(intermediate); 
    
    if (int_pid = fork()) {
        
        char* lexer = malloc(512* sizeof(*lexer));

        sprintf(lexer,"lexer %s\n", lexobj);
        child_in = fdopen(fd_[1], "w");
        char *command = NULL;
        char *response = NULL;
        size_t n = 0;


        fwrite(lexer, strlen(lexer), 1, child_in);
        fflush(child_in);

        printf("Stepwise Interactive Debugger Bison 1.0\n");
        printf("Please report bugs to Siddhartha.Prasad@tufts.edu\n");
        while(1) {
                
            printf("(sidbison)");
            fflush(stdout);
            getline(&command, &n, stdin);

            if(strcmp(command, "quit\n") == 0) {
               quit(); 
            }
 
            response = execute_command(command);
            if(response) {
                fprintf(stdout, response);
                free(response);
            }   
            if(command)
                free(command);
            command = NULL;
        }
        if(lexer)
            free(lexer);
        return 0; 
    } else {
        close(2);
        dup(inter_fd);
        close(1);
        dup(inter_fd);
        close(0);
        dup(fd_[0]);
        close(inter_fd);
        close(fd_[1]);
        close(fd_[0]);
        execl(bison, bison, flags,  bspec, NULL);
    }
}

char* execute_command(char* command) {

    char* final = NULL;
    char temp[256];
    size_t n = 0;   
    if (strcmp(command, "crule\n") == 0) {
        
        system("truncate -s 0 intermediate");
        final = crule();
        system("truncate -s 0 intermediate");
    
    } else if (strcmp(command, "ctkn\n") == 0) {
        
        final = ctkn();

    } else if (strcmp(command, "step\n") == 0) {
        
        final = step();

    } else if (strcmp(command, "rulepos\n") == 0) { 

        system("truncate -s 0 intermediate");
        final = rulepos();
    
    } else if (strcmp(command, "str\n") == 0) {
    
        final = str();

    } else if (strcmp(command, "steprule\n") == 0) {
    
        system("truncate -s 0 intermediate");
        final = steprule();
        system("truncate -s 0 intermediate"); /*Must truncate after a reduce */

    } else if (strcmp(command, "br\n") == 0) {
    
        final = br();

    } else if (sscanf(command, "test %s", temp) == 1) {

        test_file = malloc(512);

        system("truncate -s 0 intermediate");
        strcpy(test_file, command);
        fwrite(test_file, strlen(test_file), 1, child_in);
        fflush(child_in);
        isbison = 1;
        read_from_ibison(); 
        isbison = 0; 

        final = malloc(512);
        sprintf(final, "\n");
    
    } else if (strcmp(command, "ibison\n") == 0) {


        printf("(ibison)");
        fflush(stdout);
        isbison = 1;
        getline(&final, &n, stdin);
        fwrite(final, strlen(final), 1, child_in);
        fflush(child_in);

        if(final)
            free(final);
        final = malloc(512);
        read_from_ibison(); 
        strcpy(final,"\n");
        isbison = 0;
    }  else {
        final = malloc(512);
        sprintf(final,"Unrecognized command '%s'\n", command);
    }

    return final;
}



