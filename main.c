#include "myshell.h"
FILE *outFile_ptr; // used for output redirection, creating a file if it's not created, and or opening an already created file for use
int main(int argc, char* argv[100]) {
	//initialize variables needed
	char line[100]; // line to parse from user (separate commands from arguments)
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
					if (inputFileContent[strlen(inputFileContent)-1] == '\n') { // remove trailing newline if there is one in command
						inputFileContent[strlen(inputFileContent)-1] = '\0'; // replace that newline character with null character
					}
					argv = parseCommand(inputFileContent, argv, &argc); // parse line from file
					if (executeCommand(argv, &argc) == 0) break; // attempt to execute line from file based off parsed arguments in argv
				}
			}
		}
		fprintf(stdout, "\nSuccessfully executed commands from input file: %s - now exiting..\n", inputFileName);
		return 0; // at end of executing commands from input file, exit shell
	}
	if (argc == 1) { // argc only equal to 1, meaning only ./myshell was executed, accept user input as commands
		while(1) { //perform endless loop until shell detects an EOF condition such as 'Ctrl+D' or user types 'quit'
			printf("dillon@shell "); // print prompt
			if (getcwd(cwd, sizeof(cwd)) != NULL) { // print pathname of current directory (NOT A TEST ACTUALLY NEED AS PART OF PROGRAM SO DON'T MACRO OUT)
				printf("(%s) >> ", cwd); // print pathname of current directory
			}
			// Read command from standard input, exit on EOF conditions (Ctrl + D & "exit")
			if (fgets(line, sizeof(line), stdin) == NULL) break; // get command from stdin, breaks if Ctrl + D entered
			fprintf(stdout, "Command entered: %s\n", line); //print command to stdout
			if (line[strlen(line)-1] == '\n') { // remove trailing newline if there is one
				line[strlen(line)-1] = '\0'; // replace that newline with null
			}

			
			int i = 0;
			while (line[i] != '\0') { // (FOR TESTING) prints each character of the user-entered command
				printf("%c\n", line[i]);
				i++;
			}

			printf("Home directory: %s\n", getenv("HOME")); // (FOR TESTING)

			argv = parseCommand(line, argv, &argc); // parse user command entered

			printf("ARGC: %d\n", argc); // (FOR TESTING)
			i = 0; // (FOR TESTING) prints out each parsed C-string from user-entered command on a new line
			while (argv[i] != '\0') {
				printf("ARGV: %s\n", argv[i]);
				i++;
			}
			printf("\n"); // (FOR TESTING) print newline for readability
			
			if (executeCommand(argv, &argc) == 0) break; // attempt to execute user command based off parsed arguments in argv
		}
	}
	return 0;
}

