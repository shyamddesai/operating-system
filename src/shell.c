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

    // init user input
    for (int i = 0; i < MAX_USER_INPUT; i++)
        userInput[i] = '\0';

    // init shell memory
    mem_init();
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
                errorCode = parseInput(array_commands[i]);
                if (errorCode == -1) exit(99); // ignore all other errors
                memset(userInput, 0, sizeof(userInput));
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
