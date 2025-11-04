#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n",__FILE__, __LINE__), exit(EXIT_FAILURE))

//the user input should be ./prog12 -n myfile -p 644 -s 1000, we must read the values after -n -p and -s and store them

//if user didnt give correct arguments show the proper usage and quit
void usage(char *pname){
    fprintf(stderr, "usage: %s -n Name -p OCTAL -s SIZE\n", pname);
    exit(EXIT_FAILURE);
} 

void make_file(char *name, ssize_t size, mode_t perms, int percent){
    FILE *f;
    // we adjust permision mask
    //when we create a file the system starts with a base permisiion
    umask(~perms & 0777); // we disable the not peritted things from 0777(all permisions) so ~perms, Turn off all permission bits that are not in my desired perms
    if((f = fopen(name, "w+")) == NULL){ // w+ means create a new file for reading and writing and truncate if it exists
    // w+ - default creation mode    
        ERR("fopen");
    }
    if(size > 0){
        if(fseek(f, size - 1, SEEK_SET)) ERR("fseek");
        if(fputc('\0', f) == EOF) ERR("fputc");
    }
    for(int i = 0; i < (size * percent)/100; i++){ // (size * percent)/100 - number of random characters to write 10% of total
        if(fseek(f, rand() % size, SEEK_SET)) // rand() % size - pick random byte position in a file, fseek moves cursor to that position, SEEK_SET - beginning of the file
            ERR("fseek");
        //'Z' - 'A' + 1 = 26 letters total, i % ... - reminder when dividing by 26 -> nums 0...25, 'A' + ... - converts nums to characters: 'A' + 1 = 'B'
        //printf one character to file sream f: fprintf(s1, "%c", some_character);
        fprintf(f, "%c", 'A' + (i % ('Z' - 'A' + 1))); //write a single uppercase letter it cycles throught A-Z in order
    }
    //unwritten positions of the file remain zeroes 
    if(fclose(f))
        ERR("fclose");
}

int main(int argc, char **argv){
    int c;
    char *name = NULL;
    mode_t perms = -1; // mode_t is a system-defined type used for filepermissiosn and file types, holds permission bits
    ssize_t size = -1; // signed intiger type used for sizes or counts can also indicate an error negative value, used for return values from functions like read() or write(), holding file sizes in bytes
    //getopt looks for options that start with -, recognizes letters p:n:s, save each options argument - the word afyer it in the glal variable optarg, returns option letter
    while((c = getopt(argc, argv, "p:n:s:")) != -1) { 
        switch(c){ // we switch between option letters the getopt returns
            case 'p':
            //strtol converts string to long (numeric value), 8means interpret as octal(base8)
                perms = strtol(optarg, (char**)NULL, 8); // we store value after -p in a global variable called optarg
                break;
            case 's':
                size = strtol(optarg, (char**)NULL, 10); // we store string after -s in size convert it from text to a number using base 10 (decimal)
                break;
            case 'n':
                name = optarg; // we store string after -n in name
                break;
            case '?':
            default:
                usage(argv[0]); // if user types unknown option we print usage message and quit
        }
    }
    //we check if three options were provided, (mode_t)-1 czyli jakby zmieniamy typ -1 na mode_t
    if((NULL == name) || ((mode_t)-1 == perms) || (-1 == size)) usage(argv[0]);
    if(unlink(name) && errno != ENOENT) ERR("unlink"); // ENOENT indicates nonexistent file so we cant delete it if it didnt exist
    srand(time(NULL)); // gives you a new sequence of random numbers each time you run the program
    make_file(name, size, perms, 10);
    return EXIT_SUCCESS;
}


//getopt automatically reads command-line options, : after a letter means that this option needs an argument, the function reads one by one and returns the option letter 'p' 'n' 's' each time, when no more option left it returns -1







//int fseek(FILE *stream, long offset, int whence); - moves the files read/write cursor to a specific byte returns 0 if succeedes
//stream - file pointer from fopen, offset - how many bytes to move, whence - from where to start counting: SEEK_SET – from beginning, SEEK_CUR – from current position, SEEK_END – from end (use negative offsets to move backward)
//FILE *f = fopen("data.txt", "r"); - we open file, fclose(f) - closes the
//r - open file for reading, w - truncate to 0 and open for writing, append - appending data to the end of the existing content, r+ - opens content for reading and writing
//ftell - returns the files exact size in bytes
//unlink - deletes a file
//umask - sets processes default perrmision mask, (user, group, others) (read(4), write(2), execute(1))
//BEFORE WRITING AND READING OPEN FILE FIRST AND CLOSE WHEN DONE

//writing data
//text: fprintf(FILE *stream, const char *format, ...) - writes text to a file
//one char: fputc(int c, FILE *stream) - writes a single character to a file.
//string: fputs(const char *str, FILE *stream) - Writes a whole string (without adding \n automatically)
//structs, arrays, binary files: fwrite(const void *ptr, size_t size, size_t count, FILE *stream) - Writes binary data — raw bytes from memory

//example: int data[5] = {1, 2, 3, 4, 5}; fwrite(data, sizeof(int), 5, f);


//reading data
//fscanf(FILE *stream, const char *format, ...) - Works like scanf, but reads from a file
//fgetc(FILE *stream) - Reads one character from a file
//fgets(char *buf, int n, FILE *stream) - Reads a whole line (up to n-1 chars or newline)
//fread(void *ptr, size_t size, size_t count, FILE *stream) - Reads binary data (raw bytes)

//example: char buffer[100]; size_t n = fread(buffer, 1, sizeof(buffer), f);, readss up to 100 bytes, n tells how many actually read


//ssize_t writev(int fildes, const struct iovec *iov, int iovcnt); fildes - descriptor to which data is written, iov - array of structures describing the buffers from which data is gathered
//void   *iov_base -> pointer to the memory area
//size_t  iov_len  -> length of the memory area
//ssize_t readv(int fildes, const struct iovec *iov, int iovcnt); 