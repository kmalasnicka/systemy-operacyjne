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

//dirent - name of the file, stat -properties of the file

// struct dirent {
//     ino_t          d_ino;       inode number (unique file ID on disk)
//     char           d_name[];    filename (null-terminated string)};

//struct dirent *dp = readdir(dirp); - you get dirent per file
// dp->d_name is just the file’s name, not the full path.

// struct stat {
//     mode_t    st_mode;    file type and permissions
//     ino_t     st_ino;     inode number
//     dev_t     st_dev;     device ID
//     nlink_t   st_nlink;   number of hard links
//     uid_t     st_uid;     user ID of owner
//     gid_t     st_gid;     group ID of owner
//     off_t     st_size;    total file size in bytes
//     time_t    st_atime;   last access time
//     time_t    st_mtime;   last modification time
//     time_t    st_ctime;   last status change};

//readdir() → gives you names (struct dirent).
//stat() / lstat() → gives you info about each name (struct stat)

//while ((dp = readdir(dirp)) != NULL) {
    // struct stat s;
    // lstat(dp->d_name, &s);

    // if (S_ISDIR(s.st_mode)) printf("It's a directory!\n");}
