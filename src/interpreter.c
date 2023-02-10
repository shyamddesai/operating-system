#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "shellmemory.h"
#include "shell.h"

int MAX_ARGS_SIZE = 7;

int badcommand() {
	printf("%s\n", "Unknown Command");
	return 1;
}

// helper function to print too many tokens in set command
int tooManyTokens() {
	printf("%s\n", "Bad command: Too many tokens");
	return 2;
}

int badTokenNotAlphaNumeric() {
	printf("%s\n", "Bad command: Token is not alphanumeric");
	return 4;
}

int isAlphaNumeric(char *var) {
	if (strlen(var) == 0)
		return badTokenNotAlphaNumeric();

	for (int i = 0; i < strlen(var); i++) {
		if (!isalnum(var[i])) {
			return badTokenNotAlphaNumeric();
		}
	}

	return 0;
}

// For run command only
int badcommandFileDoesNotExist() {
	printf("%s\n", "Bad command: File not found");
	return 3;
}

char *concatenateTokens(char *command_args[], int args_size) {

	int token_size = 0;
	for (int i = 2; i < args_size; i++) {
		token_size = token_size + strlen(command_args[i]) + 1; //+1 addresses the number of tokens for the spacing
	}

	char *tokens = (char *)malloc(sizeof(char) * token_size); // dynamic allocation based on user input; token_size + 1 due to NULL character
	for (int i = 0; i < token_size; i++) 
		tokens[i] = '\0'; // reinitialize all values of tokens to NULL characters to prevent garbage values when exiting batch mode and entering interactive mode

	// concatenate all STRING/tokens into one
	for (int i = 2; i < args_size; i++) {
		strcat(tokens, command_args[i]);
		strcat(tokens, " ");
	}
	tokens[strlen(tokens) - 1] = '\0'; // replace last space character with NULL character

	return tokens;
}

int help();
int quit();
int set(char *var, char *value);
int echo(char *var);
int print(char *var);
int run(char *script);
int my_mkdir(char* dirname);
int my_ls();
int my_cd(const char *dirname);
int my_touch(char *filename);
int badcommandFileDoesNotExist();

// Interpret commands and their arguments
int interpreter(char *command_args[], int args_size) {

	if (args_size < 1)	{
		return badcommand();
	}

	if(args_size > MAX_ARGS_SIZE) {
		return tooManyTokens();
	}

	for (int i = 0; i < args_size; i++)	{ // strip spaces new line etc
		command_args[i][strcspn(command_args[i], "\r\n")] = 0;
	}

	if (strcmp(command_args[0], "help") == 0)	{
		// help
		if (args_size != 1)
			return badcommand();
		return help();
	}	else if (strcmp(command_args[0], "quit") == 0)	{
		// quit
		if (args_size != 1)
			return badcommand();
		return quit();
	}	else if (strcmp(command_args[0], "set") == 0)	{
		// set
		if (args_size < 3)
			return badcommand();
		if (args_size > 7)
			return tooManyTokens();
		if (isAlphaNumeric(command_args[1]) == 4)
			return 4;
		return set(command_args[1], concatenateTokens(command_args, args_size));
	}	else if (strcmp(command_args[0], "print") == 0)	{
		//print
		if (args_size != 2)
			return badcommand();
		return print(command_args[1]);
	}	else if (strcmp(command_args[0], "echo") == 0)	{
		//echo
		if (args_size != 2)
			return badcommand();
		return echo(command_args[1]);
	}	else if (strcmp(command_args[0], "run") == 0)	{
		//run
		if (args_size != 2)
			return badcommand();
		return run(command_args[1]);
	} else if (strcmp(command_args[0], "my_mkdir") == 0) {
		//my_mkdir
	    if (args_size != 2) return badcommand();
		return my_mkdir(command_args[1]);
	} else if (strcmp(command_args[0], "my_cd") == 0) {
		//my_cd
		if (args_size != 2) return badcommand();
		return my_cd(command_args[1]);
	} else if (strcmp(command_args[0], "my_ls") == 0) {
		//my_ls
		if (args_size != 1) return badcommand();
		return my_ls();
	} else if (strcmp(command_args[0], "my_touch") == 0) {
		//my_touch
		if (args_size != 2) return badcommand();
		return my_touch(command_args[1]);
	
	} else return badcommand();
}

int help()	{

	char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
run SCRIPT.TXT		Executes the file SCRIPT.TXT\n ";
	printf("%s\n", help_string);
	return 0;
}

