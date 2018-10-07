#include "myshell.h"

/* Parse command read in from user, increment argc for each string in command */
char** parseCommand(char* command, char** argv, int *argc) {       
	int i = 0;
	*(argc) = 0; // make sure for each command, argc is set back to 0
    for (i = 0; i < 100; i++) {
        argv[i] = strtok_r(command, " \t\r\a", &command); // seperate each string by space and store that string in the respective index of params C-String array, nice thing about strtok_r is that it removes empty spaces all together and all other delimiters
		
        if (argv[i] == NULL) break;
		(*argc)++; // increment argc for every string in the command
    }
	argv[strlen(*argv) + 1] = NULL; // set first location in array after C-Strings to NULL (necessary for passing argv array to execvp() system call)
	
	/* Shows attempt to parse out unecessary string from array of C-strings - see README for more info and down below for deleteSpecialOperatorsFunction*/
	// i = 0;
	// while(argv[i] != NULL) { 
	// 	if (strcmp(argv[i], ">") == 0) {
	// 		argv = deleteSpecialOperators(argv, ">");
	// 	}
	// 	i++;
	// }
	
	return argv; // return argv array to be executed on
	// i = 0;
	// while (argv[i] != NULL) {
	// 	printf("argv in parser: \"%s\" with an i = %d\n", argv[i], i);
	// 	if (strcmp(argv[i], "&") == 0) { // if & character found for background execution
	// 		printf("& FOUND!\n"); 
	// 		// flip global variable for background to true (1)
	// 		argv[i] = NULL; // & found, set it to NULL to parse it out
	// 		i++;
	// 		(*argc)--; // decrease argc by one since & should not count as an argument to be executed on
	// 		continue;
	// 	}
	// 	if (strcmp(argv[i], "|") == 0) { // if | character found for piping
	// 		printf("| FOUND!\n"); 
	// 		// flip global variable for background to true (1)
	// 		argv[i] = NULL; // | found, set it to NULL to parse it out
	// 		(*argc)--; // decrease argc by one since | should not count as an argument to be executed on
	// 	}
    // 	if (strcmp(argv[i], "<") == 0) { // if & character found for input redirection
	// 		printf("< FOUND!\n"); 
	// 		// flip global variable for input redirection to true (1)
	// 		argv[i] = NULL; // < found, set it to NULL to parse it out
	// 		(*argc)--; // decrease argc by one since < should not count as an argument to be executed on
	// 	}
    // 	if (strcmp(argv[i], ">") == 0) { // if > character found for output redirection - OVERWRITE case
	// 		printf("> FOUND!\n"); 
	// 		// flip global variable for output redirection - OVERWRITE to true (1)
	// 		argv[i] = NULL; // > found, set it to NULL to parse it out
	// 		(*argc)--; // decrease argc by one since > should not count as an argument to be executed on
	// 	}
    // 	if (strcmp(argv[i], ">>") == 0) { // if >> characters found for output redirection - APPEND case
	// 		printf(">> FOUND!\n"); 
	// 		// flip global variable for output redirection - APPEND to true (1)
	// 		argv[i] = NULL; // >> found, set it to NULL to parse it out
	// 		(*argc)--; // decrease argc by one since >> should not count as an argument to be executed on
	// 	}
	// 	i++;
	// }
}

// char** deleteSpecialOperators(char **text, const char *word)
// {
//     char **dst = text, **src = text, **res = text;
//     size_t size = 1, deleted = 0;

//     // loop while we have a non-null string that isn't empty
//     while (*src && (*src)[0])
//     {
//         char *tmp = strdup(*src);
//         if (tmp == NULL)
//         {
//             perror("Failed to allocate tmp");
//             exit(EXIT_FAILURE);
//         }

//         char *token = strtok(tmp, word);

//         // search for matching word
//         while (token && strcmp(word, token))
//             token = strtok(NULL, word);

//         // if not found, keep the string. otherwise delete it.
//         if (!token)
//         {
//             *dst++ = *src++;
//             size++;
//         }
//         else
//         {
//             free(*src++);
//             ++deleted;
//         }
//     }

//     // resize the original array (which could have only gotten smaller)
//     if (deleted > 0)
//     {
//         res = realloc(text, size * sizeof(*res));
//         if (res == NULL)
//         {
//             perror("Failed to allocate res");
//             exit(EXIT_FAILURE);
//         }
//         res[size-1] = *src;
//     }

//     return res;
// }