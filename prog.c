#include <errno.h> //error handling
#include <getopt.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))
//defines macro ERR that: prints system error info with perror(source), prints file name and line number, exits program with failure code

//if you run program incorectly this function shows how to use it
void usage(int argc, char* argv[]){
    printf("%s pattern\n", argv[0]);
    printf("pattern - string pattern to search at standard input\n");
    exit(EXIT_FAILURE); //???
}

int main(int argc, char* argv[]){
    if(argc != 2){ //if you give more then one argument argc must be 2 because name of file and pattern
        usage(argc, argv);
    }
    char* pattern = argv[1]; // patern if the word that we write after name of program
    char* line_number_env = getenv("W1_LINENUMBER"); // env variable saying if we should print line numbers
    char* logfile_env = getenv("W1_LOGFILE"); // env variable saying if we should we output to a file
    FILE* f = NULL;
    int is_line_number_printed = 0;
    // we check if we print line numbers
    if(line_number_env && ((strcmp(line_number_env, "TRUE") == 0) || (strcmp(line_number_env, "true") == 0) || (strcmp(line_number_env, "1") == 0))){
        is_line_number_printed = 1;
    }
    if(logfile_env){ // if logfile_env is set and not nullwe try to open that file
        if((f = fopen(logfile_env, "w")) == NULL) // "w" is write mode, if the file exists the contents are erased if it doesnt then its created
            ERR("fopen"); // if its failure we print error
    }
    char* line = NULL; // buffer that will hold a line
    size_t line_len = 0; // size in bytes of that buffer
    int line_number = 0; 
    //at first buffer is empty so we give line and line_len to function and getline allocates memory and stores the adress in line it updates line_len to the size of the allocated memory
    //if line is not null it reallocates buffer big enough for new line, it means you can reuse same buffer for every line
    //it automatically reads from stdin until it sees a newline character
    while(getline(&line, &line_len, stdin) != -1){
        line_number++; // we increment line number
        if(strstr(line, pattern)){ // strstr returns pointer to the first occurence of pattern inside line or NULL if not found
            if(is_line_number_printed){ //flag set based on W1_LINENUMBER, if its 1  we show line numbers 
                printf("%d:%s", line_number, line);
                if(f){ // ensures we write to the log when its opened
                    fprintf(f, "%d:%s", line_number, line); // we print this to the file f instead of the terminal 
                    fflush(f); // writes data to the disk immediately
                }
            } else { // if we dont print line numbers
                printf("%s", line);
                if(f){
                    fprintf(f, "%s", line); // we write to the file
                    fflush(f);
                }
            }
        }
    }
    //getline uses malloc to allocate memory for lines so when we are done reading we must free that memory 
    if(line) free(line); //if line is not null not to free sth null if so we free
    if(f && fclose(f)) // only try to close if it was opened, if f is null no file opened if is skipped
        ERR("fclose"); 
    return EXIT_SUCCESS;
}

//to test:
//./prog apple < example.txt
//W1_LINENUMBER=TRUE ./prog apple < example.txt
//W1_LOGFILE=out.txt ./prog apple < example.txt
//W1_LINENUMBER=1 W1_LOGFILE=out.txt ./prog apple < example.txt

