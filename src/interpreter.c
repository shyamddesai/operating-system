#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include "shellmemory.h"
#include "shell.h"
#include "pcb.h"

#define MAX_LINE_LENGTH 1000

int MAX_ARGS_SIZE = 8;
char **file_names;
int num_files;
int current_file = 0;
pthread_mutex_t mutex;

int badcommand() {
	printf("%s\n", "Unknown Command");
	return 1;
}
// helper function to print too many tokens in set command

int tooManyTokens() {
	printf("%s\n", "Bad command: Too many tokens");
	return 2;
}

int sameFileName() {
	printf("%s\n", "Bad command: same file name");
	return 5;
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
int exec(char prog[], int length);
//int my_mkdir(char* dirname);
int my_ls();
int my_cd(const char *dirname);
int my_touch(char *filename);
int badcommandFileDoesNotExist();
void* read_file(void* filename);

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
		//return my_mkdir(command_args[1]);
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
	} else if (strcmp(command_args[0], "exec") == 0) {
		//exec
		if (args_size < 3) return badcommand();
		if (args_size > 8) return tooManyTokens(); //increased for # and MT
		
		//Check for FileNameTest
		for(int i=1; i<args_size-1; i++) {
			for(int j=0; j<args_size-i-1; j++) {
				if(i+j+1 != args_size-1) {
					if(strcmp(command_args[i], command_args[i+j+1]) == 0) return sameFileName();
				}
			}
		}

		char programs[1000]; //row = args_size-1 excluding exec 
		programs[0] = '\0';
		int len=0;
		
		for(int i=1; i<args_size; i++) {
			len += strlen(command_args[i]);
			len++; //increase len for spaces
			strcat(programs, command_args[i]); //copy the number of programs and policy
			strcat(programs," ");
		}
		programs[len-1] = '\0'; //append last space with NULL

		return exec (programs, args_size-1);
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
	if (p == NULL) return badcommandFileDoesNotExist();
	pcb_init();
	fgets(line, 999, p);
	while (1) {
		//printf("p: %s\n", line);
		pcb_set_script(line); //pass line to pcb
		if (feof(p)) break;
		fgets(line, 999, p);
	}
	insert_queue();
	display_queue();
	cleanup();
	//print_script_commands();
	fclose(p);
	return errCode;
}

