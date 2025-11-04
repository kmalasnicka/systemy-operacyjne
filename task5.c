#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

void usage(char *pname) {
    fprintf(stderr, "usage: %s path1 size1 path2 size2...\n", pname);
    exit(EXIT_FAILURE);
} 

int main(int argc, char **argv) {
    if (argc < 3 || (argc % 2) != 1) {
        usage(argv[0]);
    }

    for (int i = 1; i < argc; i += 2) {
        const char *path = argv[i];  // Get the directory path
        int size_limit = atoi(argv[i + 1]);  // Convert size argument to an integer

        // Open the directory using open() system call
        int dir_fd = open(path, O_DIRECTORY | O_RDONLY);
        if (dir_fd == -1) {
            perror("open");
            printf("Failed to open the directory: %s\n", path);
            continue;  // Skip to the next directory if there is an error
        }

        struct dirent *dp;
        long long total_size = 0;  // To accumulate the total size of files in the directory

        char buffer[1024];  // Buffer for reading directory entries
        int nread;

        // Read directory entries using the low-level read() system call
        while ((nread = read(dir_fd, buffer, sizeof(buffer))) > 0) {
            struct dirent *entry;
            for (int i = 0; i < nread;) {
                entry = (struct dirent *)&buffer[i];
                i += entry->d_reclen;

                // Skip "." and ".."
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                    continue;
                }

                // Get file information
                struct stat filestat;
                char full_path[512];
                snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);  // Full path to the file

                if (stat(full_path, &filestat) == -1) {
                    perror("stat");
                    continue;
                }

                // Add the file size to the total size if it’s a regular file
                if (S_ISREG(filestat.st_mode)) {
                    total_size += filestat.st_size;
                }
            }
        }

        if (nread == -1) {
            perror("read");
        }

        // Close the directory
        close(dir_fd);

        // Print the directory name if the total size is above the limit
        if (total_size > size_limit) {
            printf("%s\n", path);
        }
    }

    return EXIT_SUCCESS;
}
