#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "shellmemory.h"
#include "shell.h"

int MAX_ARGS_SIZE = 7;

int badcommand(){
	printf("%s\n", "Unknown Command");
	return 1;
}

//helper function to print too many tokens in set command
int tooManyTokens(){
	printf("%s\n", "Bad command: Too many tokens");
	return 1;
}

// For run command only
int badcommandFileDoesNotExist(){
	printf("%s\n", "Bad command: File not found");
	return 3;
}

int help();
int quit();
int set(char* var, char* value);
int print(char* var);
int run(char* script);
int badcommandFileDoesNotExist();

// Interpret commands and their arguments
int interpreter(char* command_args[], int args_size){
	int i;

	//if ( args_size < 1 || args_size > MAX_ARGS_SIZE){ //remove check for max arg size and deal with it manually instead
	if ( args_size < 1){
		return badcommand();
	}

	for ( i=0; i<args_size; i++){ //strip spaces new line etc
		command_args[i][strcspn(command_args[i], "\r\n")] = 0;
	}

	if (strcmp(command_args[0], "help")==0){
	    //help
	    if (args_size != 1) return badcommand();
	    return help();
	
	} else if (strcmp(command_args[0], "quit")==0) {
		//quit
		if (args_size != 1) return badcommand();
		return quit();

	} else if (strcmp(command_args[0], "set")==0) {
		//set
		if (args_size < 3) return badcommand();
		if(args_size > 7) return tooManyTokens();	

		int token_size = 0;
		for(int i=2; i<args_size; i++) {
			token_size = token_size + strlen(command_args[i]) + 1; //+1 addresses the number of tokens for the spacing
		}

		char* tokens = (char*) malloc(sizeof(char)*token_size); //dynamic allocation based on user input; token_size + 1 due to NULL character

		//concatenate all STRING/tokens into one
		for (int i=2; i<args_size; i++) {
			strcat(tokens, command_args[i]);
			strcat(tokens, " ");
		} tokens[strlen(tokens)] = '\0'; //replace last space character with NULL character

		return set(command_args[1], tokens);

	} else if (strcmp(command_args[0], "print")==0) {
		if (args_size != 2) return badcommand();
		return print(command_args[1]);
	
	} else if (strcmp(command_args[0], "run")==0) {
		if (args_size != 2) return badcommand();
		return run(command_args[1]);

	} else if (strcmp(command_args[0], "my_mkdir")==0) {
	    if (args_size != 2) return badcommand();
		return my_mkdir(command_args[1]);

	} else if (strcmp(command_args[0], "my_cd")==0) {
		if (args_size != 2) return badcommand();
		return my_cd(command_args[1]);

	} else if (strcmp(command_args[0], "my_ls")==0) {
		if (args_size != 1) return badcommand();
		return my_ls();
	} else if (strcmp(command_args[0], "my_touch")==0) {
		if (args_size != 2) return badcommand();
		return my_touch(command_args[1]);
	
	} else return badcommand();
}

int help(){

	char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
run SCRIPT.TXT		Executes the file SCRIPT.TXT\n ";
	printf("%s\n", help_string);
	return 0;
}

int quit(){
	printf("%s\n", "Bye!");
	exit(0);
}

int set(char* var, char* value){
	char *link = "=";
	char buffer[1000];
	strcpy(buffer, var);
	strcat(buffer, link);
	strcat(buffer, value);

	mem_set_value(var, value);

	return 0;
}

int print(char* var){
	printf("%s\n", mem_get_value(var)); 
	return 0;
}

int run(char* script){
	
	int errCode = 0;
	char line[1000];
	FILE *p = fopen(script, "rt"); // the program is in a file

	if (p == NULL){
		printf("P is null\n");
		return badcommandFileDoesNotExist();
	}

	fgets(line, 999, p);

	while (1)
	{
		errCode = parseInput(line); // which calls interpreter()
		memset(line, 0, sizeof(line));

		if (feof(p))
		{
			break;
		}
		fgets(line, 999, p);
	}

	fclose(p);

	return errCode;
}
int is_alphanumeric(char* str) {
  
  int i;
 
 //testing to see if the string is alphanumeric
 //return 1 if it is, 0 if not alphanumeric

 for (i = 0; i < strlen(str); i++) { 
    if (str[i] < 123) {
      return 1;
    }
  }
  return 0;
}

int my_mkdir(char* dirname) {
  //if (dirname[0] == '$') {
    
    int i;
	int newdirectory;
	
	// Check if the variable exists in the shell memory
    // If the variable exists, create the directory using the value associated with it
    
	if (mkdir(dirname, 0775)) {
		printf("Error: Directory already exists\n");
		return 1;
	}
      
	  if (is_alphanumeric(dirname) == 1) {
		mkdir(dirname, 0775); 
		printf("alphanumeric test passed\n");                             //find the pathway of the directory to do this	
    }  
	  else {
        // If the value is not a single alphanumeric token, display an error message
        printf("did not pass the alphanumeric test\n");
		}
		return 0;
    } 


int my_ls() {
/*  int count =0;
  int i = 0;
  int entries[100];
  
 // *mem_get_value(char *var_in);
  
  for (i; i < count; i++) {
    if (entries[i].d_name) {
        printf("%s/\n", entries[i].d_name);
      }
    }
	//qsort() sort the list

	for(k=0; k< count; k++);{
		//print all of the elements in the list
	}
	return 0;

	//add a sorting method
	//find a way to iterate through the directory
	// add all the names of the directories in an array and sort that array*/
  int d = opendir(".");
  
  if (d) {
    while ((readdir(d)) != NULL) {
      printf("%s\n", d);
    }
    closedir(d);
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

int my_touch(char *filename){
	int fd = creat(filename, 0666);
	return 0;
}