int exec(char prog[], int length) {
	char programs[length][1000];
	int row = 0;
	int col = 0;
	int count_total = 0;
		
	for (int i=0; i<strlen(prog); i++) {
		if(prog[i] == ' ') {
			programs[row][col] = '\0'; //append NULL character to end of each row
			row++;
			col = 0;
		} else {
			programs[row][col++] = prog[i];
		}
	}
	
	int length_Files[row]; //store length of each file
		
	if(strcmp(programs[row],"MT")==0) { //Multithreading
		if(strcmp(programs[row-1],"#")==0) { //Multithreading w/Background
			row--;
		}

		if(strcmp(programs[row-1],"RR30")==0) { //Multithreading Background w/RR30
			row--;

			//initialize length of files
			for(int i=0; i<row; i++) {
				length_Files[i] = 0; 
			}
			
			for(int i=0; i<row; i++) {
				int errCode = 0;
				char line[1000];

				for(int j=0; j<1000; j++) {
					line[j] = '\0';
				} 

				FILE *p = fopen(programs[i], "rt"); // the program is in a file
				if (p == NULL)	return badcommandFileDoesNotExist();

				pcb_init();
				fgets(line, 999, p);
				length_Files[i]++; //increment length of each file
				count_total++;

				while (1) {
					if (feof(p)) break;
					fgets(line, 999, p);
					
					length_Files[i]++;
					count_total++; //count total number of commands from all files
				}
			
				fclose(p);
			}
			
			pthread_t threads[2];
			for (int i=0; i<2; i++) {
				int start = i*3;

				pthread_create(&threads[i], NULL, read_file, (void*) programs[start]);
				if (start+1 < row)	pthread_create(&threads[i], NULL, read_file, (void*) programs[start+1]);
				if (start+2 < row)	pthread_create(&threads[i], NULL, read_file, (void*) programs[start+2]);
			}

			for (int i=0; i<2; i++) {
				pthread_join(threads[i], NULL);
			}

			insert_queue();
			display_queue();
			cleanup();
			
			return 0;
		}
/*
		else if(strcmp(programs[row-1],"RR")==0)         //for round robin
		{
			
			for(int i=0; i<row; i++) 
			{
				//printf("--%s--\n", programs[i]);
				
				int errCode = 0;
				char line[1000];
				for(int j=0; j<1000; j++) {
					line[j] = '\0';
				} 
				FILE *p = fopen(programs[i], "rt"); // the program is in a file
				if (p == NULL)	return badcommandFileDoesNotExist();
				pcb_init();
				fgets(line, 999, p);
				length_Files[i]++; //increment length of each file
				while (1) {
					if (feof(p)) break;
					// memset(line, '\0', sizeof(line));
					fgets(line, 999, p);
					// printf("---%s-----\n",line);
					// if(strlen(line)==0) break;
					length_Files[i]++;
					count_total++; //count total number of commands from all files
				}
				
				fclose(p);
			}
			
			char all_commands[count_total][1000];
			int all_commands_counter=0;
			//int positions[count]; //holds indices of instructions needed for all files
			int counter=0; //index
			int file1=0;
			int file2=0;
			int file3=0;
			int i=0;
			int errCode = 0;
			char line[1000];
			FILE *p[row]; 
			
			
			for(int j=0; j<1000; j++) {
				line[j] = '\0';
			}
		
			for(int j=0; j<row; j++) {
				p[j] = fopen(programs[j], "rt"); //open upto 3 file pointers
				if (p[j] == NULL)	return badcommandFileDoesNotExist();
			}
			pcb_init();
			all_commands_counter=0;
			while(1) {
				switch (i) { //check 0th index
					case 0:
						for(int j=0; j<2; j++) { //check if two instructions exist in file
							if((length_Files[i]-file1)>0 && counter<row) { //check if lines left to execute, check if right number of files				
								if (feof(p[i])) break;
								fgets(line, 999, p[i]);
								strcpy(all_commands[all_commands_counter++],line); //pass upto two lines into temporary ready queue
								//pcb_set_script(line); //pass line to pcb
								
								file1++;
							}
						} 
						break;
					
					case 1:
						for(int j=0; j<2; j++) {
							if((length_Files[i]-file2)>0 && counter<row) { //check if lines left to execute, check if right number of files				
								if (feof(p[i])) break;
								fgets(line, 999, p[i]);
								strcpy(all_commands[all_commands_counter++],line); //pass upto two lines into temporary ready queue
								//pcb_set_script(line); //pass line to pcb
								
								file2++;
							}
						} 
						break;
					case 2:
						for(int j=0; j<2; j++) {
							if((length_Files[i]-file3)>0 && counter<row) { //check if lines left to execute, check if right number of files				
								if (feof(p[i])) break;
								fgets(line, 999, p[i]);
								strcpy(all_commands[all_commands_counter++],line); //pass upto two lines into temporary ready queue
								// pcb_set_script(line); //pass line to pcb
								
								file3++;
							}
						} 
						break;
				}
				i++;
				if(i>2) i=0; //restart from 0
				int flag = 0;
				for(int j=0; j<row; j++) {
					if(!feof(p[j])) flag = 1;
				}
				
				if(flag == 0) break;
			}
			// insert_queue();
			// display_queue();
			// cleanup();
			for(int j=0; j<row; j++) fclose(p[j]);
			//even elements
			// void *program1(void *arg) 
			// {
			// 	int i;
			// 	for (i = 0; i <count_total; i+=2) 
			// 	{
			// 		//printf("Program 1: %d %s\n", all_commands[i]);
			// 	}
			// 	pthread_exit(NULL);
			// }
			// //odd elements
			// void *program2(void *arg) 
			// {
			// 	int i;
			// 	for (i = 1; i < count_total; i+=2) 
			// 	{
			// 		//printf("Program 2: %d %s\n", i,all_commands[i]);
			// 	}
			// 	pthread_exit(NULL);
			// }
			// pthread_t thread1, thread2;
			// pthread_create(&thread1, NULL, program1, NULL);
			// pthread_create(&thread2, NULL, program2, NULL);
			// pthread_join(thread1, NULL);
			// pthread_join(thread2, NULL);	
		}
*/	
	
	} else if(strcmp(programs[row],"#")==0) { //Background Execution only
		if(strcmp(programs[row-1], "SJF") == 0) { //Background w/SJF
			row--;
				for(int i=1; i<row; i++) {
					for(int j=1; j<row-i-1; j++) {
						if(length_Files[j] > length_Files[j+1]) {
							char tempFiles[1000];
							strcpy(tempFiles, programs[j]);
							strcpy(programs[j], programs[j+1]);
							strcpy(programs[j+1], tempFiles);
							
							int tempLength = length_Files[j];
							length_Files[j] = length_Files[j+1];
							length_Files[j+1] = tempLength;
						}
					}
				}

			
			for(int i=0; i<row; i++) { //ignore policy while iterating
				int errCode = 0;
				char line[1000];

				for(int i=0; i<1000; i++) {
					line[i] = '\0';
				}

				FILE *p = fopen(programs[i], "rt"); // the program is in a file
				if (p == NULL)	return badcommandFileDoesNotExist();

				pcb_init();
				fgets(line, 999, p);

				while (1) {	
					pcb_set_script(line); //pass line to pcb
					memset(line, '\0', sizeof(line));

					if (feof(p)) break;
					fgets(line, 999, p);
					if(strlen(line)==0)	break;
				}

				insert_queue();
				display_queue();
				cleanup();
				
				fclose(p);
			}
		}
		
		if(strcmp(programs[row-1], "RR30") == 0) { //Background w/RR30
			row--;

			int positions[row]; //holds indices of instructions needed for all files
			int count = 0;
			int counter=0; //index
			int file1=0;
			int file2=0;
			int file3=0;
			int i=0;
			int errCode = 0;
			FILE *p[row]; 

			char line[1000];
			for(int j=0; j<1000; j++) {
				line[j] = '\0';
			}
		
			for(int j=0; j<row; j++) {
				p[j] = fopen(programs[j], "rt"); //open upto 3 file pointers
				if (p[j] == NULL)	return badcommandFileDoesNotExist();
			}

			pcb_init();
						
			while(1) {
				for(int j=0; j<30; j++) { //check if two instructions exist in file
					if (feof(p[i])) break;
					memset(line, '\0', sizeof(line));

					fgets(line, 999, p[i]);
					if(strlen(line)==0) break;
					
					pcb_set_script(line); //pass line to pcb
					file1++;
				}

				i++;
				
				if(i==row) break;

				int flag = 0;
				for(int j=0; j<row; j++) {
					if(!feof(p[j])) flag = 1;
				}
					
				if(flag == 0) break;
			}
			
			insert_queue();
			display_queue();
			cleanup();

			for(int j=0; j<row; j++) fclose(p[j]);
		}

		if(strcmp(programs[row-1],"RR")==0) { //Background w/RR
			row--;

			for(int i=0; i<row; i++) {
				int errCode = 0;

				char line[1000];
				for(int j=0; j<1000; j++) line[j] = '\0';
				
				FILE *p = fopen(programs[i], "rt"); // the program is in a file
				if (p == NULL)	return badcommandFileDoesNotExist();
				
				pcb_init();
				fgets(line, 999, p);
				length_Files[i]++; //increment length of each file
				
				while (1) {
					if (feof(p)) break;
					fgets(line, 999, p);
					length_Files[i]++;
				}
			
				fclose(p);
			}
			
			int positions[row]; //holds indices of instructions needed for all files
			int counter=0; //index
			int file1=0;
			int file2=0;
			int file3=0;
			int i=0;
			int errCode = 0;
			FILE *p[row]; 
			
			char line[1000];
			for(int j=0; j<1000; j++) line[j] = '\0';
	
			for(int j=0; j<row; j++) {
				p[j] = fopen(programs[j], "rt"); //open upto 3 file pointers
				if (p[j] == NULL) return badcommandFileDoesNotExist();
			}

			pcb_init();

			while(1) {
				switch (i) { //check 0th index
					case 0:
						for(int j=0; j<2; j++) { //check if two instructions exist in file
							if((length_Files[i]-file1)>0 && counter<row) { //check if lines left to execute, check if right number of files				
								if (feof(p[i])) break;
								fgets(line, 999, p[i]);
								pcb_set_script(line); //pass line to pcb
								
								file1++;
							}
						} 
						break;
					
					case 1:
						for(int j=0; j<2; j++) {
							if((length_Files[i]-file2)>0 && counter<row) { //check if lines left to execute, check if right number of files				
								if (feof(p[i])) break;
								fgets(line, 999, p[i]);
								pcb_set_script(line); //pass line to pcb
								
								file2++;
							}
						} 
						break;

					case 2:
						for(int j=0; j<2; j++) {
							if((length_Files[i]-file3)>0 && counter<row) { //check if lines left to execute, check if right number of files				
								if (feof(p[i])) break;
								fgets(line, 999, p[i]);
								pcb_set_script(line); //pass line to pcb
								
								file3++;
							}
						} 
						break;
				}

				i++;
				if(i>2) i=0; //restart from 0
				
				int flag = 0;
				for(int j=0; j<row; j++) {
					if(!feof(p[j])) flag = 1;
				}
				
				if(flag == 0) break;
			}

			insert_queue();
			display_queue();
			cleanup();
			
			for(int j=0; j<row; j++) fclose(p[j]);
		}
	} else	{ //CASE: NO BACKGROUND EXECUTION
		for(int i=0; i<row; i++) length_Files[i] = 0;
				
		for(int i=0; i<row; i++) {
			int errCode = 0;

			char line[1000];
			for(int j=0; j<1000; j++) line[j] = '\0';

			FILE *p = fopen(programs[i], "rt"); // the program is in a file
			if (p == NULL)	return badcommandFileDoesNotExist();
			
			pcb_init();
			fgets(line, 999, p);
			length_Files[i]++; //increment length of each file
			
			while (1) {
				if (feof(p)) break;
				memset(line, '\0', sizeof(line));
			
				fgets(line, 999, p);
				if(strlen(line) == 0) break;
				length_Files[i]++;
			}

			fclose(p);
		}

		if(strcmp(programs[row], "RR") == 0) { //No Background w/RR
			int positions[row]; //holds indices of instructions needed for all files
			int counter=0; //index
			int count = 0;
			int file1=0;
			int file2=0;
			int file3=0;
			int i=0;
			int errCode = 0;
			FILE *p[row]; 

			char line[1000];
			for(int j=0; j<1000; j++) line[j] = '\0';
		
			for(int j=0; j<row; j++) {
				p[j] = fopen(programs[j], "rt"); //open upto 3 file pointers
				if (p[j] == NULL)	return badcommandFileDoesNotExist();
			}

			pcb_init();
						
			while(1) {
				for(int j=0; j<2; j++) { //check if two instructions exist in file
					if (feof(p[i])) break;
					memset(line, '\0', sizeof(line));

					fgets(line, 999, p[i]);
					if(strlen(line)==0) break;
					
					pcb_set_script(line); //pass line to pcb
					file1++;
				}

				i++;
				if(i==row) i=0; //restart from 0
				
				int flag = 0;
				for(int j=0; j<row; j++) {
					if(!feof(p[j])) flag = 1;
				}
					
				if(flag == 0) break;
			}
			
			insert_queue();
			display_queue();
			cleanup();
			
			for(int j=0; j<row; j++) fclose(p[j]);
		}

		if(strcmp(programs[row], "RR30") == 0) { //No Background w/RR30
			int positions[row]; //holds indices of instructions needed for all files
			int counter=0; //index
			int count = 0;
			int file1=0;
			int file2=0;
			int file3=0;
			int i=0;
			int errCode = 0;
			FILE *p[row]; 

			char line[1000];
			for(int j=0; j<1000; j++) line[j] = '\0';
		
			for(int j=0; j<row; j++) {
				p[j] = fopen(programs[j], "rt"); //open upto 3 file pointers
				if (p[j] == NULL)	return badcommandFileDoesNotExist();
			}

			pcb_init();
			
			while(1) {
				for(int j=0; j<30; j++) { //check if two instructions exist in file
					if (feof(p[i])) break;
					memset(line, '\0', sizeof(line));

					fgets(line, 999, p[i]);
					if(strlen(line)==0) break;
					
					pcb_set_script(line); //pass line to pcb
					file1++;
				}

				i++;
				if(i==row) i=0; //restart from 0
				
				int flag = 0;
				for(int j=0; j<row; j++) {
					if(!feof(p[j])) flag = 1;
				}
					
				if(flag == 0) break;
			}
			
			insert_queue();
			display_queue();
			cleanup();

			for(int j=0; j<row; j++) fclose(p[j]);
		}

		if(strcmp(programs[row], "FCFS") == 0 || strcmp(programs[row], "SJF") == 0) { //No Background w/FCFS and SJF
			//Shortest Job First: swap contents and length of files into ascending order
			if(strcmp(programs[row], "SJF") == 0) {
				for(int i=0; i<row; i++) {
					for(int j=0; j<row-i-1; j++) {
						if(length_Files[j] > length_Files[j+1]) {
							char tempFiles[1000];
							strcpy(tempFiles, programs[j]);
							strcpy(programs[j], programs[j+1]);
							strcpy(programs[j+1], tempFiles);
							
							int tempLength = length_Files[j];
							length_Files[j] = length_Files[j+1];
							length_Files[j+1] = tempLength;
						}
					}
				}
			}
			
			//pass lines of code FCFS (even after sorting for SJF)
			for(int i=0; i<row; i++) { //ignore policy while iterating	
				int errCode = 0;

				char line[1000];
				for(int i=0; i<1000; i++) line[i] = '\0';

				FILE *p = fopen(programs[i], "rt"); // the program is in a file
				if (p == NULL)	return badcommandFileDoesNotExist();
				
				pcb_init();
				fgets(line, 999, p);
				
				while (1) {
					pcb_set_script(line); //pass line to pcb
					if (feof(p)) break;
					fgets(line, 999, p);
				}

				insert_queue();
				display_queue();
				cleanup();
				
				fclose(p);
			}
		}

		if(strcmp(programs[row], "AGING") == 0) { //No Background w/AGING
			struct queue {
				char fileName[1000];
				int length;
			} aging_queue[row]; //third array

			int toBeProcessed[row]; //first array
			FILE *p[row]; //second array
			int matchrow = row;

			char line[1000];
			for(int j=0; j<1000; j++) line[j] = '\0';

			//initiate aging_queue array struct with file names and length
			for(int i=0; i<row; i++) {
				strcpy(aging_queue[i].fileName, programs[i]);
				aging_queue[i].length = length_Files[i];
			}

			//initate the file pointers
			for(int j=0; j<row; j++) {
				p[j] = fopen(aging_queue[j].fileName, "rt"); //open upto 3 file pointers
				if (p[j] == NULL) return badcommandFileDoesNotExist();
			}

			while(1) {
				if (feof(p[0])) {
					fclose(p[0]);
					
					for(int l=0;l<matchrow; l++) {
						strcpy(programs[l], programs[l+1]);
						length_Files[l] = length_Files[l+1];
						aging_queue[l] = aging_queue[l+1];
						p[l]=p[l+1]; //swap contents when reached EOF
					} matchrow--;
				}

				if(matchrow <= 0) break; //when all commands are executed, leave loop
				
				//sort ready queue based off number of lines left after aging
				for(int i=0; i<matchrow; i++) {
					for(int j=0; j<matchrow-i-1; j++) {
						if(length_Files[j] > length_Files[j+1]) {
							char tempFiles[1000];
							strcpy(tempFiles, programs[j]);
							strcpy(programs[j], programs[j+1]);
							strcpy(programs[j+1], tempFiles);
							
							//swap length of files
							int tempLength = length_Files[j];
							length_Files[j] = length_Files[j+1];
							length_Files[j+1] = tempLength;

							//swap files in the queue based on priority
							struct queue temp_queue = aging_queue[j];
							aging_queue[j] = aging_queue[j+1];
							aging_queue[j+1] = temp_queue;

							//swap file pointers
							FILE *temp_pointer = p[j];
							p[j] = p[j+1];
							p[j+1] = temp_pointer;
						}
					}
				}

				fgets(line, 999, p[0]);
				pcb_set_script(line);

				//decrement length of the other files not at head
				for(int l=1; l<matchrow; l++) {
					if(length_Files[l]>0) length_Files[l]--; //length shouldn't be negative
				}
			}

			insert_queue();
			display_queue();
			cleanup();
		}
	}

	return 0;
}

