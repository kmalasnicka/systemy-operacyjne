#define _XOPEN_SOURCE 500 // enables <ftw.h> to declare nftw(), must be defined before headers

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <ftw.h>

#define MAXFD 20 //upper bound for file descriptors - unique identifiers that the os assigns to a file when its opened
#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))
 
int dirs = 0, files = 0, links = 0, other = 0; // counters

//callback function that nftw calls once per visited item inside a directory
int walk(const char *name, const struct stat *s, int type, struct FTW *f){
    switch(type){
        case FTW_DNR: // directory not readable, still countrs as dir
        case FTW_D: // directory
            dirs++;
            break;
        case FTW_F: // regular file
            files++;
            break;
        case FTW_SL: // symbolic link
            links++;
            break;
        default:
            other++;
    }
    return 0; // ifneverything succeded
}

int main(int argc, char **agrv){
    for(int i = 1; i < argc; i++){ // iterate over each command line path, we skip program name at agrv[0]
        //because we are passing function name walk, not calling it name(), it is a pointer automatically
        if(nftw(agrv[i], walk, MAXFD, FTW_PHYS) == 0) // nftw passes arguments automatically to walk, so every callback for nftw must have same parameters
        //FTW_PHYS - do not follow symlinks (count links themselves; don’t traverse into their targets), we dont want to follow symlinks because it might point outside of the tree and create infinite loops
            printf("%s:\nfiles:%d\ndirs:%d\nlinks:%d\nother:%d\n", agrv[i], files, dirs, links, other); // on success nftw returns 0 and we print
        else // if nftw returned non zero we print a generic failure
            printf("%s: access denied\n", agrv[i]);
            perror(argv[i]);
        dirs = files = links = other = 0; // we reset counters before next root path!!!
    }
    return EXIT_SUCCESS;
}


//nftw - walks an entire directory tree starting at path -> returns 0 if it finished tree successfulu
//int nftw(const char *path, int (*fn)(const char *fpath, const struct stst *sb,
// int typeflag, struct FTW *ftwbuf), int fd_limit, int flags)
// path - start directory, the root
//fn - function that countrs prints or sth like that
//fd_limit - max open directory file descriptors
//flags: FTW_PHYS - dont follow symlinks, FTW_DEPTH - do post-order..

//int callback(
//    const char *fpath,          full path to the current file or directory
//    const struct stat *sb,      pointer to file information (size, permissions, type etc)
//    int typeflag,               tells what kind of object this is (file, dir, link, etc.)
//    struct FTW *ftwbuf          contains info about where you are in the tree

//S_ISREG(sb->st_mode)  is regular file
//S_ISDIR(sb->st_mode)  is directory
//S_ISLNK(sb->st_mode)  is symbolic link

//struct FTW {
//    int base;    index in fpath where the file name starts
//    int level;   how deep we are (root = 0)
//};
//Example:
//If fpath = "/usr/bin/cat", then
//ftwbuf->base = 9 → 'c' is at position 9 in the string,
//ftwbuf->level = 2 → depth = 2 (root /usr = 0, /usr/bin = 1, /usr/bin/cat = 2).

//walk, walk&    → pointer to function walk()
//walk()         → call the function
//similarrly:
//int arr[3];
//int *p = arr;   -> arr automatically becomes a pointer to its first element
