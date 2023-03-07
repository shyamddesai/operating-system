#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "interpreter.h"
#include "shellmemory.h"
#include "pcb.h"

int MAX_USER_INPUT = 1000;
int parseInput(char ui[]);

int main(int argc, char *argv[]) {
    printf("%s\n", "Shell version 1.2 Created January 2023");
    //help();
    printf("\n");

    char prompt = '$';              // Shell prompt
    char userInput[MAX_USER_INPUT]; // user's input stored here
    int errorCode = 0;              // zero means no error, default
    int backgroundExecutionFlagMain = 0;
    char tempBackgroundExecution[1000];
    FILE *tempFile = fopen("temp.txt", "w+");

    // init user input
    for (int i = 0; i < MAX_USER_INPUT; i++)
        userInput[i] = '\0';

    // init shell memory
    mem_init();
    while (1) {
        if (isatty(0) == 1) { // returns 1 when file descriptor input refers to the keyboard
       
            printf("%c ", prompt);

            while (fgets(userInput, MAX_USER_INPUT - 1, stdin) == NULL); // fgets returns NULL when reached EOF

            if (feof(stdin)) freopen("/dev/tty", "r", stdin); // when reached end of file, switch stream to the terminal

            char array_commands[10][1000];
            int row = 0;
            int col = 0;
            int oneliner = 0;
            int backgroundExecutionFlag = 0;
        
            for (int i=0; i<strlen(userInput); i++) {
                if (userInput[i] == '#') {
                    backgroundExecutionFlag = 1;
                    backgroundExecutionFlagMain = 1;
                }

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
                if(backgroundExecutionFlag == 0) { 
                    errorCode = parseInput(userInput);
                    if (errorCode == -1) exit(99); // ignore all other errors
                    memset(userInput, 0, sizeof(userInput));
                } else {
                    backgroundExecutionFlag = 0; //ignore execution of first line if exec ... #, and continue
                    strcpy(tempBackgroundExecution, array_commands[0]);
                    tempBackgroundExecution[strlen(array_commands[0])-3] = '\0'; //store the exec command without the (space), #, and newline by appending NULL at the end
                }
            } else {
                array_commands[row][col - 1] = '\0'; // insert NULL character at the end of the last command

                for (int i=0; i <= row; i++) errorCode = parseInput(array_commands[i]);
                if (errorCode == -1) exit(99); // ignore all other errors
                memset(userInput, 0, sizeof(userInput));
            }
        } else {
            char backgroundCommands[10][1000];
            char tmp[1000];
            char tmp1[1000];
            int rowCommands = 0;
            int shell_commands=0;
             while (fgets(userInput, MAX_USER_INPUT - 1, stdin) != NULL) {
                if(userInput[strlen(userInput)-1] == '\n') {
                    strncpy(backgroundCommands[rowCommands++], userInput, strlen(userInput)-1); //remove \n at the end of commands
                } else {
                     strcpy(backgroundCommands[rowCommands++], userInput);
                }
            }
     

            if(tempFile != NULL) {
               
            int c=0;
            for(int i=0; i<rowCommands; i++) {
                for(int j =0;j<strlen(backgroundCommands[i]);j++){
                    if(backgroundCommands[i][j]==' '){
                         tmp[j]='\0';
                        break;
                    }
                    tmp[j]=backgroundCommands[i][j];
                }
                //fprintf(tempFile1, "%s", "TEST\n");
                //printf("temp: --%s--\n", tmp);

                if(strcmp(tmp,"exec") != 0){
                    //printf("%s\n", backgroundCommands[i]);
                   // fputs("TEST", tempFile);

                   fputs(backgroundCommands[i], tempFile);
                     fputs("\n", tempFile);
                    shell_commands++;
                    strcpy(tmp1,backgroundCommands[i]);
                    for(int j=i; j>c;j--)
                    {
                        strcpy(backgroundCommands[j],backgroundCommands[j-1]);
                    }
                    strcpy(backgroundCommands[c],tmp1);
                    c=i;
                }
                
            }
            }
            
            
            //if(feof(tempFile)) fclose(tempFile);
            
            // if(tempFile != NULL) {
            //     for(int i=0; i<rowCommands; i++) {
            //         fputs(backgroundCommands[i], tempFile);
            //         fputs("\n", tempFile);
            //     }
            // }

            if(tempFile!=NULL)
            {
                fclose(tempFile);
                tempFile = NULL;
            }
            char finalExecCommands[100];
            for(int l=0; l<100; l++) finalExecCommands[l] = '\0';

            char policy[5];
            char removeExec1[100][100];
            int m=0;

            strcpy(removeExec1[m++],  "exec temp.txt ");

            for(int i=shell_commands; i<rowCommands; i++) {
                //printf("i:%d; length:%d;  %s\n", i, strlen(backgroundCommands[i]), backgroundCommands[i]);

                char removeExec[100];
                

                for(int l=0; l<100; l++) removeExec[l] = '\0';

                int k=5;
                int j;
                for(j=5; j<strlen(backgroundCommands[i])+k; j++) {
                    removeExec[j-k] = backgroundCommands[i][j];
                }

                removeExec[j-k] = '\0';

                //remove #
                if(removeExec[strlen(removeExec)-1] == '#') {
                    removeExec[strlen(removeExec)-2] = '\0';
                }

                //remove multiple occurences of SJF
                if(strstr(removeExec, "SJF")) {
                    removeExec[strlen(removeExec)-3] = '\0';
                    strcat(policy, "SJF");
                }

                if(strstr(removeExec, "RR")) {
                    removeExec[strlen(removeExec)-2] = '\0';
                    strcat(policy, "RR");
                }

                //printf("--%s--\n", removeExec);

                strcpy(removeExec1[m++], removeExec);

                //strcat(tempArray, removeExec);
                //strcat(tempArray, "\0");
                //printf("%s\n", tempArray);
                
                //fprintf(tempFile, "%s", "TEST\n");
            }

            for(int i=0; i<m; i++) {
                printf("temp: %s\n", finalExecCommands);
                printf("removeExec1[%d]: %s\n", i, removeExec1[i]);

                strcat(finalExecCommands, removeExec1[i]);
            }

            if (feof(stdin)) freopen("/dev/tty", "r", stdin);
            errorCode = parseInput(finalExecCommands);
            if (errorCode == -1) exit(99); // ignore all other errors
           
        }
    }

    //execute exec ... # command last
    if(backgroundExecutionFlagMain == 1) {
        errorCode = parseInput(tempBackgroundExecution);
        if (errorCode == -1) exit(99); // ignore all other errors
        memset(userInput, 0, sizeof(userInput));
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
