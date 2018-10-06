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
#include <pwd.h> // for cd'ing into home directory

extern char **environ; // for printing env strings/accessing dirs
extern int errno; // for error handling

/* function prototypes */
//char* readCommand();
//char** parseLine(char *line);
void parseCommand(char* command, char** params, int *argc);
int executeCommand(char** argv);
int cdTo(char** argv);
int clrScreen();
int printDirContents();
int printEnvStrings();
int echoUserInput(char** input);
int displayManual();
int pauseShell();
int quitTerm();

/* structs */
struct InternalCommand {
	char *commandName;
	int (*func)(char**);
};
struct InternalCommand commands[] = {
	{ "cd", cdTo },
	{ "clr", clrScreen },
	{ "dir", printDirContents },
	{ "environ", printEnvStrings },
	{ "echo", echoUserInput },
	{ "help", displayManual },
	{ "pause", pauseShell },
	{ "quit", quitTerm }
};


int main(int argc, char *argv[100]) {
	//initialize variables needed
	char line[100]; // line to parse from user (separate commands from arguments)
	char cwd[100]; // used for printing current working directory after each command (cd'ing to new dir etc.)
	//char* argv[100]; // holds all arguments to be passed to commands
	//argc = 0; // argc is 0 before line read in
	//int argc; // keeps track of number of arguments passed in (argument count)
	int status; // used when creating other processes to determine if they executed or failed

	if (argc >= 2) { // if argc at this point equals greater than or equal to 2 then we know we need to read commands from a file instead of user input
		printf("Argc at start of program before any user input: %d\n\n", argc);
		printf("Argv at start of program before any user input (if this exists means we must execute commands from commands in some input file): %s\n\n", argv[1]);
		char* inputFileName = argv[1]; // save input file name to used (this is to ensure the name is not lost through executions and can be used in printing statements (for testing or error logging) later on)
		FILE *inputFile_ptr = fopen(inputFileName, "r"); // attempt to open file for reading
		char inputFileContent[200];
		if (inputFile_ptr == NULL) { // check to see if file failed to open
			fprintf(stdout, "No input file by the name \"%s\" found..\n", inputFileName); // print error
			return 0; // end program
		} else { // file opened successfully
			// read file and line by line execute it's commands
			fprintf(stdout, "Input file: %s opened successfully! Ready to execute commands from input file!\n", inputFileName);
			while(!feof(inputFile_ptr)) {
				if(fgets(inputFileContent, 200, inputFile_ptr)) {
					parseCommand(inputFileContent, argv, &argc); // parse line from file
					if (executeCommand(argv) == 0) break; // attempt to execute line from file based off parsed arguments in argv
				}
			}
		}
		fprintf(stdout, "\nSuccessfully executed commands from input file: %s - now exiting..\n", inputFileName);
		return 0; // at end of executing commands from input file, exit shell
	}
	if (argc == 1) { // argc only equal to 1, meaning only ./myshell was executed, accept user input as commands
		while(1) { //perform endless loop until shell detects an EOF condition such as 'Ctrl+D' or user types 'quit'
			printf("dillon@shell> "); // print prompt
			if (getcwd(cwd, sizeof(cwd)) != NULL) { // print pathname of current directory (NOT A TEST ACTUALLY NEED AS PART OF PROGRAM SO DON'T MACRO OUT)
				printf("- %s\n", cwd); // print pathname of current directory
			}
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
			printf("Home directory: %s\n", getenv("HOME")); // TEST

			parseCommand(line, argv, &argc); // parse user command enterd

			printf("ARGC: %d\n", argc); // TEST
			i = 0;
			while (argv[i] != '\0') {
				printf("ARGV: %s\n", argv[i]);
				i++;
			}
			
			printf("Length of line entered: %lu\n", strlen(line));
			
			if (executeCommand(argv) == 0) break; // attempt to execute user command based off parsed arguments in argv
		}
	}
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

/* Execute on parsed command, account for different operators, internal, and external commands */
int executeCommand(char** argv) {
	int i = 0;
	if (argv[0] == '\0') { // make sure some type of command entered (doesn't matter valid or invalid at this point because if it's an invalid command will be checked later)
		fprintf(stderr, "No command entered!\n"); // print error to stderr
		return 0; // return back to main while loop
	}
	// while (argv[i] != '\0') { // while argv is not equal to NULL character, search for any operators (redirection, piping)

	// }
	while (argv[i] != '\0') {
		if (commands[i].commandName == argv[0]) { // if an internal command
			printf("Command entered was right");
		}
		if (strcmp(argv[0], "cd") == 0) { // if cd command entered
			//if (argv[1]) { // if argv 1 holds a value, attempt to cdTo that directory
				fprintf(stdout, "Entered cd internal command function call in EXECUTE..\n");
				return cdTo(argv);
			//}
		}
		if(strcmp(argv[0], "help") == 0) { // if help command entered
			fprintf(stdout, "Entered help internal command function call in EXECUTE..\n");
			return displayManual(); // return result of help command (1 is successful, 0 is not)
			// if (displayManual()) { // if displayManual was successful
			// 	return 1;
			// } else {
			// 	fprintf(stdout, "Something went wrong executing help internal command..\n"); // print that something wrong with executing help internal command
			// }
		}
		if (strcmp(argv[0], "clear") == 0) {
			fprintf(stdout, "Entered clear internal command function call in EXECUTE..\n");
			return clrScreen();
		}
		if (strcmp(argv[0], "dir") == 0) {
			fprintf(stdout, "Entered dir internal command function call in EXECUTE..\n");
			return printDirContents();
		}
		if (strcmp(argv[0], "env") == 0) {
			fprintf(stdout, "Entered env internal command function call in EXECUTE..\n");
			return printEnvStrings(); // returns int for if function call successful or not
		}
		if (strcmp(argv[0], "echo") == 0) {
			fprintf(stdout, "Entered echo internal command function call in EXECUTE..\n");
			return echoUserInput(argv); // will quit the shell program by returning 0
		}
		if (strcmp(argv[0], "pause") == 0) {
			fprintf(stdout, "Entered pause internal command function call in EXECUTE..\n");
			return pauseShell(); // will quit the shell program by returning 0
		}
		if (strcmp(argv[0], "quit") == 0) {
			fprintf(stdout, "Entered quit internal command function call in EXECUTE..\n");
			return quitTerm(); // will quit the shell program by returning 0
		}
		i++;
	}
	fprintf(stdout, "\nNo recognizable command was found..reenter a proper command or type \"help\" for more information on proper commands.\n\n");
	return 1;
}

// char* readCommand() {
// 	char *command = NULL;
// 	size_t buffCapacity = 0;
// 	ssize_t nread;
// 	int i = 0;

// 	while ((nread = getline(&command, &buffCapacity, stdin)) != -1) {
// 		printf("Retrieved line of length %zu:\n", nread);
// 		printf("Command: %s", command);
// 		if (command[i-1] == '\n') {
// 			break;
// 		}
// 		fwrite(command, nread, 1, stdout);
// 		i++;
// 	}
// 	//command[i-1]= '\n';
// 	return command;
// }

// char** parseLine(char *line) {
// 	int i = 0;
// 	int bufferSize = PARSE_BUFF_SIZE; // define bufferSize to use for parsing line
// 	char** tokens = malloc(sizeof(char) * bufferSize); // all tokenized arguments to be returned to argv
// 	char* token; // individual token placed in tokens to be returned

// 	if (!tokens) { // check if malloc failed
// 		fprintf(stderr, "Error: Failed to allocate memory!\n"); // print error
// 		exit(EXIT_FAILURE); // exit
//   	}

//   	token = strtok(line, PARSE_DELIMS); // get token

// 	while (token != NULL) {
// 		tokens[i] = token;
// 		i++;
// 		if (i >= bufferSize) { // if i excedes bufferSize, we know we have to allocate more space for tokens
// 			bufferSize += PARSE_BUFF_SIZE; // increase the bufferSize
// 			tokens = realloc(tokens, sizeof(char*) * bufferSize); // reallocate memory for tokens using new bufferSize
// 			if (!tokens) { // check if realloc failed
// 				fprintf(stderr, "Error: Failed to reallocate memory!\n"); // print error
// 				exit(EXIT_FAILURE); // exit
// 			}
// 			token = strtok(NULL, PARSE_DELIMS); // reset token to be used
// 		}
// 	}


//   	tokens[i] = NULL; // set last token to null (this should place NULL at end of the C-string array)
//   	return tokens; // return tokens which is just everything but delims to be used
// }

/* Internal Commands */
//cdTo
int cdTo(char **argv) {
	uid_t uid = getuid(); // returns the real user ID of the calling process, that "real user ID is that of the user who has invoked the program." straight from the man page 'man getuid'
    struct passwd* pwd = getpwuid(uid); // getpwuid (as per the man page) "searches the password database for the given login name, useruid, or user uuid". It stores the results of the search in a pwd structure to be used. While this is more inefficient than just chdir(getenv("HOME")), I couldn't get that to work so this is the alternative

	if (argv == NULL) { // if argv is null
		fprintf(stderr, "\nError: cd failed1..\n"); // print an error
	}
	//char* env = getenv("HOME");
	if (argv[1] == '\0') { // if no second argument and just cd was entered
		fprintf(stdout, "ONLY cd entered, attempting to switch to home directory..\n");
	if (!(chdir(pwd->pw_dir))) { // switch to home directory using environment variable HOME
			fprintf(stdout, "\nSuccessful cd to HOME!\n");
			return 1; // return 1 for successful cd to HOME
		} else {
			fprintf(stderr, "cd: That directory does not exist..\n"); // print that directory does not exist - also means chdir couldn't chdir properly
			return 1; // return 1 so as not to end shell program
		}
	}

	if (!(chdir(argv[1]))) { // chdir returned 0, successful cd
		fprintf(stdout, "\nSuccessful cd!\n"); //print success
		return 1; // return 1 for success
	} else { // otherwise chdir failed
		fprintf(stderr, "cd: That directory does not exist..\n"); // print that directory does not exist - also means chdir couldn't chdir properly
		return 1; // return 0 for failure
	}
}

/* Clears shell screen */
int clrScreen() {
	printf("\033[H\033[2J"); // trick to clear screen
	return 1; // return 1 for clear success
}

/* Prints the contents of the current directory/Also supports output redirection! */
int printDirContents() {
	struct dirent *s; // pointer to directory entry
	DIR *dir = opendir("."); // open directory (opendir() returns pointer of DIR type)
	if (dir == NULL) { // check if dir is NULL
		printf("Error: Could not open current directory."); // print error if it is
		return 0; // return 0 for failure
	}

	while ((s = readdir(dir)) != NULL) { // readdir() returns pointer to next directory entry until it reaches end of the directory(NULL) or an error
		fprintf(stdout, "%s\t", s->d_name); // print the directory/file name in the current directory
	}
	fprintf(stdout, "\n"); // print new line after printing out contents of current directory for readability

	closedir(dir); // close dir
	return 1; // return 1 for success
}

/* Prints all environment variables to stdout/Also supports output redirection! */
int printEnvStrings() {
	int i = 0;
	while(environ[i] != NULL) {
		printf("%s\n", environ[i]);
		i++;
	}
	return 1; // return 1 for success
}

/* Echoes whatever the user entered back to screen/Also supports output redirection! */
int echoUserInput(char** input) {
	int i = 1; // i equal to one so as not to print the actual 'echo' part of command entered
	while(input[i] != '\0') { // while input (the argv array) is not equal to null
		fprintf(stdout, "%s ", input[i]); // print everything user entered except for 'echo' part
		i++; // increment i
	}
	fprintf(stdout, "\n"); // print newline at end for readability
	return 1; // return 1 for success
}

/* Prints user manual if just 'help' entered, if 'help cd' entered then a prompt as to how to use that internal command is printed to screen/Also supports output redirection! */
int displayManual() {
	printf("help screen :)\n");
	//FILE *readMe_ptr = fopen("readme.txt", "r");
	// allocate buffer for reading
	// read file with fgets, go until EOF reached, print all to stdout
	// close file
	return 1; // for success
}

/* Pauses the shell's execution until the user presses Enter */
int pauseShell() {
	fflush(stdin); // necessary before using gets()/getchar() to ensure no garbage included
	fprintf(stdout, "Press Enter to continue...\n"); // prompt user to press Enter to resume shell execution
	getchar(); // gets a character from stdin, in this case wants Enter pressed before returning 1 below
    return 1; // for success
}

/* Quits the shell by returning 0 */
int quitTerm() {
	return 0;
}