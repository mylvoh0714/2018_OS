#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#define STRTOK_DELIM " \t\r\n\a"

const int STRTOK_BUFSIZE=64; // When strtok, this is for maximum buffer size
							 // if string exceeds, then realloc buffer to +=BUFSIZE

char *ReadLine(void) // ReadLine function is simple
{	                 // because getline() resizes buffer for itself
					 // see "http://linux.die.net/man/3/getline"
	char *line = NULL;
	size_t buffersize = 0;
	if(getline(&line, &buffersize, stdin)==EOF) // this is for Ctrl+D Command(=quit Command)
	{
		return "\0";	
	}
	return line;
}

char **SplitSemicol(char *line) // Split line into multiple commands tokenizing semicolon.
{	
	int buffersize = STRTOK_BUFSIZE;
	int pos = 0;
	char **tokens = malloc(buffersize * sizeof(char*));
	char *token;

	if(!tokens) {
		fprintf(stderr, "error detected!! \n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, ";");
	while(token != NULL) {
		tokens[pos] = token;
		pos++;

		if (pos >= buffersize) { // when (input size > buffer size) : buffer size +=; by realloc function
			buffersize += STRTOK_BUFSIZE;
			tokens = realloc(tokens, buffersize * sizeof(char*));
			if (!tokens) {
				fprintf(stderr, "error detected!! \n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, ";");
	}
	tokens[pos] = NULL;
	return tokens;
}

char **SplitLine(char *line) // Split each command line into word, i.e tokenizing blank.
{	
	int buffersize = STRTOK_BUFSIZE;
	int pos = 0;
	char **tokens = malloc(buffersize * sizeof(char*));
	char *token;

	if(!tokens) {
		fprintf(stderr, "error detected!! \n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, STRTOK_DELIM);
	while(token != NULL) {
		tokens[pos] = token;
		pos++;

		if (pos >= buffersize) { // when (input size > buffer size) : buffer size +=; by realloc function
			buffersize += STRTOK_BUFSIZE;
			tokens = realloc(tokens, buffersize * sizeof(char*));
			if (!tokens) {
				fprintf(stderr, "error detected!! \n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, STRTOK_DELIM);
	}
	tokens[pos] = NULL;
	return tokens;
}

int Execute(char **args) // the parameter args is array of char[]
{                        // which consist of one command.
	pid_t pid;           // ex) if command is  "grep main.c" , then args = ["grep","main.c"]
	int status;

	if (args[0] == NULL) { // No command input
		return 1;
	}

	if (strcmp(args[0], "quit") == 0) { // "quit" command
		return 0;
	}

	pid = fork();
	if (pid == 0) {  // Child PID
		if (execvp(args[0], args) == -1) { // make child process and check error 
			perror("Shell");
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0) {   // fork() error
		perror("Shell");
	} else {  // Parent PID
		do {
			waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1;
}

void BatchMode(void)
{
	char line[1024];     // The "line" variable's max buffer size is 1024.
	char **semicol_args; // i.e only can read sizeof(char)*1024 in each line of batch file.
	char **args;
	FILE *fp;

	fp = fopen("./batchfile","r");
	while( !feof(fp))
	{
		fgets(line,1024,fp);
		printf("%s",line);
		semicol_args = SplitSemicol(line); // "semicol_args" : array of multiple commands seperated by ';'
		for( int i=0; semicol_args[i]!=NULL; i++)
		{
			args = SplitLine(semicol_args[i]);  //  "args" : array of char[] which consist of one command
			Execute(args);
			free(args);
		}
		free(semicol_args);
	}
}

void InteractiveMode(void)
{
	char *line;
	char **semicol_args;
	char **args;
	int status;

	do {
		printf("prompt> ");
		line = ReadLine();
		if(strcmp(line,"\0") == 0) { // Ctrl+D command input
			break;	
		}
		semicol_args = SplitSemicol(line); // "semicol_args"" : array of multiple commands seperated by ';'
		for(int i=0; semicol_args[i]!=NULL; i++)
		{
			args = SplitLine(semicol_args[i]);  //  "args" : array of char[] which consist of one command
			status = Execute(args);		// "status" variable is made for "quit" command
			free(args);
		}
		free(semicol_args);
		free(line);
	} while(status);
}


int main(int argc, char **argv)
{
	if(argc == 1) {
		InteractiveMode();
	} else if(argc == 2) {
		BatchMode();
	} else {
		printf("Error!! : \"./shell\",\"./shell file\" format input only\n");
	}
	return 0;
}