int quit()	{
	printf("%s\n", "Bye!");
	exit(0);
}

int set(char *var, char *value)	{
	char *link = "=";
	char buffer[1000];
	strcpy(buffer, var);
	strcat(buffer, link);
	strcat(buffer, value);
	mem_set_value(var, value);

	return 0;
}

int echo(char *var)	{
	if (var[0] != '$')	{
		printf("%s\n", var);
	} else	{
		char *value_to_echo = (char *)malloc(sizeof(char) * strlen(var));
		for (int i = 1; i < strlen(var); i++)		{
			value_to_echo[i - 1] = var[i];
		}
		value_to_echo[strlen(var) - 1] = '\0';

		if (strcmp(mem_get_value(value_to_echo), "Variable does not exist") == 0)
			printf("\n");
		else
			printf("%s\n", mem_get_value(value_to_echo));
	}

	return 0;
}

int print(char *var)	{
	printf("%s\n", mem_get_value(var));
	return 0;
}

int run(char* script)	{
	
	int errCode = 0;
	char line[1000];
	FILE *p = fopen(script, "rt"); // the program is in a file

	if (p == NULL)	{
		return badcommandFileDoesNotExist();
	}

	fgets(line, 999, p);

	while (1) {
		errCode = parseInput(line); // which calls interpreter()
		memset(line, 0, sizeof(line));

		if (feof(p)) {
			break;
		}
		fgets(line, 999, p);
	}

	fclose(p);

	return errCode;
}
int is_alphanumeric(char* str) {

 //testing to see if the string is alphanumeric
 //return 1 if it is, 0 if not alphanumeric
	for (int i = 0; i < strlen(str); i++) { 
		if (str[i] < 123) {
			return 1;
		}
	}
	
	return 0;
}

int my_mkdir(char* dirname) {

	if (dirname[0] == '$') {
		dirname++; //increment pointer to ignore $
		char* dirnameToMake = mem_get_value(dirname);

		//check if one token
		for (int i=0; dirnameToMake[i] != '\0'; ++i) {
        	if (dirnameToMake[i] == ' ') {
				printf("Bad command: my_mkdir\n");
				return 1;
			}
		}

		if(strcmp(dirnameToMake, "Variable does not exist") == 0) {
			printf("Bad command: my_mkdir\n");
			return 1;
		}
		
		mkdir(dirnameToMake, 0775);
		return 0;
  	}

	if (is_alphanumeric(dirname) == 1) {
		mkdir(dirname, 0775);
		return 0;
	}  else {
		printf("Did not pass the alphanumeric test\n");
		return 1;
	}

	if (mkdir(dirname, 0775)) { //check last since mkdir is called in if statement
		printf("Error: Directory already exists\n");
		return 1;
	}

	return 0;
} 


int my_ls() {

	DIR* dir = opendir(".");
	struct dirent *pDirent;

	int ls_count = 0;
	while ((pDirent = readdir(dir)) != NULL) {
		ls_count++;
	}

	closedir (dir);
	dir = opendir(".");

	ls_count -= 2; //remove . and ..

	char ls_filenames [ls_count][100]; //dynamic allocation
	int i = 0;

	while ((pDirent = readdir(dir)) != NULL) {
		if(strcmp(pDirent->d_name, ".") != 0 && strcmp(pDirent->d_name, "..") != 0) {
			strcpy(ls_filenames[i++], pDirent->d_name);
		} 
	}

	//sort ls filenames
	char temp[100];
	for (int i=0; i<ls_count; i++) {
		for (int j=0; j<ls_count-i-1; j++) {
			if(strcmp(ls_filenames[j], ls_filenames[j+1]) > 0) {
				strcpy(temp, ls_filenames[j]);
				strcpy(ls_filenames[j], ls_filenames[j+1]);
				strcpy(ls_filenames[j+1], temp);
			}
		}
	}

	for(int j=0; j<ls_count; j++) {
		printf("%s\n", ls_filenames[j]);
	}

  return(0);  
}

//change this, some methods are not in this library
int my_cd(const char *dirname) {
	char *result = getenv("PWD");
	if (result == NULL) {
		perror("getenv");
		return 1;
	}
  
	char path[strlen(result) + strlen(dirname) + 2];
	strcpy(path, result);
	strcat(path, "/");
	strcat(path, dirname);

	if (chdir(path) != 0) {
		printf("Bad command: my_cd\n");
	}
}

int my_touch(char *filename) {
	int fd = creat(filename, 0666);
	return 0;
}
