#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "shellmemory.h"
#include "shell.h"

char script_commands[1000][1000]; //static allocation for 3 files for upto 1000 commands
int row = 0;
int col = 0;

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
    for (int i=0; i<1000; i++){
        if (pcb_commands[i].pid == 0) {
            pcb_commands[i].pid = i+1; //increment unique pid
            pcb_commands[i].position = address;
            pcb_commands[i].length = length;
            return;
        } 
    }

    return;
}

void pcb_set_script_multithreading(char script[]) {
    for(int i=0; i<strlen(script); i++) {
        if(script[i] == '\n' || script[i] == '\r') {
            script[i] = '\0';
        }
    }
    
    strcpy(script_commands[row], script);
    pcb_set_value(row, col-1);
    
    //printf("row: %s, row #: %d, col: %d, len: %d\n", script_commands[row], row, col, strlen(script));

    row++; //increment number of commands after setting the value
    memset(script, '\0', strlen(script));
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
        pcb_set_value(row, col-1);
        row++; //increment number of commands after setting the value
}

void print_script_commands() {
    for (int i=0; i<1000; i++){
        if (pcb_commands[i].pid != 0) {
            
            int errCode = 0;
            errCode = parseInput(script_commands[pcb_commands[i].position]); // which calls interpreter()

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

        i++;
    }
}

void display_queue() {
    
    if(front == NULL) {
        printf("QUEUE EMPTY\n");
    } else {
        ptr_queue = front;
        
        while(ptr_queue != NULL) {
             
            int errCode = 0;
            if(strlen(script_commands[ptr_queue->ptr->position]) != 0) errCode = parseInput(script_commands[ptr_queue->ptr->position]); // which calls interpreter()
            if (errCode == -1) exit(99); // ignore all other errors

            //printf("len: %d; Command Name: --%s--\n", strlen(script_commands[ptr_queue->ptr->position]), script_commands[ptr_queue->ptr->position]);
            ptr_queue = ptr_queue->next;
        }
    }
}

void cleanup() {
    if(front == NULL) {
        //printf("NO ELEMENTS\n");
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