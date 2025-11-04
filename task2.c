#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>


#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n",__FILE__, __LINE__), exit(EXIT_FAILURE))

int main(int argc, char **argv){
    if (argc != 1) {  // no params
        fprintf(stderr, "usage: %s (run with no arguments)\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char *path = ".";
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    long long total_size = 0;

    if((dir = opendir(".")) == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }
    while((entry = readdir(dir)) != NULL){ 
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) // we skip over special entries . and .. 
            continue;
        char full_path[256];
        if(snprintf(full_path, sizeof(full_path), "%s/%s", ".", entry->d_name) >= sizeof(full_path)){ //we concatinae current directory and name of subdirectory
            fprintf(stderr, "Warning: path is too long for %s\n", entry->d_name);
            continue;
    }
             if(stat(full_path, &file_stat) == -1){
            perror("stat");
            continue;
        }
        printf("File size: %lld\n", (long long)file_stat.st_size);
        if(S_ISREG(file_stat.st_mode)){
            total_size += file_stat.st_size;
        }
    }
    closedir(dir);
    printf("total size: %lld\n", total_size);
    return EXIT_SUCCESS;
}