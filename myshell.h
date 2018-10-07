/*
    Header file for all included libraries, function prototypes, and global variables
*/
#ifndef parser
    #include <stdio.h> // for fopen, fclose, fgets
    #include <stdlib.h> // malloc, realloc, free
    #include <string.h> // for strsep, strcmp and other string functions
    #include <errno.h> // for errno which is set by execvp upon failure
    #include <unistd.h> // for fork and exec
    #include <sys/types.h>  // for pid_t used in keeping track of process ids when executing external commands
    #include <sys/wait.h> // for wait
    #include <sys/stat.h> // for stat function, to see if file exists in a directory to be executed on
    #include <fcntl.h> // for O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO when getting file descriptors for input/output redirection
    #include <dirent.h> // for chdir and opendir
    #include <pwd.h> // for cd'ing into home directory

    extern char **environ; // for printing env strings/accessing dirs
    extern int errno; // for error handling, to be used in conjunction with strerror()
    char cwd[100]; // used for printing current working directory after each command (cd'ing to new dir etc.), global so that cdTo function can access and change it's value as well as in the main while loop where it is printed

    /* function prototypes */
    char** parseCommand(char* command, char** params, int *argc);
    int executeCommand(char** argv, int *argc);
    int cdTo(char** argv);
    int clrScreen();
    int printDirContents();
    int printEnvStrings();
    int echoUserInput(char** input);
    int help(char** commandToGetInfoAbout, int *argc);
    int pauseShell();
    int quitTerm();
    char** deleteSentences(char **text, const char *word);
#endif