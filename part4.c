#define _XOPEN_SOURCE 700

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/uio.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

// Book structure to hold title, author, genre
struct Book {
    char title[65];  // Title field, 64 bytes + null terminator
    unsigned int size;
} typedef book_t;

// Function to read the binary database and store the book entries
void read_database(const char* db_path) {
    int db_fd = open(db_path, O_RDONLY);
    if (db_fd == -1) {
        ERR("open");
    }

    // Get the file size to determine how many entries we have
    struct stat st;
    if (fstat(db_fd, &st) == -1) {
        ERR("fstat");
    }

    size_t db_size = st.st_size;
    if (db_size % 68 != 0) {
        fprintf(stderr, "Database file size is not a multiple of 68 bytes\n");
        exit(EXIT_FAILURE);
    }

    int num_entries = db_size / 68;

    // Allocate buffer to read the database entries
    struct iovec* storage = malloc(num_entries * sizeof(struct iovec));
    book_t* books = malloc(num_entries * sizeof(book_t));

    for (int i = 0; i < num_entries; i++) {
        storage[i].iov_base = &books[i];
        storage[i].iov_len = 68;
    }

    // Read the database entries using readv
    ssize_t bytes_read = readv(db_fd, storage, num_entries);
    if (bytes_read == -1) {
        ERR("readv");
    }

    // Traverse the library and check for missing or mismatched books
    for (int i = 0; i < num_entries; i++) {
        books[i].title[64] = '\0'; // Ensure null-termination of the title

        // Construct the file path for the book in the by-title index
        char* book_path = join_paths("index/by-title", books[i].title);

        struct stat file_stat;
        if (stat(book_path, &file_stat) == -1) {
            if (errno == ENOENT) {
                printf("Book \"%s\" is missing\n", books[i].title);
            } else {
                ERR("stat");
            }
        } else {
            // Check for size mismatch
            if (file_stat.st_size != books[i].size) {
                printf("Book \"%s\" size mismatch (%u vs %ld)\n", books[i].title, books[i].size, file_stat.st_size);
            }
        }

        // Clean up allocated memory for the book path
        free(book_path);
    }

    // Free memory and close database file
    free(storage);
    free(books);
    close(db_fd);
}

// Join 2 paths and return a newly allocated string
char* join_paths(const char* path1, const char* path2) {
    char* res;
    const int l1 = strlen(path1);
    res = malloc(strlen(path1) + strlen(path2) + 2);  // +1 for '/' and +1 for null terminator
    if (!res) ERR("malloc");

    strcpy(res, path1);
    if (path1[l1 - 1] != '/') {
        res[l1] = '/';
        res[l1 + 1] = 0;
    }
    strcat(res, path2);
    return res;
}

// Create the index directory structure
void create_index_directories() {
    if (mkdir("index", 0755) == -1 && errno != EEXIST) {
        ERR("mkdir");
    }
    if (mkdir("index/by_visible_title", 0755) == -1 && errno != EEXIST) {
        ERR("mkdir");
    }
    if (mkdir("index/by-title", 0755) == -1 && errno != EEXIST) {
        ERR("mkdir");
    }
    if (mkdir("index/by-genre", 0755) == -1 && errno != EEXIST) {
        ERR("mkdir");
    }
}

// Main function
int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <database_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Create the index directory structure
    create_index_directories();

    // Read the database and check for missing books and size mismatches
    read_database(argv[1]);

    return EXIT_SUCCESS;
}
