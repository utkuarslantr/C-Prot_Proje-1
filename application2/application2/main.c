#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

// Function to search for a string in a binary file
int search_string_in_binary_file(const char *file_path, const char *search_string) {
    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return -1; // Return -1 on error
    }

    int found = 0;
    char buffer[BUFFER_SIZE];
    size_t search_len = strlen(search_string);
    size_t read_bytes;

    // Read the file in chunks
    while ((read_bytes = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        // Search for the string in the buffer
        char *pos = buffer;
        while ((pos = memchr(pos, search_string[0], (read_bytes - (pos - buffer))))) {
            // Check if remaining buffer has enough space to match the search string
            if ((read_bytes - (pos - buffer)) >= search_len) {
                if (memcmp(pos, search_string, search_len) == 0) {
                    found = 1;
                    break;
                }
                pos++; // Move to next occurrence of first character
            } else {
                break; // Not enough space to match the search string
            }
        }
        if (found) {
            break; // Stop searching if found
        }
    }

    fclose(file);

    return found;
}

// Function to print the entire content of a file
void print_file_contents(const char *file_path) {
    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char buffer[BUFFER_SIZE];
    size_t read_bytes;

    // Read and print the file in chunks
    while ((read_bytes = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        fwrite(buffer, 1, read_bytes, stdout);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <file_path> <search_string>\n", argv[0]);
        return 1;
    }

    char *file_path = argv[1];
    char *search_string = argv[2];

    // Print file contents
    printf("File contents:\n");
    print_file_contents(file_path);
    printf("\n");

    // Call function to search for the string in the binary file
    int result = search_string_in_binary_file(file_path, search_string);

    // Print result based on the search
    if (result == 1) {
        printf("'%s' found in file '%s'.\n", search_string, file_path);
    } else if (result == 0) {
        printf("'%s' not found in file '%s'.\n", search_string, file_path);
    } else {
        printf("Error opening or reading from file '%s'.\n", file_path);
    }

    return 0;
}
