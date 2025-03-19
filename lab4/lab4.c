#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

const int BUFFER_SIZE = 256;

// Custom fgets implementation using read(2)
//passing in a fd, buffer to read file into, and the buffer size
char *fgets2(char *buffer, int size, int fd) {
    ssize_t read_sz;  //Stores the return value of read()-->number of bytes read.
    int i = 0; //Tracks the number of characters read into buffer, resets after finding "\n"
    char c; //A temporary variable to store one character at a time.

    while ((read_sz = read(fd, &c, 1)) > 0) {  // Read one character at a time
        //iterate each character into buffer
        if (i < size - 1) {
            buffer[i++] = c;
        } 
    }

    if (c == '\n') {  // Stop when we reach newline     
        buffer[i] = '\0'; // Null-terminate
        return buffer;
    }
}

// Function to lookup key in the given file
int lookup_key(int fd, const char *key, char delimiter, char *buffer, size_t buf_sz) {
    char line[BUFFER_SIZE];

    while (fgets2(line, sizeof(line), fd)) {  //Reads one line at a time from the file using fgets2(). Stores the line in line[].
        int match = 1;
        for (int i = 0; key[i] != '\0'; i++) {  //Compares the first characters of line[] with key[].
            if (line[i] != key[i]) {
                match = 0;    //if the line returned doesnt start with the same character as key, break the for loop and back to while                      
                break;
            }
        }

        if (match) { //find delimiter
            int i = 0;
            while (line[i] != delimiter && line[i] != '\0') {   //find delimiter
                i++;  // Move to the next character
            }

            if (line[i] == delimiter) {  
                i++;  // Move past the delimiter
            }

            while (line[i] == ' ' || line[i] == '\t'){
                i++; // Skip whitespace
            } 

            int j = 0;

            while (line[i] != '\n' && line[i] != '\0' && j < buf_sz - 1) {
                buffer[j++] = line[i++];  //copying characters in buffer
            }

            buffer[j] = '\0'; // Null-terminate

            return 0; // Success
        }
    }

    return -1; // Key not found
}

int main(int argc, char *argv[]) {
    //ensures the user provides enough arguments
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <key> <delimiter> <file>\n", argv[0]);
        return 1;
    }

    //arguments to pass into lookup
    const char *key = argv[1];
    char delimiter = argv[2][0]; // First character of delimiter argument
    const char *file_path = argv[3];

    //open file and set fd
    int fd = open(file_path, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open file");
        return 1;
    }

    //buffer to hold extracted value
    char buffer[BUFFER_SIZE];

    //calling lookup key
    if (lookup_key(fd, key, delimiter, buffer, sizeof(buffer)) == 0) {
        printf("%s\n", buffer);
    }

    close(fd);
    return 0;
}


  // while (i < size - 1) {  // Read up to size - 1 characters
    //     read_sz = read(fd, &c, 1);  // Read one character at a time
    //     if (read_sz <= 0) {
    //         if (i == 0) {
    //             return NULL;  // Nothing was read, return NULL (EOF or error)
    //         } else {
    //             return buffer;  // Some data was read, return buffer
    //         }
    //     }

    //     buffer[i++] = c;
    //     if (c == '\n') {
    //         break;
    //     }
    // }  
    //     buffer[i++] = c;
    // //     if (c == '\n') {
    // //         break;
    // //     }
    // // }  