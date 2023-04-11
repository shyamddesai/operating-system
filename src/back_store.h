#ifndef BACK_STORE_H
#define BACK_STORE_H

#define FRAME_SIZE 3
#define MAX_LINE_SIZE 100
#define FRAME_STORE_SIZE framesize

// Structure for a frame in the frame store
typedef struct Frame {
    char code[FRAME_SIZE][MAX_LINE_SIZE];  // 3 lines of code
    int isOccupied;  // indicates whether the frame is currently being used
} Frame;

// Structure for a variable in the variable store
typedef struct Variable {
    char name[MAX_LINE_SIZE];  // name of the variable
    //char value[MAX_LINE_SIZE];  // value of the variable
    struct Variable* next;  // pointer to the next variable in the linked list
} Variable;

// Pointer to the head of the linked list representing the variable store
// Variable* varStoreHead = NULL;
// Array of frames to represent the frame store
// Frame frameStore[FRAME_STORE_SIZE];

void initialize_backing_store();
void cleanup_backing_store();
void load_script(char* script_file, char* fileName );
int load_frame(char file_array[5][100], int total_programs);
int rr_function(int counter);
int load_main_function(char file_array[5][100], int prog_count, int counter);
int read_cache (char command[]);
void disp_main_function(int tot);
int demand_page_replacement(char file_array[5][100], int prog_count, int counter, int fno);
int rr_function1(char file_array[5][100], int prog_count, int counter,int t);
void queue_elements();
void add_to_queue(char line[MAX_LINE_SIZE]);
void disp_main_frame_store();
void disp_main_frame_store1();
void fun_after_demand(int t);
int load_main_function2(int prog_count, int counter);
int demand_frame1_replacement(int fno);
void remove_from_queue();
void remove_element(char line[100]);
int find_cache (char command[30]);
int load_main_function1(char file_array[5][100], int prog_count, int counter);
int count_queue_elements();
void remove_visited_element();
#endif