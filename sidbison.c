/* 
 * iBison: Siddhartha Prasad, Feb 2016. 
 *
*/

#include "sidbison.h"

/**********/
const int version = 1;
/**********/


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

int count_ws (char *s); /* Counts the number of white spaces in a string */
/*********************************/

void quit() {

    printf("Quitting\n");
    kill(int_pid, 9);
    system("rm -r tmp/intermediate");
    exit(0);

}

void finished_parsing() {

    if(in_crule) {
        if(last_reduced){
            free(last_reduced);
        }
        last_reduced = malloc(128);
        sprintf(last_reduced, "Hit EOF before crule could be identified. Taking a step and calling crule again may help\n");
 
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
        system("truncate -s 0 tmp/intermediate");
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


    if( !state_stk) {

        char *to_return = malloc(512);
        sprintf(to_return, "crule: %s", "Parsing has not begun yet, please take a step\n");
        return to_return;

    }
    system("truncate -s 0 tmp/intermediate");
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
      
        int old_len = count_ws(state_stk);
        int old_ps = parser_state;        
        short len_changed;
        short len_equal_and_state_changed;
      
        do {
            steprule();
            int new_len = count_ws(state_stk);
            len_changed = new_len < old_len;
            len_equal_and_state_changed = (new_len == old_len)
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
    system("truncate -s 0 tmp/intermediate");
    str(); /* Restore state*/
    return to_return;
}

char *steprule() {

    system("truncate -s 0 tmp/intermediate");
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
    system("truncate -s 0 tmp/intermediate");
    char* curr_rule = crule();
    char *to_return = malloc(1024);
    sprintf(to_return, "%sWhere %s", rule_pos, curr_rule);
    return to_return;
}

char *br() {

    char *to_return = malloc(128);
    char token [256];
    char final [300];
    printf("Token: ");
    fflush(stdout);
    fgets(token, sizeof(token), stdin);
    

    if(strlen(token) <= 1) {
        sprintf(to_return, "br: %s Not a valid token\n", c_token);
        return to_return;
    }
    /* Removing trailing newline character*/
    char *pos;
    if((pos=strchr(token, '\n')) != NULL)
        *pos = '\0';
   
    while(!c_token || strcmp(token, c_token) != 0) {
            next();
            read_from_ibison();
    }

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

            if(strstr(out, "Parser error")) {
                if(in_crule) {

                    if(last_reduced)
                        free(last_reduced);
                    last_reduced = malloc(128);
                    strcpy(last_reduced, "Could not indentify crule as string was not accepted\n");
                    eof = 1;
                    break;

                } else {
                    printf("Parser error! String not accepted\n");
                    quit();
                }

            }

            if(strcmp(out, state_response) == 0) {
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
            execl("/bin/cat", "/bin/cat",  "tmp/intermediate", NULL);
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

    system("mkdir tmp");


    pipe(fd_);
    in_crule = 0;
    if (argc < 3) {

        if(argc == 2) {
            if(strcmp("-h", argv[1]) == 0) {

                printf("sidBison %d\n\ncrule: Returns the current rule being parsed.\nstepruule: Steps to the next rule parsed in the Bison specification\nstrIdentifies the current position in the entire parsing process\nbrBreaks at a particular input token\nstep: Steps to the next action taken by the parser\nctkn: Displays the current token\nrulepos: Current position in rule\ntest <filename> Accepts a file to be debugged\nquit: Ends program\n", version);

            } else if (strcmp("-v", argv[1]) == 0) {
                printf("Version %d\n", version);
            }
        }
        

       fprintf(stderr,"\nUsage: sidbison <Bison specification> <lexer object>\n");
        exit(0);
    }

    lexobj = malloc(512);
    bspec = malloc(512);
    strcpy(lexobj, argv[2]);
    strcpy(bspec, argv[1]);

    intermediate = fopen("tmp/intermediate", "a+");
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
        
        system("truncate -s 0 tmp/intermediate");
        final = crule();
        system("truncate -s 0 tmp/intermediate");
    
    } else if (strcmp(command, "ctkn\n") == 0) {
        
        final = ctkn();

    } else if (strcmp(command, "step\n") == 0) {
        
        final = step();

    } else if (strcmp(command, "rulepos\n") == 0) { 

        system("truncate -s 0 tmp/intermediate");
        final = rulepos();
    
    } else if (strcmp(command, "str\n") == 0) {
    
        final = str();

    } else if (strcmp(command, "steprule\n") == 0) {
    
        system("truncate -s 0 tmp/intermediate");
        final = steprule();
        system("truncate -s 0 tmp/intermediate"); /*Must truncate after a reduce */

    } else if (strcmp(command, "br\n") == 0) {
    
        final = br();

    } else if (sscanf(command, "test %s", temp) == 1) {

        test_file = malloc(512);

        system("truncate -s 0 tmp/intermediate");
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
        sprintf(final,"Unrecognized command %s\n", command);
    }

    return final;
}


/* Counts the number of whitespaces in a string */
int count_ws (char *s) {

    int count = 0;
    if(s) {
        while(*s != '\0') {

            if(isspace(*s)) {
                count++;
            }
            s++;
        }
    }
    return count;
}