void* read_file(void* filename) {
    char line[999];
	char* fname = (char*) filename;
    
	FILE* fp = fopen(fname, "r");
    if (fp == NULL) {
        printf("Error: could not open file %s\n", fname);
        pthread_exit(NULL);
    }

	while(1) {
		memset(line, '\0', sizeof(line));
		
		if (feof(fp)) break;
		fgets(line, 999, fp);
		
		if(strlen(line)==0) break;
		pcb_set_script(line);
	}

    fclose(fp);
    pthread_exit(NULL);
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

// int my_mkdir(char* dirname) {
// 	if (dirname[0] == '$') {
// 		dirname++; //increment pointer to ignore $
// 		char* dirnameToMake = mem_get_value(dirname);
// 		//check if one token
// 		for (int i=0; dirnameToMake[i] != '\0'; ++i) {
//         	if (dirnameToMake[i] == ' ') {
// 				printf("Bad command: my_mkdir\n");
// 				return 1;
// 			}
// 		}
// 		if(strcmp(dirnameToMake, "Variable does not exist") == 0) {
// 			printf("Bad command: my_mkdir\n");
// 			return 1;
// 		}
		
// 		// mkdir(dirnameToMake, 0775);
// 		return 0;
//   	}
// 	if (is_alphanumeric(dirname) == 1) {
// 		// mkdir(dirname, 0775);
// 		return 0;
// 	}  else {
// 		printf("Did not pass the alphanumeric test\n");
// 		return 1;
// 	}
// 	// if (mkdir(dirname, 0775)) { //check last since mkdir is called in if statement
// 	// 	printf("Error: Directory already exists\n");
// 	// 	return 1;
// 	// }
// 	return 0;
// } 

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