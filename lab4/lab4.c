#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 256

// Custom fgets implementation using read(2)
char *my_fgets(char *buffer, int size, int fd) {
    static char internal_buffer[BUFFER_SIZE];
    static int buf_pos = 0, buf_size = 0;
    int i = 0;

    while (i < size - 1) {
        if (buf_pos >= buf_size) {
            buf_size = read(fd, internal_buffer, BUFFER_SIZE);
            if (buf_size <= 0) return NULL; // End of file or error
            buf_pos = 0;
        }

        char c = internal_buffer[buf_pos++];
        buffer[i++] = c;
        if (c == '\n') break; // Stop at newline
    }

    buffer[i] = '\0'; // Null-terminate
    return buffer;
}

// Function to lookup key in the given file
int lookup_key(int fd, const char *key, char delimiter, char *buffer, size_t buf_sz) {
    char line[BUFFER_SIZE];

    while (my_fgets(line, sizeof(line), fd)) {
        int match = 1;
        for (int i = 0; key[i] != '\0'; i++) {
            if (line[i] != key[i]) {
                match = 0;
                break;
            }
        }

        if (match) {
            int i = 0;
            while (line[i] != delimiter && line[i] != '\0') i++; // Find delimiter
            if (line[i] == delimiter) i++; // Move past delimiter

            while (line[i] == ' ' || line[i] == '\t') i++; // Skip whitespace

            int j = 0;
            while (line[i] != '\n' && line[i] != '\0' && j < buf_sz - 1) {
                buffer[j++] = line[i++];
            }
            buffer[j] = '\0'; // Null-terminate

            return 0; // Success
        }
    }

    return -1; // Key not found
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <key> <delimiter> <file>\n", argv[0]);
        return 1;
    }

    const char *key = argv[1];
    char delimiter = argv[2][0]; // First character of delimiter argument
    const char *file_path = argv[3];

    int fd = open(file_path, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open file");
        return 1;
    }

    char value[BUFFER_SIZE];
    if (lookup_key(fd, key, delimiter, value, sizeof(value)) == 0) {
        printf("%s\n", value);
    }

    close(fd);
    return 0;
}