/* Execute on parsed command, account for different operators, internal, and external commands */
int executeCommand(char** argv, int *argc) {
	if (argv[0] == '\0') { // make sure some type of command entered (doesn't matter valid or invalid at this point because if it's an invalid command will be checked later)
		fprintf(stderr, "No command entered!\n"); // print error to stderr
		return 0; // return back to main while loop
	}

	int i = 0; // keep track of index for argv
	int indexOfInputRedirOperator = 0; // used for keeping track of index where special operator occured
	int inputRedirectionEncountered = 0; // boolean var which is flipped to 1 if < is found in argv

	int indexOfOutputRedirOperator = 0;  // used for keeping track of index where special operator occured
	int outputRedirectionEncountered = 0; // boolean var which is flipped to 1 if > or >> is found in argv

	int indexOfPipeOperator = 0;  // used for keeping track of index where special operator occured
	int pipeEncountered = 0; // boolean var which is flipped to 1 if | is found in argv

	/* Check for special operators <, >, >>, &, | */
	while(argv[i] != NULL) { // while argv is not equal to NULL, search for any operators (redirection, piping)
		printf("argv in execute function: %s\n", argv[i]); // (FOR TESTING)
		if (strcmp(argv[i], "<") == 0) {
			printf("< ENCOUNTERED!\n");
			argv[i] = strcpy(argv[i], "");
			(*argc)--; // decrement argc since we don't want < to be included
			inputRedirectionEncountered = 1; // flip var that < was encountered to true (1)
			indexOfInputRedirOperator = i; // keep track of index of special operator, so when we execute on it, we know everything to the left and right of it
			i++;
			continue;
		}
		if (strcmp(argv[i], ">") == 0) {
			printf("> ENCOUNTERED!\n");
			argv[i] = strcpy(argv[i], "");
			(*argc)--;
			outputRedirectionEncountered = 1; // flip var that > was encountered to true (1)
			indexOfOutputRedirOperator = i; // record index of > found
			i++;
			continue;
		}
		if (strcmp(argv[i], ">>") == 0) {
			printf(">> ENCOUNTERED!\n");
			argv[i] = strsep(&argv[i], ">>");
			(*argc)--;
			outputRedirectionEncountered = 1; // flip var that >> was encountered to true (1)
			indexOfOutputRedirOperator = i; // record index of >> found
			i++;
			continue;
		}
		if (strcmp(argv[i], "|") == 0) {
			printf("| ENCOUNTERED!\n");
			argv[i] = strcpy(argv[i], "");
			(*argc)--;
			pipeEncountered = 1; // flip var that > was encountered to true (1)
			indexOfPipeOperator = i; // record index of | found
			i++;
			continue;
		}
		i++;
	}

	if (inputRedirectionEncountered) { // if an input redirection operator was encountered
		// i = 0;
		// argv[indexOfInputRedirOperator] = NULL; // make sure index of operator is NULL so it doesn't attempt to execute
		// if (strcmp(argv[indexOfInputRedirOperator], NULL) == 0) {
		// 	argv[indexOfInputRedirOperator] = strtok_r(argv[indexOfInputRedirOperator], NULL, argv);
		// }
		pid_t pid = fork();
		if (pid >= 0) {
			if (pid == 0) { // Child
				int inputFile = open(argv[1], O_RDONLY); // get file descriptor for input file
				// Replace stdin with inputFile
				close(0); // close stdin
				dup2(inputFile, 0); // replace stdin with file descriptor for inputFile
				close(inputFile); // close inputFile descriptor
				// Attempt to execute command; print a message and exit child process if error was encounted
				if (execvp(argv[0], &argv[2]) < 0) {
					fprintf(stdout, "\nError executing child process: %s\n\n", strerror(errno)); // there was an error, print errno, which is also set by execvp to indicate the kind of error
					exit(0); // exits child process if error occureed
				}
			} else { // Parent
				int status = 0;
				wait(&status); // wait for child process to finish executing
				fprintf(stdout, "\nSuccessful input redirection!\n"); // (FOR TESTING)
				return 1;
			}
		}
	}

	if (outputRedirectionEncountered) { // if an output redirection operator was encountered
		outFile_ptr = fopen(argv[1], "a");
		pid_t pid = fork();
		if (pid >= 0) {
			if (pid == 0) { // Child
				int outFile = open(argv[1], O_WRONLY|O_CREAT|O_APPEND, S_IRWXU|S_IRWXG|S_IRWXO); // get file descriptor for input file
				// Replace stdout with inputFile descriptor
				close(1); // close stdout
				dup2(outFile, 1); // replace stdout with file descriptor of outFile
				close(outFile); // close outFile descriptor
				// Attempt to execute command; print a message and exit child process if error was encounted
				if (execvp(argv[0], &argv[2]) < 0) {
					fprintf(stdout, "\nError executing child process: %s\n\n", strerror(errno)); // there was an error, print errno, which is also set by execvp to indicate the kind of error
					exit(0); // exits child process if error occureed
				}
			} else { // Parent
				int status = 0;
				wait(&status); // wait for child process to finish executing
				fprintf(stdout, "\nSuccessful output redirection!\n"); // (FOR TESTING)
				return 1;
			}
		}
	}

	// if (pipeEncountered) { // if a pipe operator was encountered
	// 	int pfds[2]; // create file descriptor
	// 	if (pipe(pfds) == 0) { // create pipe before fork, the pipe will be shared between two processes
	// 		if (fork() == 0) { // fork to create child process
	// 			// Child Process
	// 			close(1); // close stdout
	// 			dup2(pfds[1], 1); // wire stdout into pipe's write end
	// 			close(pfds[0]); // close read end of pipe
	// 			execlp(argv[0], argv[0], "-1", NULL); // execute command from child process
	// 		} else { // parent process
	// 			// Parent process
	// 			close(0); // close stdin
	// 			dup2(pfds[0], 0); // wire stdin into pipe's read end
	// 			close(pfds[1]); // close write end of the pipe
	// 			execlp(argv[0], argv[0], "-1", NULL); // execute command from parent process
	// 		}
	// 	}
	// 	return 1; // executed on pipe successfully
	// }

	// i = 0;
	// while(argv[i] != NULL) { // (DON'T NEED FOR TESTING) prints argv[i] after splitting on a special operator
	// 	printf("argv in execute function AFTER SPLIT: %s\n", argv[i]);
	// 	i++;
	// }

	
	/* Check if command entered was an internal command, if it was then make a function call to the specific function */
	if (strcmp(argv[0], "cd") == 0) { // if cd command entered
		//if (argv[1]) { // if argv 1 holds a value, attempt to cdTo that directory
			fprintf(stdout, "Entered cd internal command function call in EXECUTE..\n");
			return cdTo(argv); // return result of cdTo command (1 is successful, 0 is not)
		//}
	}
	if(strcmp(argv[0], "help") == 0) { // if help command entered
		fprintf(stdout, "Entered help internal command function call in EXECUTE..\n");
		return help(argv, argc); // return result of help command (1 is successful, 0 is not)
	}
	if (strcmp(argv[0], "clear") == 0) {
		fprintf(stdout, "Entered clear internal command function call in EXECUTE..\n");
		return clrScreen(); // return result of clear command (1 is successful, 0 is not)
	}
	if (strcmp(argv[0], "dir") == 0) {
		fprintf(stdout, "Entered dir internal command function call in EXECUTE..\n");
		return printDirContents(); // return result of dir command (1 is successful, 0 is not)
	}
	if (strcmp(argv[0], "env") == 0) {
		fprintf(stdout, "Entered env internal command function call in EXECUTE..\n");
		return printEnvStrings(); // return result of env command (1 is successful, 0 is not)
	}
	if (strcmp(argv[0], "echo") == 0) {
		fprintf(stdout, "Entered echo internal command function call in EXECUTE..\n");
		return echoUserInput(argv); // return result of echo command (1 is successful, 0 is not)
	}
	if (strcmp(argv[0], "pause") == 0) {
		fprintf(stdout, "Entered pause internal command function call in EXECUTE..\n");
		return pauseShell(); // return result of pause command (1 is successful, 0 is not)
	}
	if (strcmp(argv[0], "quit") == 0) {
		fprintf(stdout, "Entered quit internal command function call in EXECUTE..\n");
		return quitTerm(); // will quit the shell program by returning 0
	}

	/* Check if command is a file in the current directory and execute it to TEST: see that if you run your shell, you're able to run the executable again and thus have two shells executing at once - can check this by running ps -a to see all processes running */ 
	struct stat buffer; // struct stat from header <sys/stat.h> from man pages
	int exist = stat(argv[0], &buffer); // read the file's attributes, the file being argv[0]
	if (exist == 0) { // if stat returned 0 meaning that file specified by argv[0] exists and stat() was successful
		fprintf(stdout, "File EXISTS! Attempt to execute it!\n"); // (FOR TESTING) prints that it exists
		// execute file
		pid_t pid = fork(); // fork current process
		int status = 0; // set int status to wait for child process to complete
		if (pid >= 0) { // if no error in forking
			if (pid == 0) { // Child Process
				// is redirection?
				// is piping?
				
				if (execvp(argv[0], argv) < 0) { // is normal command, if the execvp call returns anything less than 0
					fprintf(stdout, "\nError executing child process: %s\n\n", strerror(errno)); // there was an error, print errno, which is also set by execvp to indicate the kind of error
					exit(0); // does not exit shell, only exits child process
				}
			} else { // Parent Process w/ NO background execution
				if(!(strcmp(argv[*argc-1], "&") == 0)) waitpid(pid, &status, 0); // so long as last character is not &, don't wait for execution
				//wait(&status);
				//waitpid(pid, &status, 0);
				fprintf(stdout, "\nExecuting external command: \"%s\" may or may not have been successful.. do you see an error reported directly above this line? Or what you would expect from an external command such as \"ls\"? If you see nothing printed above, then you may have used a command like \"mkdir\" or \"touch\", in those cases check your current directory for the new directories/files you may have created :)\n", argv[0]); // (FOR TESTING)
				return 1; // executing external command was successful
			}
		} else {
			puts("Forking error!");
		}
	}


	/* Otherwise, assume program to be a program in the system */
	pid_t pid = fork(); // fork current process
	int status = 0; // set int status to wait for child process to complete
	if (pid >= 0) { // if no error in forking
		if (pid == 0) { // Child Process
			// is redirection?
			// is piping?
			
			if (execvp(argv[0], argv) < 0) { // is normal command, if the execvp call returns anything less than 0
				fprintf(stdout, "\nError executing child process: %s\n", strerror(errno)); // there was an error, print errno, which is also set by execvp to indicate the kind of error
				fprintf(stdout, "No recognizable command was found..reenter a proper external or internal command - type \"help\" for more information on proper commands.\n\n");
				exit(0); // does not exit shell, only exits child process
			}
		} else { // Parent Process w/ NO background execution
			if(!(strcmp(argv[*argc-1], "&") == 0)) waitpid(pid, &status, 0); // as long as last character is not &, wait for child process to finish
			fprintf(stdout, "\nExecuting external command: \"%s\" may or may not have been successful.. do you see an error reported directly above this line? Or what you would expect from an external command such as \"ls\"? If you see nothing printed above, then you may have used a command like \"mkdir\" or \"touch\", in those cases check your current directory for the new directories/files you may have created :)\n", argv[0]); // (FOR TESTING)
			return 1; // executing external command was successful
		}
	} else {
		fprintf(stdout, "FORKING ERROR!\n");
		return 0; // for failure
	}

	return 0; // return failure if something above does not return 1
}

