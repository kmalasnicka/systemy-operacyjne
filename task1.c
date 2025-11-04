#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>


#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n",__FILE__, __LINE__), exit(EXIT_FAILURE))
 

int walk(const char *name, const struct stat *s, int type, struct FTW *f){
    printf("%s\n", name); 
    return 0;
}

int main(int argc, char **argv){
    if(nftw(".", walk, 20, FTW_PHYS) != 0){ // insted of argv[1] shouldnt it be "."
        perror("nftw");
        printf("%s: access denied\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}


    // DIR *dirp;
    // FILE *f;
    // if(f = fopen(output.txt, "a") == NULL) ERR("fopen");
    // struct stat filestat;
    // struct dirent *dp;
    // for(int i = 1, i < argc, i++){
    //     if((dirp = opendir("argv[i]")) == NULL) ERR("opendir");
    //     if((dp = readdir(dirp)) != NULL){ // robimy strukture dp gdzie storujemy imie pliku
    //         if(lstat(dp->d_name, &filestat)) ERR("lstat"); // robimy druga strukture gdzie przechowamy total file size in bytes
    //         if(filestat->st_size > argv[i + 1]){
    //             fprintf(f, argv[i]);
    //         }
    //     }
    // }
    // return EXIT_SUCCESS;