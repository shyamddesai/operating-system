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
	FILE *p = fopen(script,"rt");  // the program is in a file

	if(p == NULL){
		return badcommandFileDoesNotExist();
	}

	fgets(line,999,p);
	while(1){
		errCode = parseInput(line);	// which calls interpreter()
		memset(line, 0, sizeof(line));

		if(feof(p)){
			break;
		}
		fgets(line,999,p);
	}

    fclose(p);

	return errCode;
}
int create_directory(char* dirname) {
    if (mkdir(dirname) == -1) {
        printf("Error creating directory %s\n", dirname);
		return -1;
    } else {
		mkdir(dirname);
        printf("Directory %s created\n", dirname);
		return 0;
    }
}

int my_mkdir(char* dirname) {
    if (dirname[0] == '$') {
        char* env_var = getenv(dirname + 1);
        if (env_var == NULL) {
            printf("Bad command: my_mkdir\n");
        } else {
            create_directory(env_var);
        }
    } else {
        create_directory(dirname);
    }
	return 0;
}

void my_ls() {
  
  int count, i;

  count = 0;
  while ((entry = readdir(dir))) {
    entries[count++] = entry;
  }
  closedir(dir);

  for (i = 0; i < count; i++) {
    if (strcat(entries[i].d_name, &buf) == 0) {
      if (S_ISDIR(buf.st_mode)) {
        printf("%s/\n", entries[i].d_name);
      } else {
        printf("%s\n", entries[i].d_name);
      }
    }
  }
}

void my_cd(const char *dirname) {
  char *result = getenv("PWD");
  if (result == NULL) {
    perror("getenv");
    return;
  }
  
  char path[strlen(result) + strlen(dirname) + 2];
  strcpy(path, result);
  strcat(path, "/");
  strcat(path, dirname);

  if (chdir(path) != 0) {
    printf("Bad command: my_cd\n");
  }
}
