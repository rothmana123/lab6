#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>


void read_from_stream(FILE *stream, bool linecount, int *linenumber) {
    //will need to take in lineCount parameter as a pointer
    
    char buf[128];
     //add linecount to each line
    while (fgets(buf, sizeof(buf), stream) != NULL) {
        if (linecount) {
            printf("%d %s", (*linenumber)++, buf);
        } else { //if linecount false, no linecount 
            printf("%s", buf);
        }
    } 
}

int main(int argc, char *argv[])
{
    int linenumber = 1;
    int c;
    bool linecount = false;
    extern int optind;

    // Process flags
    while ((c = getopt(argc, argv, "nh")) != -1) {
        switch (c) {
            case 'n':
                //-n flag for providing a line count
                linecount = true;
                break;
            case 'h':
                printf("Usage: %s [-n] [-r <places>] [arguments...]\n", argv[0]);
                printf("  -n    Add numebrs to each line printed\n");
                printf("  -h    Show this help message\n");
                return 0;
            default:
                abort();
        }
    }

    // If no files are provided, read from stdin
    if (optind == argc) {  // No filename provided, read from stdin
        read_from_stream(stdin, linecount, &linenumber);
    } else {
        for (int i = optind; i < argc; i++) { // Process each argument
            if (strcmp(argv[i], "-") == 0) { // If "-" is provided, read from stdin
                read_from_stream(stdin, linecount, &linenumber);
                //add linecount parameter
            } else {
                FILE *file = fopen(argv[i], "r");
                if (file == NULL) {
                    perror(argv[i]); // Print error but continue
                    continue;
                }
                //add linecount parameter
                read_from_stream(file, linecount, &linenumber);
                fclose(file);
            }
        }
    }
    return 0;
}

/*
Thus far, we’ve only really covered output – printing to the terminal with printf or puts. To implement cat, we also need to be able to read a file and then print its contents. 

The functions you should use for this lab are:

fopen – opens a file
fgets – gets a string from a file
fclose – closes a file when you are done with it
You’ll want to review the man pages for each of these to understand how they work.

Check out the class schedule page for a full walkthrough of cat to get an idea of how it should work. The basic workflow is:

Loop through each command line argument
Each argument is a file, so open it
If opening the file was successful, read its contents line by line
Print each of the lines to the terminal
Close the file
Move on to the next file
That’s it!

When you read the files, you have to have somewhere to put their contents temporarily before printing them – a buffer. We’ll create a character array to serve as our buffer:

// Create a buffer of 128 characters
char buf[128];

// ... and then pass 'buf' to the fgets function

*/


 //fopen(const char *restrict pathname, const char *restrict mode);
    // char pathname[100] = argv[1];
    // char mode[100] = argv[2];
    // fopen(pathname, mode);

