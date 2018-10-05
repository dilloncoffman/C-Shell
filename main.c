#include <stdio.h> // fopen, fclose, fgets
#include <stdlib.h> // malloc, realloc, free
#include <string.h> // strtok, strcmp
#include <errno.h>
#include <unistd.h> // for fork and exec
#include <sys/types.h> 
#include <sys/wait.h>
#include <dirent.h> // for chdir and opendir
#include <readline/readline.h>
#include <readline/history.h>

extern char *environ[]; // for printing env strings/accessing dirs
extern int errno; // for error handling

#define PARSE_BUFF_SIZE 100
#define PARSE_DELIMS " \r\t\a\n"

/* function prototypes */
//char* readCommand();
//char** parseLine(char *line);
void parseCommand(char* command, char** params, int *argc);
// int cdTo(char *argv);
// void clrScreen();
// void printDirContents();
// void printEnvStrings();
// void echoUserInput(char* input);
// void displayManual();
// void pauseShell();
// void quitTerm();

/* structs */
// struct InternalCommand {
// 	char *commandName;
// 	int (*func)(char*);
// };
// struct InternalCommand commands[] = {
// 	{ "cd", &cdTo },
// 	{ "clr", &clrScreen },
// 	{ "dir", &printDirContents },
// 	{ "environ", &printEnvStrings },
// 	{ "echo", &echoUserInput },
// 	{ "help", &displayManual },
// 	{ "pause", &pauseShell },
// 	{ "quit", &quitTerm }
// };


int main(int argc, char *argv[100]) {
	//initialize variables needed
	char line[100]; // line to parse from user (separate commands from arguments)
	//char* argv[100]; // holds all arguments to be passed to commands
	//argc = 0; // argc is 0 before line read in
	
	//int argc; // keeps track of number of arguments passed in (argument count)
	int status; // used when creating other processes to determine if they executed or failed

	//perform endless loop until shell detects an EOF condition
	while(1) {
		printf("dillon@shell> "); // print prompt

		// Read command from standard input, exit on EOF conditions (Ctrl + D & "exit")
		if (fgets(line, sizeof(line), stdin) == NULL) break; // get command from stdin, breaks if Ctrl + D entered
		fprintf(stdout, "Command entered: %s\n", line); //print command to stdout
        if (line[strlen(line)-1] == '\n') { // remove trailing newline if there is one
            line[strlen(line)-1] = '\0'; // replace that newline with null
        }

		int i = 0;
		while (line[i] != '\0') {
			printf("%c\n", line[i]);
			i++;
		}
		parseCommand(line, argv, &argc);
		printf("ARGC: %d\n", argc);
		i = 0;
		while (argv[i] != '\0') {
			printf("ARGV: %s\n", argv[i]);
			i++;
		}
		printf("line entered: %s\n", line);
		printf("Length of line entered: %lu\n", strlen(line));



		if(strcmp(argv[0], "exit\0") == 0) break; // exit if exit command entered (PUT INTO INTERNAL COMMAND VOID QUIT())
		
        
		// separate command from arguments - parse the line's arguments into argv

		// Exit and free any memory necessary

	}
	//free(&line);
	//free(&argv);
	return 0;
}

/* Parse command read in from user, increment argc for each string in command */
void parseCommand(char* command, char** argv, int *argc)
{       
	int i = 0;
	*(argc) = 0; // make sure for each command, argc is set back to 0
    for(i = 0; i < 100; i++) {
        argv[i] = strsep(&command, " "); // seperate each string by space and store that string in the respective index of params C-String array
        if(argv[i] == NULL) break;
		(*argc)++; // increment argc for every string in the command
    }
	argv[strlen(*argv) + 1] = NULL; // set first location in array after C-Strings to NULL (necessary for passing argv array to execvp() system call)
}

char* readCommand() {
	char *command = NULL;
	size_t buffCapacity = 0;
	ssize_t nread;
	int i = 0;

	while ((nread = getline(&command, &buffCapacity, stdin)) != -1) {
		printf("Retrieved line of length %zu:\n", nread);
		printf("Command: %s", command);
		if (command[i-1] == '\n') {
			break;
		}
		fwrite(command, nread, 1, stdout);
		i++;
	}
	//command[i-1]= '\n';
	return command;
}

char** parseLine(char *line) {
	int i = 0;
	int bufferSize = PARSE_BUFF_SIZE; // define bufferSize to use for parsing line
	char** tokens = malloc(sizeof(char) * bufferSize); // all tokenized arguments to be returned to argv
	char* token; // individual token placed in tokens to be returned

	if (!tokens) { // check if malloc failed
		fprintf(stderr, "Error: Failed to allocate memory!\n"); // print error
		exit(EXIT_FAILURE); // exit
  	}

  	token = strtok(line, PARSE_DELIMS); // get token

	while (token != NULL) {
		tokens[i] = token;
		i++;
		if (i >= bufferSize) { // if i excedes bufferSize, we know we have to allocate more space for tokens
			bufferSize += PARSE_BUFF_SIZE; // increase the bufferSize
			tokens = realloc(tokens, sizeof(char*) * bufferSize); // reallocate memory for tokens using new bufferSize
			if (!tokens) { // check if realloc failed
				fprintf(stderr, "Error: Failed to reallocate memory!\n"); // print error
				exit(EXIT_FAILURE); // exit
			}
			token = strtok(NULL, PARSE_DELIMS); // reset token to be used
		}
	}


  	tokens[i] = NULL; // set last token to null (this should place NULL at end of the C-string array)
  	return tokens; // return tokens which is just everything but delims to be used
}

/* Internal Commands */
//cdTo
int cdTo(char *argv) {
	if (argv == NULL) { // if argv is null
		printf("\nError: cd failed..\n"); // print an error
	}
	int result = chdir(&argv[1]); // store return of chdir into result
	if (result == 0) { // chdir returned 0, successful cd
		printf("\nSuccessful cd!\n"); //print success
		return 1; // return 1 for success
	} else {
		printf("Error: cd failed..\n"); // print error
		return 0; // return 0 for failure
	}
}

//clrScreen
void clrScreen() {
	printf("\033[H\033[2J");
}

// printDirContents
void printDirContents() {
	struct dirent *s; // pointer to directory entry
	DIR *dir = opendir("."); // open directory (opendir() returns pointer of DIR type)
	if (dir == NULL) { // check if dir is NULL
		printf("Error: Could not open current directory."); // print error if it is
	}

	while ((s = readdir(dir)) != NULL) { // readdir() returns pointer to next directory entry until it reaches end of the directory(NULL) or an error
		printf("%s\t", s->d_name); // print the directory/file name in the current directory
	}

	closedir(dir);
}
//printEnvStrings
void printEnvStrings() {
	int i = 0;
	while(environ[i] != NULL) {
		printf("%s\n", environ[i]);
		i++;
	}
}
//echoUserInput
void echoUserInput(char* input) {
	int i = 0;
	while(input != NULL) {
		printf("%c", input[i]);
	}
}

//displayManual
void displayManual() {
	FILE *readMe_ptr = fopen("readme.txt", "r");
	// allocate buffer for reading
	// read file with fgets, go until EOF reached, print all to stdout
	// close file
}

//pauseShell
void pauseShell() {
	fflush(stdin); // necessary before using gets()/getchar() to ensure no garbage included
}

//quitTerm
void quitTerm() {
	
}