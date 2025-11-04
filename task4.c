#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n",__FILE__, __LINE__), exit(EXIT_FAILURE))

void usage(char *pname){
    fprintf(stderr, "usage: %s path1 size1 path2 size2...\n", pname);
    exit(EXIT_FAILURE);
} 

int main(int argc, char **argv){
    if(argc < 3 || (argc % 2) != 1){
        usage("argv[0]");
    }

    for(int i = 1; i < argc; i+=2){
        const char *path = argv[i];
        int size_limit = atoi(argv[i+1]); // Convert size argument to an integer
        DIR *dir;
        if((dir = opendir(path)) == NULL) {
            perror("opendir");
            printf("failed to open the directory: %s\n", path);
            continue;
        }
        struct dirent *dp;
        struct stat filestat;
        long long total_size = 0;
        while((dp = readdir(dir)) != NULL) {
            if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0){
                continue;
            }
            char full_path[512];
            snprintf(full_path, sizeof(full_path), "%s/%s", path, dp->d_name);
            if(lstat(full_path, &filestat) == -1) {
                perror("lstat"); 
                continue;
            }
            if(S_ISREG(filestat.st_mode)){
                total_size += filestat.st_size;
            }
        }
        closedir(dir);
        if(total_size > size_limit){
            printf("%s\n", path);
        }
    }
    return EXIT_SUCCESS;
}

