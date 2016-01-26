/* Senior Thesis
 * 
 * 
 *  Compile With: gcc -g sidbison.c -lnsl -o sidbison
 *
 *  Run with bison specification and lexer shared object as input
 *
 *
 *  TODO:
 *      - Redo break to only leverage next
 *      - ibison option things should be recorded too or removed
 * 
*/
#include <signal.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/time.h>




int num_next;
char *test_file;
short isbison;
char *bspec;
char *lexobj;
int parser_state;
char *c_token; //current token
char *tkn_stk;
unsigned short red;
char *last_reduced;
FILE *child_in; //input into child
FILE* intermediate;
FILE* logfile;
char *bison = "ibison";
char *flags = "-i";
int int_pid;
int inter_fd;
int in_crule;
char *steprule();
char* read_from_ibison();
char* execute_command(char* command);

void quit() {

    printf("Quitting\n");
    kill(int_pid, 9);
    system("rm intermediate");
    system("rm logfile");
    exit(0);

}

void next() {

    //Writing to a logfile here might be weird bcuz a crule will add
    //roo many nexts
    fwrite("next\n", strlen("next\n"), 1, child_in);
    fflush(child_in);
    if(!in_crule) {
        num_next++;
        fwrite("next\n", strlen("next\n"), 1, logfile);
        fflush(logfile);
    }
}

char *crule() {

    int pid;
    /* Do not lose the current ibison version
 *  Also do not want to lose current state. Probably
 *  put all the state things in a struct and save the struct
 *
 *  */
    FILE *tmp = child_in;
    int fd_[2]; 
    pipe(fd_);
    if (pid = fork()) {
        
        child_in = fdopen(fd_[1], "w");
        close(fd_[0]);
        
        size_t n = 400;
        char *command = malloc(512);
        
        rewind(logfile);
        in_crule = 1;

        int i;
        for(i = 0; i <2; i++) {
            getline(&command, &n, logfile);
            fwrite(command, strlen(command), 1, child_in);
            fflush(child_in);

        }
        
        for(i = 0; i < num_next; i++) {
            next();
            read_from_ibison();
        }
        
        steprule();

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
    child_in = tmp;
    kill(pid,9);
    char *to_return = malloc(512);
    sprintf(to_return, "crule: %s", last_reduced);
    return to_return;
}

char *steprule() {

    system("truncate -s 0 intermediate");
    red = 0;

    while(!red) {
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

char *rulepos() {
    fwrite("stack\n", strlen("stack\n"), 1, child_in);
    fflush(child_in);
    read_from_ibison();
    char *to_return = malloc(512);
    sprintf(to_return, "rulepos: %s\n",tkn_stk); 
    return to_return;
}

char *str(){}

char *br() {

    char token [256];
    char final [300];
    printf("Character: ");
    fflush(stdout);
    fgets(token, sizeof(token), stdin);
    
    /*sprintf((char *)final, "token %s\n", token);
    
    //writing commands to ibison
    fwrite(final, strlen(final), 1, child_in);
    fflush(child_in);
    fwrite(final, strlen(final), 1, logfile);
    fflush(logfile);*/

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
        size_t n = 400;
        size_t n2 = 400;
        int i;

        inter_in = fdopen(fd[0], "r");
        close(fd[1]);

        while(getline(&out, &n, inter_in) != -1) {
            
            if(isbison) {
                printf("(ibison) %s\n",out);
            }

            if(!in_crule && 
            sscanf(out, "Reading a token...next token is: %s\n",temp) == 1) {
            
               if(c_token)
                    free(c_token);

               c_token = malloc(512);
               strcpy(c_token, temp);

            } else if(sscanf(out, "Reduced %s\n",temp) == 1) {
               red = 1;
               free(out);
               out = NULL;
               getline(&out, &n, inter_in);
               free(last_reduced);
               last_reduced = malloc(512);
               strcpy(last_reduced, out);
            
            } else if(!in_crule && 
              sscanf(out, "Token is shifted. Entering state %d\n",
              &parser_state) == 1) {

            } else if (!in_crule &&
              strcmp(out, "Stacks:(states, tokens)\n") == 0) {
           
              free(out);
              out = NULL;
              getline(&out, &n, inter_in);
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

        logfile = fopen("logfile", "w+");

        fwrite(lexer, strlen(lexer), 1, child_in);
        fwrite(lexer, strlen(lexer), 1, logfile);
        fflush(child_in);
        fflush(logfile);

        printf("Stepwise Interactive Debugger Bison 1.0\n");
        printf("Please report bugs to Siddhartha.Prasad@tufts.edu\n");
        while(1) {
                
            printf(">");
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

    } else if (strcmp(command, "rulepos\n") == 0) { 

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
        fwrite(test_file, strlen(test_file), 1, logfile);
        fflush(logfile);
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
        fwrite(final, strlen(final), 1, logfile);
        fflush(logfile);

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



