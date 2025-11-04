#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

void scan_dir(){
    DIR *dirp; // variable that will be used to open a directory
    struct dirent *dp; //used to read one file/folder at a time
    struct stat filestat; //stores info about each file
    int dirs = 0, files = 0, links = 0, other = 0; //counters
    if((dirp = opendir(".")) == NULL){ // open current directory if fails exit
        ERR("opendir");
    }
    do{
        errno = 0; // we set errno to 0 to later check if an error happened
        // readdir(dirp) reads one entry like a.txt, folder1 etc, if theres nothing left it returns null
        if((dp = readdir(dirp)) != NULL){
            //lstat fills filestat with details about the file
            if(lstat(dp->d_name, &filestat)) ERR("lstat"); // stat tells info about the file symlink points to, lstat about the link itself
            //we check what kind of file this is:
            if(S_ISDIR(filestat.st_mode)) dirs++;
            else if(S_ISREG(filestat.st_mode)) files++;
            else if(S_ISLNK(filestat.st_mode)) links++;
            else other++;
        }
    } while(dp != NULL); // read until theres no more files in the folder
    if(errno != 0) ERR("readdir"); // if we finished because of an error and not because reading finished print an error
    if(closedir(dirp)) ERR("closedir"); //we close the directory
    printf("files: %d, dirs: %d, links: %d, others: %d", files, dirs, links, other);
}

int main(int argc, char **argv){
    scan_dir();
    return EXIT_SUCCESS;

}

//stat vs lstat:
//link.txt -> realfile.txt
//stat("link.txt") → shows details of realfile.txt
//lstat("link.txt") → shows that link.txt is a symlink