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

//book with title, author, genre
struct Book{
    char* title;
    char* author;
    char* genre;
}typedef book_t;

//function to read the file line by line 
book_t parser(FILE* f){
    char* lineptr = NULL; // pointer to a buffer where each line will be stored
    size_t n = 0; // size of the buffer
    char* token;
    char* title = NULL;
    char* author = NULL;
    char* genre = NULL;
    while(getline(&lineptr, &n, f) != -1){ // we read file until eof
        lineptr[strlen(lineptr)-1] = '\0'; // getline reads \n so we want to replace it with \0
        token = strtok(lineptr, ":"); // we assign value in front of : to token 
        if((token = strtok(NULL, ":")) == NULL){ // if no value after : we return null
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

int walk(const char *path, const struct stat *s, int type, struct FTW *f) {
    if(type == FTW_F){// process only regular files
        const char* target_dir = "index/by_visible_title";
        //we check if symlink already exists
        char* visible_title_path = join_paths(target_dir, path + f->base);
        struct stat st;
        if (stat(visible_title_path, &st) == 0) {  // If the file already exists
            fprintf(stderr, "Error: Duplicate file name detected: %s\n", visible_title_path);
            free(visible_title_path);
            return -1;  // Return error if the link already exists
        }

        // Create the relative symbolic link
        if (symlink(path, visible_title_path) == -1) {
            ERR("symlink");
        }

        free(visible_title_path);
    }
    return 0;
}

// Function to initialize the index directory and create subdirectories
void create_index_directories() {
    if (mkdir("index", 0755) == -1 && errno != EEXIST) {
        ERR("mkdir");
    }
    if (mkdir("index/by_visible_title", 0755) == -1 && errno != EEXIST) {
        ERR("mkdir");
    }
}

void usage(int argc, char** argv)
{
    (void)argc;
    fprintf(stderr, "USAGE: %s path\n", argv[0]);
    exit(EXIT_FAILURE);
}



int main(int argc, char** argv) { 
    if (argc != 1) {
        fprintf(stderr, "Usage: %s\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Create the index directory structure
    create_index_directories();

    // Use nftw to traverse the "library" directory
    if (nftw("library", walk, 10, FTW_PHYS) == -1) {
        ERR("nftw");
    }

    return EXIT_SUCCESS;
}
