#define _XOPEN_SOURCE 700

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

struct Book{
    char* title;
    char* author;
    char* genre;
}typedef book_t;

book_t parser(FILE* f){
    char* lineptr = NULL;
    size_t n = 0;
    char* token;
    char* title = NULL;
    char* author = NULL;
    char* genre = NULL;
    while(getline(&lineptr, &n, f) != -1){
        lineptr[strlen(lineptr)-1] = '\0';
        token = strtok(lineptr, ":");
        if((token = strtok(NULL, ":")) == NULL){
            continue;
        } 
        if(strcmp(lineptr, "author") == 0){
            author = strdup(token);
        }
        if(strcmp(lineptr, "title") == 0){
            title = strdup(token);
        }
        if(strcmp(lineptr, "genre") == 0){
            genre = strdup(token);
        }
    }
    free(lineptr);
    if(author != NULL){
        printf("author: %s", author);
    } else {
        printf("author: missing!\n");
    }
    if(title != NULL){
        printf("title: %s", title);
    }else {
        printf("title: missing!\n");
    }
    if(genre != NULL){
        printf("genre: %s", genre);
    }else {
        printf("genre: missing!\n");
    }
    book_t book;
    book.title = title;
    book.author = author;
    book.genre = genre;
    return book;
}

// join 2 path. returned pointer is for newly allocated memory and must be freed
char* join_paths(const char* path1, const char* path2)
{
    char* res;
    const int l1 = strlen(path1);
    if (path1[l1 - 1] == '/')
    {
        res = malloc(strlen(path1) + strlen(path2) + 1);
        if (!res)
            ERR("malloc");
        strcpy(res, path1);
    }
    else
    {
        res = malloc(strlen(path1) + strlen(path2) + 2);  // additional space for "/"
        if (!res)
            ERR("malloc");
        strcpy(res, path1);
        res[l1] = '/';
        res[l1 + 1] = 0;
    }
    return strcat(res, path2);
}

void usage(int argc, char** argv)
{
    (void)argc;
    fprintf(stderr, "USAGE: %s path\n", argv[0]);
    exit(EXIT_FAILURE);
}



int main(int argc, char** argv) { 
    if(argc != 2){
        usage(argc, argv);
    }
    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        ERR("fopen");
    }

    book_t book = parser(file);

    if (book.title != NULL) {
        free(book.title);
    }
    if (book.author != NULL) {
        free(book.author);
    }
    if (book.genre != NULL) {
        free(book.genre);
    }

    fclose(file);

    return EXIT_SUCCESS;;
}
