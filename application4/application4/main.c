#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// Function to check if a file is a PE (Portable Executable) file
int isPEFile(const char* filepath) {
    FILE* file = fopen(filepath, "rb"); // Open file in binary mode
    if (file == NULL) { // Unable to open file
        perror("Dosya acilamadi");
        return 0; // Return false
    }

    unsigned char signature[2];
    fread(signature, 1, 2, file); // Read first 2 bytes
    fclose(file); // Close file

    // Check if first 2 bytes match "MZ" (Portable Executable file signature)
    if (signature[0] == 'M' && signature[1] == 'Z') {
        return 1; // PE file detected, return true
    }

    return 0; // Not a PE file, return false
}

// Function to search for "MALWARE" string in a file
void searchMalware(const char* filepath) {
    FILE* file = fopen(filepath, "rb"); // Open file in binary mode
    if (file == NULL) { // Unable to open file
        perror("Dosya acilamadi");
        return; // Exit function
    }

    char buffer[1024];
    size_t bytesRead;
    int found = 0;

    // Read file chunk by chunk
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        // Search for "MALWARE" string
        for (size_t i = 0; i < bytesRead - 7; i++) {
            if (memcmp(&buffer[i], "MALWARE", 7) == 0) {
                found = 1;
                printf("\"MALWARE\" ifadesi %ld adresinde bulundu.\n", ftell(file) - bytesRead + i);
                break;
            }
        }
        if (found) {
            break;
        }
    }

    if (!found) {
        printf("\"MALWARE\" ifadesi herhangi bir konumda bulunamadi.\n");
    }

    fclose(file); // Close file
}

// Function to search for PE files in a directory
void searchPEFiles(const char* directory) {
    struct dirent* entry;
    DIR* dir = opendir(directory); // Open directory

    if (dir == NULL) {
        perror("Klasor acilamadi");
        return;
    }

    while ((entry = readdir(dir)) != NULL) { // For each entry in directory
        char filepath[1024];
        snprintf(filepath, sizeof(filepath), "%s/%s", directory, entry->d_name);

        struct stat sb;
        if (stat(filepath, &sb) == 0 && S_ISREG(sb.st_mode)) { // If entry is a regular file
            if (isPEFile(filepath)) { // If file is a PE file
                printf("%s: ", filepath);
                searchMalware(filepath); // Search for "MALWARE"
            }
            else {
                printf("%s: Gecerli PE dosyasi degil\n", filepath);
            }
        }
    }

    closedir(dir); // Close directory
}

// Main program
int main(int argc, char* argv[]) {
    if (argc < 2) { // Check if directory path is provided
        fprintf(stderr, "Kullanim: %s <klasor_yolu>\n", argv[0]);
        return 1; // Exit program with error
    }

    const char* directory = argv[1]; // Get directory path from command line

    // Search for PE files in directory
    searchPEFiles(directory);

    return 0; // Exit program successfully
}
