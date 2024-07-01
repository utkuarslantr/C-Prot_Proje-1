#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

void search_in_file(const char *filename, const char *search_str) {
    FILE *file = fopen(filename, "rb"); // Open file in binary mode
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char buffer[1024];
    size_t search_len = strlen(search_str);
    int found = 0; // Flag to check if search string is found

    while (1) {
        size_t read_bytes = fread(buffer, 1, sizeof(buffer), file);
        if (read_bytes == 0) {
            if (feof(file)) {
                break; // End of file
            } else {
                perror("Error reading file");
                break;
            }
        }

        // Search for search_str in buffer
        for (size_t i = 0; i <= read_bytes - search_len; ++i) {
            if (memcmp(&buffer[i], search_str, search_len) == 0) {
                printf("Found '%s' in file '%s' at byte offset %ld\n", search_str, filename, ftell(file) - read_bytes + i);
                found = 1; // Set found flag
                break;
            }
        }

        if (found) {
            break;
        }
    }

    if (!found) {
        printf("'%s' not found in file '%s'\n", search_str, filename);
    }

    fclose(file);
}

void search_in_directory(const char *dir_name, const char *search_str) {
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;

    if ((dir = opendir(dir_name)) == NULL) {
        perror("Error opening directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);

        if (stat(path, &statbuf) == -1) {
            perror("Error getting file stat");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue; // Skip current and parent directories

            search_in_directory(path, search_str); // Recursively search subdirectories
        } else {
            search_in_file(path, search_str); // Search the file
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <directory_path> <search_string>\n", argv[0]);
        return 1;
    }

    char *dir_name = argv[1];
    char *search_str = argv[2];

    printf("Searching for '%s' in directory '%s'\n", search_str, dir_name);
    search_in_directory(dir_name, search_str);

    return 0;
}
