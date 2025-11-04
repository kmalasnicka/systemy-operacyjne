#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
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

    // Open out.txt for writing
    FILE *output_file = fopen("out.txt", "w");
    if (output_file == NULL) {
        perror("Failed to open out.txt");
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; i += 2) {
        const char *path = argv[i];  // Get the directory path
        int size_limit = atoi(argv[i + 1]);  // Convert size argument to an integer

        DIR *dir;
        if ((dir = opendir(path)) == NULL) {
            // If directory can't be opened, print error and log to out.txt
            fprintf(stderr, "Failed to open directory: %s\n", path);
            fprintf(output_file, "Failed to open directory: %s\n", path);
            continue;  // Skip to the next directory if there is an error
        }

        struct dirent *dp;
        struct stat filestat;
        long long total_size = 0;  // To accumulate the total size of files in the directory

        // Process all files in the directory
        while ((dp = readdir(dir)) != NULL) {
            // Skip "." and ".."
            if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
                continue;  // Skip these entries
            }

            char full_path[512];  // Buffer to store the full path of the file
            snprintf(full_path, sizeof(full_path), "%s/%s", path, dp->d_name);  // Concatenate the directory and file name

            // Get file information
            if (lstat(full_path, &filestat) == -1) {
                perror("lstat");
                continue;
            }

            // Add the file size to the total size if it’s a regular file
            if (S_ISREG(filestat.st_mode)) {
                total_size += filestat.st_size;
            }
        }

        // Close the directory
        closedir(dir);

        // Print the directory name to out.txt if the total size is above the limit
        if (total_size > size_limit) {
            fprintf(output_file, "%s\n", path);  // Write to out.txt
        }
    }

    // Close the output file
    fclose(output_file);

    return EXIT_SUCCESS;
}
