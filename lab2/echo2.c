#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>

//VS Code didnt like opting and optarg, so adding these here
extern char *optarg;
extern int optind;

//int rotate(char *ch, int places) {
int rotate(char c, int rotatePlaces)
{
    int ascii_value = (int)c;

    // Check if the character is within the printable ASCII range (33-126)
    if (ascii_value >= 33 && ascii_value <= 126)
    {
        // Perform the rotation
        ascii_value = (ascii_value - 33 + rotatePlaces) % 94 + 33;
        // Convert back to character and print
        printf("%c", (char)ascii_value);

        return 0; // Successful rotation
    }

    // Return 1 if the character is outside the valid range
    return 1;
}

int main(int argc, char *argv[]) {
    int c;
    int rotatePlaces = 0;  // Initialize to 0 to avoid rotating if -r is not used
    //int start = 1;  // replacing with optind
    bool newLine = true;

    // Process flags
    while ((c = getopt(argc, argv, "nhr:")) != -1) {
        switch (c) {
            case 'n':
                //-n flag for skipping \n 
                printf("-n flag found\n");
                newLine = false;
                //start++;  // Skip the -n argument
                break;
            case 'r':
                // -r flag for rotating characters
                rotatePlaces = atoi(optarg);  // Convert optarg to integer
                break;
                /*previous approach
                 if (optarg != NULL) {
                     rotatePlaces = atoi(optarg);  // Convert the rotation value from string to integer
                     printf("-r flag found, rotating by %d places\n", rotatePlaces);
                 }
                //start+=2;  // Skip the -r argument
                */
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

    // Rotate all characters in all remaining arguments after -r is processed
    for (int i = optind; i < argc; ++i) {
        for (int j = 0; argv[i][j] != '\0'; ++j) {
            //printf("arg: %c\n", argv[i][j]);
            rotate(argv[i][j], rotatePlaces);
              // Rotate each character
        }
        //printf("%s ", argv[i]);  // Print rotated argument
        printf(" ");  
    }
        
    //Print a newline if necessary
    if (newLine) {
        printf("\n");
    }

    return 0;
}

   