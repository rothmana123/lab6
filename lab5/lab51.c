#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>

//VS Code didnt like opting and optarg, so adding these here
extern char *optarg;
extern int optind;

// Define a struct to hold program options
typedef struct {
    bool newLine;
    int rotatePlaces;
} ProgramOptions;

// Function to rotate a character
int rotate(char c, int rotatePlaces) {
    int ascii_value = (int)c;

    // Check if the character is within the printable ASCII range (33-126)
    if (ascii_value >= 33 && ascii_value <= 126) {
        // Perform the rotation
        ascii_value = (ascii_value - 33 + rotatePlaces) % 94 + 33;
        printf("%c", (char)ascii_value); // Print rotated character
        return 0; // Successful rotation
    }

    // Return 1 if the character is outside the valid range
    return 1;
}

// Function to process and print arguments using program options
void rotateAndPrint(int argc, char *argv[], ProgramOptions options) {
    for (int i = optind; i < argc; ++i) {
        for (int j = 0; argv[i][j] != '\0'; ++j) {
            rotate(argv[i][j], options.rotatePlaces);  // Rotate and print each character
        }
        printf(" ");  // Space between words
    }
    
    // Print a newline if -n was not specified
    if (options.newLine) {
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    int c;
    
    // Initialize the struct with default values
    ProgramOptions options = { true, 0 }; // newLine = true, rotatePlaces = 0

    // Process command-line options
    while ((c = getopt(argc, argv, "nhr:")) != -1) {
        switch (c) {
            case 'n':
                options.newLine = false;
                break;
            case 'r':
                options.rotatePlaces = atoi(optarg);
                break;
            case 'h':
                printf("Usage: %s [-n] [-r <places>] [arguments...]\n", argv[0]);
                printf("  -n    Suppress newline at the end\n");
                printf("  -r    Rotate characters by the specified number of places\n");
                return 0;
            default:
                abort();
        }
    }

    // Process and print arguments using the options struct
    rotateAndPrint(argc, argv, options);

    return 0;
}
