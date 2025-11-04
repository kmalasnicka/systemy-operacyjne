#define _GNU_SOURCE // used for TEMP_FAILURE_RETRY
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n",__FILE__, __LINE__), exit(EXIT_FAILURE))
#define FILE_BUF_LEN 256

void usage(const char *const pname){ // explains how the program works
    fprintf(stderr, "USAGE: %s path_1 path_2\n", pname);
    exit(EXIT_FAILURE);
}
//we need bulk_read and bulk_write because read and write may process less then count
//bytes and a signal can interupt them

ssize_t bulk_read(int fd, char *buf, size_t count){
    ssize_t c;
    ssize_t len = 0;
    do{
        //TEMP_FAILURE_RETRY - automatically retries the syscall if it failed with EINTR -> interrupted by signal
        c = TEMP_FAILURE_RETRY(read(fd, buf, count));
        if(c < 0) return c; // error -1
        if(c == 0) return len; // end of file nothing was read
        buf += c;
        len += c; // total bytes read
        count -= c;
    } while (count > 0); // while size of data read is supposed to read
    return len; // buffer if full: we return number of data successfully read
}

ssize_t bulk_write(int fd, char *buf, size_t count){
    ssize_t c;
    ssize_t len = 0;
    do{
        c = TEMP_FAILURE_RETRY(write(fd, buf, count));
        if(c < 0) return c; // error
        buf += c;
        len += c;
        count -= c;
    } while (count > 0);
    return len; // total bytes written = original request if no error
}

int main(const int agrc, const char *const *const argv){
    if(agrc != 3) usage(argv[0]);
    const char *const path_1 = argv[1]; // source file
    const char *const path_2 = argv[2]; // destination file
    const int fd1 = open(path_1, O_RDONLY); // we open source for reading
    if(fd1 == -1) ERR("open");
    const int fd2 = open(path_2, O_WRONLY | O_CREAT | O_TRUNC, 0777); // we open/create destination fpor writing, 0777 - creation mode, used only if file is created
    if(fd2 == -1) ERR("open");

    char file_buf[FILE_BUF_LEN]; //stack buffer 256 bytes
    while(1){ // infinite loop we break on end of file
        ssize_t read_size = bulk_read(fd1, file_buf, FILE_BUF_LEN); // readsup to FILE_BUF_LEN iteration
        if(read_size == -1) ERR("bulk_read");
        if(read_size == 0) break; //eof
        if(bulk_write(fd2, file_buf, read_size) == -1) ERR("bulk_write"); // we pass the file_buf that we earlier read bytes into from read
    }
    if(close(fd2) == -1) ERR("close");
    if(close(fd1) == -1) ERR("close");
    return EXIT_SUCCESS;
}

// int open(const char *path, int oflag...); path - path to the file being opened, oflag - flags for opening the file -> returnd dile descriptor
// ssize_t read(int fildes, void *buf, size_t nbyte) - fildes - file descriptor obtained from open, buf - pointer to the buffer where data will be stored, 
// nbyte - size of the data the func is expecting to read -> returns number of bytes successfuly read
// ssize_t write(int fildes, const void *buf, size_t nbyte) - fildes file descriptor obtained from open, buf pointer to the buffer containing data, 
// nbyte size of the data to be sent -> output number of data successfully written