/* Internal Commands */
int cdTo(char **argv) {
	uid_t uid = getuid(); // returns the real user ID of the calling process, that "real user ID is that of the user who has invoked the program." straight from the man page 'man getuid'
    struct passwd* pwd = getpwuid(uid); // getpwuid (as per the man page) "searches the password database for the given login name, useruid, or user uuid". It stores the results of the search in a pwd structure to be used. While this is more inefficient than just chdir(getenv("HOME")), I couldn't get that to work so this is the alternative

	if (argv == NULL) { // if argv is null
		fprintf(stderr, "\nError: cd failed..\n"); // print an error
	}
	//char* env = getenv("HOME");
	if (argv[1] == '\0') { // if no second argument and just cd was entered
		fprintf(stdout, "ONLY cd entered, attempting to switch to home directory..\n");
		if (!(chdir(pwd->pw_dir))) { // switch to home directory using environment variable HOME
			if (getcwd(cwd, sizeof(cwd)) != NULL) { // print pathname of current directory (NOT A TEST ACTUALLY NEED AS PART OF PROGRAM SO DON'T MACRO OUT)
				printf("\nGOT CWD IN CD FUNCTION\n"); // print pathname of current directory
			}
			fprintf(stdout, "\nSuccessful cd to HOME directory: %s\n\n", cwd);
			return 1; // return 1 for successful cd to HOME
		} else {
			fprintf(stderr, "cd: That directory does not exist..\n"); // print that directory does not exist - also means chdir couldn't chdir properly
			return 1; // return 1 so as not to end shell program
		}
	}

	if (!(chdir(argv[1]))) { // chdir returned 0, successful cd
		if (getcwd(cwd, sizeof(cwd)) != NULL) { // print pathname of current directory (NOT A TEST ACTUALLY NEED AS PART OF PROGRAM SO DON'T MACRO OUT)
			printf("\nGOT CWD IN CD FUNCTION\n"); // print pathname of current directory
		}
		fprintf(stdout, "\nSuccessful cd to: %s\n\n", cwd); //print success
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

	printf("\n"); // print newline before printing directory contents for readability purposes
	while ((s = readdir(dir)) != NULL) { // readdir() returns pointer to next directory entry until it reaches end of the directory(NULL) or an error
		fprintf(stdout, "%s\t", s->d_name); // print the directory/file name in the current directory
	}
	printf("\n\n"); // print new lines after printing out contents of current directory for readability

	closedir(dir); // close dir
	return 1; // return 1 for success
}

/* Prints all environment variables to stdout/Also supports output redirection! */
int printEnvStrings() {
	int i = 0;
	while(environ[i] != NULL) {
		fprintf(stdout, "\n%s", environ[i]);
		i++;
	}
	printf("\n\n"); // print new line for readability
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
int help(char** commandToGetInfoAbout, int *argc) {
	if (*argc == 1) { // if argc is only one, print the user manual (the readme.txt)
		FILE *readMeFile_ptr;
		readMeFile_ptr = fopen("readme.txt", "r");
		if (readMeFile_ptr == NULL) { // error check that the file actually opened
			printf("Failed to open readme.txt for help!\n");
		}

		// print the contents of readme.txt to screen
		printf("\n\n"); // print newlines before reading contents of file for readability
		char c;
		c = fgetc(readMeFile_ptr);  // get first char in file
		while(c != EOF) { // while c is not EOF
			printf("%c", c);  // print each char from file
			c = fgetc(readMeFile_ptr);  // get next char to be read from file
		} 
		printf("\n\n\n"); // print newlines after reading contents of file for readability
		fclose(readMeFile_ptr); // be sure to close the readme.txt file
		return 1;
	} else { // otherwise look for what internal command the user needs help with and print the info they need
		int i = 0; // index of command to get info about after command of help
		while (commandToGetInfoAbout[i] != '\0') {
			if (strcmp(commandToGetInfoAbout[i], "\0") == 0) { // if no command specified to get info about
				printf("Time to print readme.txt!\n");
				// print readme.txt which is the user manual
				//FILE *readMe_ptr = fopen("readme.txt", "r");
				// allocate buffer for reading
				// read file with fgets, go until EOF reached, print all to stdout
				// close file
				return 1;
			} else if (strcmp(commandToGetInfoAbout[i], "cd") == 0) { // if command to get info about is cd
				// print a line about cd to the screen
				printf("\n\"cd\" will change you into a new directory. Type 'cd dirName' to change into that directory. Or type just cd to change to the HOME directory.\n");
				return 1; // for success
			} else if (strcmp(commandToGetInfoAbout[i], "clear") == 0) { // if command to get info about is clear
				// print a line about clear to the screen
				fprintf(stdout, "\n\"clear\" will clear the screen of the shell. Type 'clear' to do this.\n");
				return 1; // for success
			} else if (strcmp(commandToGetInfoAbout[i], "dir") == 0) { // if command to get info about is dir
				// print a line about dir to the screen
				fprintf(stdout, "\n\"dir\" will print out the contents of the current director you're in. Type 'dir' to list all the f iles and other directories within your current directory.\n");
				return 1; // for success
			} else if (strcmp(commandToGetInfoAbout[i], "env") == 0) { // if command to get info about is env
				// print a line about env to the screen
				fprintf(stdout, "\n\"env\" will print out all of your system's enviornment variables. Type 'env' to print them all out to screen.\n");
				return 1; // for success
			} else if (strcmp(commandToGetInfoAbout[i], "echo") == 0) { // if command to get info about is echo
				// print a line about echo to the screen
				fprintf(stdout, "\n\"echo\" will print back out to screen whatever you type after it. Type 'echo sometext' to print 'sometext' to the screen on a newline.\n");
				return 1; // for success
			} else if (strcmp(commandToGetInfoAbout[i], "pause") == 0) { // if command to get info about is pause
				// print a line about pause to the screen
				fprintf(stdout, "\n\"pause\" will pause the shell's execution until Enter is pressed. Type 'pause' to pause the shell at any time.\n");
				return 1; // for success
			} else if (strcmp(commandToGetInfoAbout[i], "quit") == 0) { // if command to get info about is quit
				// print a line about quit to the screen
				fprintf(stdout, "\n\"quit\" will quit the shell entirely. Type 'quit' to do so.\n");
				return 1; // for success
			}
			i++;
		}
	}
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
	printf("Shutting down dillon's shell..\n"); // (FOR TESTING)
	return 0;
}