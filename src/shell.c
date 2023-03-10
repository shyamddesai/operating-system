#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "interpreter.h"
#include "shellmemory.h"


int MAX_USER_INPUT = 1000;
int parseInput(char ui[]);

int main(int argc, char *argv[]) {
    printf("%s\n", "Shell version 1.2 Created January 2023");
    //help();
	printf("\n");

    char prompt = '$';              // Shell prompt
    char userInput[MAX_USER_INPUT]; // user's input stored here
    int errorCode = 0;              // zero means no error, default
	char p_commands[3][10]={"SJF","RR30", "RR"};
    // init user input
    for (int i = 0; i < MAX_USER_INPUT; i++)
	{
        userInput[i] = '\0';
	}
	char temp_Array[10][1000]; //holds all commands
	char temp1_Array[10][1000]; //holds exec commands
	int t_row=0;
	int t1_row=0;
	int i ,j,k,l,m;

    // init shell memory
    mem_init();
    if (!isatty(0)){
		int is_background=0;
		int is_tempfilecreated=0;
		int schedule=-1;
		
		
        while (fgets(userInput, MAX_USER_INPUT - 1, stdin) != NULL){
            
            //iterating through all commands and populating temp array
			for(int i=0;i<strlen(userInput);i++){ 
				if(userInput[i]=='\n'){
				userInput[i]='\0';
				break;
				}
			}
			userInput[strlen(userInput)]='\0';
			if(userInput[strlen(userInput)-1]=='#'){
			is_background=1;								//checks if backgound process using #
			
			//userInput[strlen(userInput)-2]='\0'; //remove (space) and # from exec command
			}
			strcpy(temp_Array[t_row++],userInput);
        }
		
		// for(int i=0; i<t_row; i++) {
        //     printf("temp: %s\n", temp_Array[i]);
        // }


		//executes only for non-background (#) commands
		if(is_background==0)
		{
			for(int i=0; i<t_row; i++) {
			errorCode = parseInput(temp_Array[i]);
            if (errorCode == -1) exit(99); // ignore all other errors
			}
			
		}
		else
		{
		//code for placing non exec files in temp.txt
		int c=0;
		FILE * fp=fopen("temp","wt");
		for(int i=0;i<t_row;i++)
		{	
			int k=0;
			char dest[1000];
			char dest1[1000];
			memset(dest, '\0', sizeof(dest)); //initialize with NULL
			memset(dest1, '\0', sizeof(dest));
			strncpy(dest, temp_Array[i], 4); //copy first four letters (to check for exec) of command into array
			
			//NEED TO TEST!!
            if(strcmp(dest,"exec")!=0)
			{
				
				is_tempfilecreated=1;
				
				strcpy(dest1, temp_Array[i]); //copy complete command if not exec command
				fputs(dest1,fp);
				fputs("\n",fp);
				
                // for(int j=i;j>c;j--)
				// {
				// 	strcpy(temp_Array[j],temp_Array[j-1]); //shifting commands to the top to move exec ... # below
				// }
				// strcpy(temp_Array[c],dest1);
			}

            // else
			// {
				
			// 	for(int j=5;j<strlen(temp_Array[i]);j++,k++)
			// 	{
			// 		temp1_Array[t1_row][k]=temp_Array[i][j]; //if exec command, strip (space) and exec substrings
			// 	}
			// 	temp1_Array[t1_row++][k]='\0'; //append NULL to the end of new string
			
			// }
			
			
			//c=i;
		}
		fclose(fp);
		
		if(is_tempfilecreated == 1) {
			char dest4[1000]; //array will be used to pass to interpreter
			memset(dest4, '\0', sizeof(dest4));

			for(int i=0; i<t_row; i++) {
				//printf("length: %d, temp: --%s--\n", strlen(temp_Array[i]), temp_Array[i]);

				if(temp_Array[i][strlen(temp_Array[i])-1] == '#') {
					strcpy(dest4, "exec temp");
					
					for(int j=4, k=9; j<strlen(temp_Array[i]); j++, k++) {
						dest4[k] = temp_Array[i][j]; //strip exec 
						//printf("dest4: %s\n", dest4);
					}
					//dest4[k] = '\0'; 
				}
			}

			//printf("dest4: %s\n", dest4);
			strcpy(temp_Array[0], dest4);
		}

		// for(int i=0;i<t_row;i++)
		// {
			
		// 	printf("\n[%d]--%s--\n",i,temp_Array[i]);
        // }
		
				for(int i=0;i<t_row;i++)
		{	
			
			char dest[1000];
			memset(dest, '\0', sizeof(dest)); //initialize with NULL
			strncpy(dest, temp_Array[i], 4); //copy first four letters (to check for exec) of command into array
			
			if(strcmp(dest,"exec") == 0)
			{
				strcpy(temp1_Array[t1_row++], temp_Array[i]);
			}
		}

		for(int i=0; i<t1_row; i++) {
			//printf("temp1: %s\n", temp1_Array[i]);
			errorCode = parseInput(temp1_Array[i]);
            if (errorCode == -1) exit(99); // ignore all other errors
		}

		// for(int i=0;i<t1_row;i++)
		// {
		// 	strcat(dest3,temp1_Array[i]); //append all exec commands together
		// 	strcat(dest3," ");
		// 	// printf("\n[%d]--%s--\n",i,temp1_Array[i]);
        // }
		
		// char dest3[1000]; //array will be used to pass to interpreter
		// memset(dest3, '\0', sizeof(dest3));
		
		// if(is_background==1)
		// {
		// 	strcpy(dest3,"exec temp ");
		// }			
		// else
		// {
		// 	strcpy(dest3,"exec ");	
		// }
		
		// for(i=0;i<t1_row;i++) //t1_row holds number of commands in temp1 array
		// {
		// 	for( j=0;j<3;j++) //iterate for SJF, RR, RR30
		// 	{
		// 		for(k =0;k<strlen(temp1_Array[i]);k++) //iterate for exec commands
		// 		{
					
		// 			if(p_commands[j][0]==temp1_Array[i][k])
		// 			{
						
        //                 //iterate through entire command to find substring of RR, RR30, or SJF
		// 				for(l=k,m=0;m<strlen(p_commands[j]);l++,m++)
		// 				{
		// 					//printf("%c->%c\n",p_commands[j][m],temp1_Array[i][l]);
		// 					if(temp1_Array[i][l]!=p_commands[j][m])
		// 					{
		// 						break;
		// 					}
							
		// 				}
		// 				if(m>=strlen(p_commands[j]))	
		// 				{
		// 					schedule=j; //schedule is 0 or 1 depending on SJF or RR
		// 					temp1_Array[i][k-1]='\0';
		// 					// for(int o=k;o<strlen(p_commands[j])+k;o++)
		// 					// {
		// 						// printf("before%d-> %s\n",o,temp1_Array[i]);
		// 						// temp1_Array[i][o]=temp1_Array[i][o+1];
		// 						// printf("alter%d -> %s\n",o,temp1_Array[i]);
		// 					// }
		// 					  //printf("found at %d of -%s-, %s\n",k,temp1_Array[i], p_commands[j]);
		// 				}
						
		// 			}
		// 		}
				
		// 	}
			
		// }

        //place exec ... # to the
		// if(strcmp(p_commands[schedule],"RR")==0)
		// {
		// 	char tmp[1000];
		// 	memset(tmp, '\0', sizeof(tmp));
		// 	strcpy(tmp,temp1_Array[0]);
		// 	for(int i=0;i<t1_row;i++)
		// 	{
		// 		strcpy(temp1_Array[i],temp1_Array[i+1]);
				
		// 	}
		// 	strcpy(temp1_Array[t1_row-1],tmp);
			
		// }
		// for(int i=0;i<t1_row;i++)
		// {
		// 	strcat(dest3,temp1_Array[i]);
		// 	strcat(dest3," ");
		// 	//printf("\n[%d]--%s--\n",i,temp1_Array[i]);
        // }
        // dest3[strlen(dest3)-1]='\0';
		// //printf("--final: %s--\n",dest3);
		
        // if(is_background != 1 && is_tempfilecreated == 1) {
        //     strcat(dest3, " temp ");
        // }

		// if(schedule!=-1)
		// {
		// 	strcat(dest3," ");
		// 	strcat(dest3,p_commands[schedule]);
		// 	strcat(dest3," ");
			
		// }
     
		// if(is_background==1)
		// {
		// 	strcat(dest3,"# ");
		// }
		
		// dest3[strlen(dest3)-1]='\0';
		// //printf("--LAST: %s--\n",dest3);
		// errorCode = parseInput(dest3);
        //     if (errorCode == -1) exit(99); // ignore all other errors
			
		// }	
		}	
			
    }
	else{
    while (1) {
        if (isatty(0)) // returns 1 when file descriptor input refers to the keyboard
            printf("%c ", prompt);

        while (fgets(userInput, MAX_USER_INPUT - 1, stdin) == NULL); // fgets returns NULL when reached EOF
      
        if (feof(stdin))
            freopen("/dev/tty", "r", stdin); // when reached end of file, switch stream to the terminal
           
        char array_commands[10][1000];
        int row = 0;
        int col = 0;
        int oneliner = 0;
      
        for (int i=0; i<strlen(userInput); i++) {
             
            if (userInput[i] == ';') {
                array_commands[row][col] = '\0'; //append NULL character at end of row
                oneliner = 1; //change flag
                row++; //move to next row
                col = 0; //reset back to 0th index
                i += 2; // skip ; and space characters
            }
            array_commands[row][col++] = userInput[i]; // copy the command, character by character and increment col
        }

        if (oneliner == 0) { // if not a one-liner, execute normally
            errorCode = parseInput(userInput);
            if (errorCode == -1) exit(99); // ignore all other errors
            memset(userInput, 0, sizeof(userInput));
        } else {
            array_commands[row][col - 1] = '\0'; // insert NULL character at the end of the last command

            for (int i=0; i <= row; i++)
			{
                errorCode = parseInput(array_commands[i]);
			}
				if (errorCode == -1) exit(99); // ignore all other errors
                memset(userInput, 0, sizeof(userInput));
        }
    }
	}
    return 0;
}

int parseInput(char ui[])
{
    char tmp[200];
    char *words[100];
    int a = 0;
    int b;
    int w = 0; // wordID
    int errorCode;
    for (a = 0; ui[a] == ' ' && a < 1000; a++)
        ; // skip white spaces
    while (ui[a] != '\n' && ui[a] != '\0' && a < 1000)
    {
        for (b = 0; ui[a] != ';' && ui[a] != '\0' && ui[a] != '\n' && ui[a] != ' ' && a < 1000; a++, b++)
        {
            tmp[b] = ui[a];
            // extract a word
        }
        tmp[b] = '\0';
        words[w] = strdup(tmp);
        w++;
        if (ui[a] == '\0')
            break;
        a++;
    }

    errorCode = interpreter(words, w);
    return errorCode;
}
