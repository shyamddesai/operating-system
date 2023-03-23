#ifndef BACK_STORE_H
#define BACK_STORE_H
void initialize_backing_store();
void cleanup_backing_store();
void load_script(char* script_file, char* fileName);
int load_frame(char file_arry[5][100], int no);

//#define FRAME_SIZE 3
#define MAX_LINE_SIZE 100
//#define FRAME_STORE_SIZE 18

// Structure for a variable in the variable store
typedef struct Variable {
    char name[MAX_LINE_SIZE];  // name of the variable
    char value[MAX_LINE_SIZE];  // value of the variable
    struct Variable* next;  // pointer to the next variable in the linked list
} Variable;
#endif