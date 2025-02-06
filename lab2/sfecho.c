#include <stdio.h>
#include <stdbool.h>

//int is the data type

int main(int argc, char *argv[])
{
	bool newline = false;
	int start = 1;
//why do we need argc if we have argv?
	if (argc >1 && argv[1][0] == '-' && argv[1][1] == 'n'){
		//turn off newline
		newline = false;
		start++;
	}
	
	for (int i = 0; i < argc; ++i){
//		if (argv[i][0] == '-'){
//			printf("we found an option: %c\n", argv[i][1]);
//		}
		printf("%s ", argv[i]);
	}

	if (newline){
		printf("\n");
	}
		
	return 0;
}
//will always return something.  In this case, if returns 0, everything worked correctly
//Isœ this automatically pulled from LosPadres to VSCODE?
