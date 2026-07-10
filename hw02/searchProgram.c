/*
Name: Noah Le
BlazerId: nhle
Project #: HW 2
To compile: make
To run: ./searchProgram [options] [directory]

REFERENCES:
funcptr.c given in the homework assignment
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

// Global variables for command-line options
int opt_S = 0;
int opt_s = 0; 
long max_size = 0;
int opt_f = 0; 
char *pattern = NULL; 
int max_depth = -1;
int opt_t = 0; 
char type_filter = '\0';

// Function pointer definition
typedef int Myfunc(const char *pathname, const struct stat *statptr, int type, int depth);

// Forward declarations
int dopath(const char *fullpath, int depth, Myfunc *func);
int process_file(const char *pathname, const struct stat *statptr, int type, int depth);

int main(int argc, char *argv[]) {
    int opt;
    char *start_dir = "."; // Default to current directory

    // Process command-line options using getopt
    while ((opt = getopt(argc, argv, "Ss:f:t:")) != -1) {
        switch (opt) {
            case 'S':
                opt_S = 1;
                break;
            case 's':
                opt_s = 1;
                max_size = atol(optarg);
                break;
            case 'f':
                opt_f = 1;
                pattern = optarg;
                // getopt doesn't handle two arguments for one flag. 
                // manually grab the next argument for depth.
                if (optind < argc && argv[optind][0] != '-') {
                    max_depth = atoi(argv[optind]);
                    optind++;
                } else {
                    fprintf(stderr, "Error: Option -f requires a string pattern and a depth.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 't':
                opt_t = 1;
                type_filter = optarg[0];
                if (type_filter != 'f' && type_filter != 'd') {
                    fprintf(stderr, "Error: Option -t requires 'f' (files) or 'd' (directories).\n");
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                fprintf(stderr, "Usage: %s [-S] [-s size] [-f pattern depth] [-t f|d] [dir]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Determine starting directory
    if (optind < argc) {
        start_dir = argv[optind];
    }

    // Start traversal using function pointer 
    dopath(start_dir, 0, process_file);

    return 0;
}

/*
 * Recursively traverses directories.
 * Returns 0 on success.
 */
int dopath(const char *fullpath, int depth, Myfunc *func) {
    struct stat statbuf;
    DIR *dp;
    struct dirent *dirp;

    // Use lstat to not follow symbolic links automatically
    if (lstat(fullpath, &statbuf) < 0) {
        return -1; // Cannot stat file
    }

    // Apply the function to the current file/directory
    if (S_ISDIR(statbuf.st_mode)) {
        func(fullpath, &statbuf, 2, depth); // 2 represents directory
        
        // Open the directory to read its contents
        if ((dp = opendir(fullpath)) == NULL) {
            return -1; // Cannot read directory
        }

        while ((dirp = readdir(dp)) != NULL) {
            // Skip "." and ".." to avoid infinite loops
            if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0) {
                continue;
            }

            // Construct new path
            char nextpath[2048];
            snprintf(nextpath, sizeof(nextpath), "%s/%s", fullpath, dirp->d_name);
            
            // Recursive call for subdirectory items
            dopath(nextpath, depth + 1, func);
        }
        closedir(dp);
    } else {
        func(fullpath, &statbuf, 1, depth); // 1 represents regular file / link
    }

    return 0;
}

/*
 * Function executed on each item found during traversal.
 * Handles filtering and output formatting.
 */
int process_file(const char *pathname, const struct stat *statptr, int type, int depth) {
    // Extract basename from the path (e.g., "projects/project1" -> "project1")
    const char *basename = strrchr(pathname, '/');
    if (basename) {
        basename++; // Skip the slash
    } else {
        basename = pathname;
    }

    // Filter: -t (Graduate requirement)
    if (opt_t) {
        if (type_filter == 'f' && !S_ISREG(statptr->st_mode)) return 0;
        if (type_filter == 'd' && !S_ISDIR(statptr->st_mode)) return 0;
    }

    // Filter: -s (Max size)
    if (opt_s) {
        long current_size = S_ISDIR(statptr->st_mode) ? 0 : statptr->st_size;
        if (current_size > max_size) return 0;
    }

    // Filter: -f (Pattern & Depth)
    if (opt_f) {
        if (strstr(basename, pattern) == NULL) return 0;
        if (depth > max_depth) return 0;
    }

    // --- Output Formatting ---

    // 1. Print tabs based on depth
    for (int i = 0; i < depth; i++) {
        printf("\t");
    }

    // 2. Print filename
    printf("%s", basename);

    // 3. Print symbolic link target if applicable
    if (S_ISLNK(statptr->st_mode)) {
        char target[1024];
        ssize_t len = readlink(pathname, target, sizeof(target) - 1);
        if (len != -1) {
            target[len] = '\0';
            printf(" (%s)", target);
        }
    }

    // 4. Filter: -S (Attributes)
    if (opt_S) {
        long print_size = S_ISDIR(statptr->st_mode) ? 0 : statptr->st_size;
        
        // Format the access time
        char time_buf[64];
        struct tm *tm_info = localtime(&statptr->st_atime);
        strftime(time_buf, sizeof(time_buf), "%b %d %H:%M:%S %Y", tm_info);

        // Extract permissions via octal mask
        printf(" (size: %ld, perms: %o, access: %s)", print_size, statptr->st_mode & 0777, time_buf);
    }

    printf("\n");
    return 0;
}