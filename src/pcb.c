#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "shellmemory.h"
#include "shell.h"

char script_commands[1000][1000]; //static allocation for 3 files for upto 1000 commands
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

struct queue *ptr_queue = NULL;
struct queue *front = NULL;
struct queue *rear = NULL;

void pcb_init() {

    for (int i=0; i<1000; i++) {    
        pcb_commands[i].pid = 0;
        pcb_commands[i].position = 0;
        pcb_commands[i].length = 0;
    }
}

void pcb_set_value(int address, int length) {
    
    //printf("HERE\n");

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

void pcb_set_script(char script[]) {
    //printf("script: %s\n", script);

    col = 0;

    for (int i=0; i<strlen(script); i++) {
        //printf("i: %d\n", i);
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
        //printf("row: %s, row #: %d, col: %d\n", script_commands[row], row, col);

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

void insert_queue() {

    int i=0;

    while(i < row) {
        ptr_queue = (struct queue *)malloc(sizeof(struct queue));

        ptr_queue->pid = pcb_commands[i].pid;
        ptr_queue->ptr = &pcb_commands[i];
        ptr_queue->next = NULL;

        if(front == NULL) { //if linked list is empty
            front = ptr_queue;
            rear = ptr_queue;
        } else {
            rear->next = ptr_queue;
            rear = ptr_queue;
        }

        //printf("i: %d\n", i);
        i++;
    }
}

void display_queue() {
    
    if(front == NULL) {
        printf("QUEUE EMPTY\n");
    } else {
        ptr_queue = front;
        
        while(ptr_queue != NULL) {
            //printf("PID: %d\n", ptr_queue->pid);
             
            int errCode = 0;
            errCode = parseInput(script_commands[ptr_queue->ptr->position]); // which calls interpreter()
            if (errCode == -1) exit(99); // ignore all other errors

            //printf("Command Name: --%s--\n", script_commands[ptr_queue->ptr->position]);
            ptr_queue = ptr_queue->next;
        }
    }
}

void cleanup() {
    if(front == NULL) {
        printf("NO ELEMENTS\n");
    } else {
        for(int i=0; i<row; i++) {
            ptr_queue = front;
            front = front->next;
            free(ptr_queue);
        }

        for(int i=0; i<row; i++) {
            script_commands[i][0] = '\0';
        }

        row = 0;
        front == NULL;
        rear = NULL;
        pcb_init(); //clear pcb_commands
        mem_init();
    }
}