#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char script_commands[100][1000]; //static allocation for 3 files for upto 100 commands
int row=0;
int col=0;

struct pcb {
    int pid;
    int position;  //index of command
    int length; //# of commmands
} pcb_commands[1000];

struct queue {
    int pid;
    struct pcb *ptr;
    struct queue *next;
} pcb_queue;

void pcb_init() {

    for (int i=0; i<1000; i++) {    
        pcb_commands[i].pid = 0;
        pcb_commands[i].position = 0;
        pcb_commands[i].length = 0;
    }
}

void pcb_set_script(char script[]) {

    col = 0;

    for (int i=0; i<strlen(script); i++) {
        if (script[i] == ';') { //handles one liners
            script_commands[row][col] = '\0'; //append NULL character at end of row
            row++; //move to next row
            col=0; //reset back to 0th index
            i+=2; // skip ; and space characters
        }

        if(script[i] != '\n' && script[i] != '\r') //handle edge case for last command
            script_commands[row][col++] = script[i]; // copy the command, character by character and increment col
    }

        script_commands[row][col] = '\0'; //append NULl character instead of \n to the end of last command
        //printf("row: %s, row #: %d\n", script_commands[row], row);

        // for(int i=0; i<row; i++) {
        //     for(int j=0; j<col; j++) {
        //         printf("%s\n", script_commands[i][j]);
        //     }
        // }
        
        pcb_set_value(row, col-1);
        row++; //increment number of commands after setting the value
}

void print_script_commands() {
    for (int i=0; i<1000; i++){
        if (pcb_commands[i].pid != 0) {
            //printf("LENGTH: %d\n", pcb_commands[i].length);
            //printf("POSITION: %d\n", pcb_commands[i].position);
            
            //printf("value: %s\n", script_commands[pcb_commands[i].position]);

            int errCode = 0;
            errCode = parseInput(script_commands[pcb_commands[i].position]); // which calls interpreter()

            // for (int j=0; j<pcb_commands[i].length; j++) {
            //     printf("%s\n", script_commands[pcb_commands[i].position][j]);
            // }
        } else return; 
    } 
}

void pcb_set_value(int address, int length) {
    
    for (int i=0; i<1000; i++){
        if (pcb_commands[i].pid == 0) {
            pcb_commands[i].pid = i+1; //increment unique pid
            pcb_commands[i].position = address;
            pcb_commands[i].length = length;
            return;
        } 
    }

    //Value does not exist, need to find a free spot.
    // for (int i=0; i<1000; i++){
    //     if (strcmp(shellmemory[i].var, "none") == 0){
    //         shellmemory[i].var = strdup(var_in);
    //         shellmemory[i].value = strdup(value_in);
    //         return;
    //     } 
    // }

    return;
}