#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "interpreter.h"
#include "shellmemory.h"

int MAX_USER_INPUT = 1000;
int parseInput(char ui[]);

// Start of everything
int main(int argc, char *argv[])
{
    printf("%s\n", "Shell version 1.2 Created January 2023");
    help();

    char prompt = '$';              // Shell prompt
    char userInput[MAX_USER_INPUT]; // user's input stored here
    int errorCode = 0;              // zero means no error, default

    // init user input
    for (int i = 0; i < MAX_USER_INPUT; i++)
        userInput[i] = '\0';

    // init shell memory
    mem_init();
    while (1)
    {
        if (isatty(0))
        {
            printf("%c ", prompt);
            //printf("Input is from the keyboard\n");
        }
        else
        {
            //printf("Input is from a file\n");
        }
        
        while (fgets(userInput, MAX_USER_INPUT - 1, stdin) == NULL);
        
        if (feof(stdin))
        {
            freopen("/dev/tty","r",stdin);
            //execvp(argv[0], (char *const *)argv);            
        }
        errorCode = parseInput(userInput);
        if (errorCode == -1) exit(99); // ignore all other errors
        memset(userInput, 0, sizeof(userInput));
    }

    return 0;
}

int parseInput(char ui[])
{

    printf("ui: ");
    for(int i=0; i<strlen(ui); i++) {
        printf("%c", ui[i]);
    } 

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
    
    printf("w: %d, words: ", w);
    for(int i=0; i<strlen(words); i++) {
        printf("%s", words[i]);
    } printf("\n");
    

    errorCode = interpreter(words, w);
    return errorCode;
}
