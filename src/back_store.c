#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include "back_store.h"

#define BACKING_STORE_DIR "backing_store"
#define FRAME_STORE_SIZE 18

int programs_loc[3][2];
char frameStore[FRAME_STORE_SIZE][100];
char main_frame_Store[FRAME_STORE_SIZE][MAX_LINE_SIZE];

//Function Prototypes
void initialize_backing_store();
void cleanup_backing_store();
void load_script(char* script_file, char* fileName );
int load_frame(char file_array[5][100], int total_programs);

char main_frame_Store[FRAME_STORE_SIZE][MAX_LINE_SIZE];
void rr_function(char file_array[5][100], int prog_count, int counter);
int  load_main_function(char file_array[5][100], int prog_count, int counter);
void disp_main_function(int tot);

void initialize_backing_store() {
    DIR *dir = opendir(BACKING_STORE_DIR);
    if (dir) {
        // Backing store directory already exists
        struct dirent *entry;
        while ((entry = readdir(dir))) {
            // Delete all contents of the directory
            char file_path[300];
            snprintf(file_path, sizeof(file_path), "%s/%s", BACKING_STORE_DIR, entry->d_name);
            remove(file_path);
        }
        closedir(dir);
    } else if (ENOENT == errno) {
        // Backing store directory does not exist, create it
        mkdir(BACKING_STORE_DIR, 0755);
    } else {
        perror("Error opening backing store directory");
        exit(EXIT_FAILURE);
    }
}

void cleanup_backing_store() {
    DIR *dir = opendir(BACKING_STORE_DIR);
    if (dir) {
        struct dirent *entry;
        while ((entry = readdir(dir))) {
            char file_path[300];
            snprintf(file_path, sizeof(file_path), "%s/%s", BACKING_STORE_DIR, entry->d_name);
            remove(file_path);
        }
        closedir(dir);
        rmdir(BACKING_STORE_DIR);
    }
}

void load_script(char* script_file, char* fileName) {
    FILE* fp;
    // char* line = NULL;
	// size_t len = 0;
	
    fp = fopen(script_file, "r");
    if (fp == NULL) {
        printf("Error: Cannot open file %s\n", script_file);
        return;
    }

	DIR *dir = opendir(BACKING_STORE_DIR);
    if (dir) {
		char file_path[256];
        snprintf(file_path, sizeof(file_path), "%s/%s", BACKING_STORE_DIR, fileName); //copy script to backing store
	
        FILE* backing_store_fp;
        char backing_store_file_name[300];

        sprintf(backing_store_file_name, "%s", file_path); //copy file path into back store file name
        backing_store_fp = fopen(backing_store_file_name, "w+");
        if (backing_store_fp == NULL) {
            printf("Error: Cannot create backing store file for %s\n", fileName);
            fclose(fp);
            return;
        }

        char chunk[128];
        while(fgets(chunk, sizeof(chunk), fp) != NULL) {
            fputs(chunk, backing_store_fp); 
        }

        fclose(fp);
        fclose(backing_store_fp);
	}
}

int load_frame(char file_array[5][100], int total_programs) {
	//printf("inside\n");
	
	int counter = 0;
	int col = 0;
	int count_lines = 0;
	Variable* varStoreHead = NULL;
	DIR *dir = opendir(BACKING_STORE_DIR);

    if (dir) {
		for(int i=0;i<total_programs;i++) {
			
			printf("%s\n",file_array[i]);
		
			char file_path[300];
            snprintf(file_path, sizeof(file_path), "%s/%s", BACKING_STORE_DIR, file_array[i]);
	
			FILE* backing_store_fp;
			char backing_store_file_name[300];
			sprintf(backing_store_file_name, "%s", file_path);
			
			backing_store_fp = fopen(backing_store_file_name, "r+");
			if (backing_store_fp == NULL) {
				printf("Error: Cannot open backing store file for %s\n", file_array[i]);
				fclose(backing_store_fp);
				return 1;
			}

			char chunk[128];
			programs_loc[i][0]=counter;
			while(fgets(chunk, sizeof(chunk), backing_store_fp) != NULL) {
			   col=0;
			   // printf("from chunk %s\n",chunk);
				for(int j=0; j<strlen(chunk); j++) {
					if(chunk[j] == ';') {
						frameStore[counter][col] = '\0';
						counter++;
						count_lines++;
						col=0;
					}

					if(chunk[j] != '\n') frameStore[counter][col++] = chunk[j];
				}

				frameStore[counter][col] = '\0';
				count_lines++;
				counter++;
			}
			
			programs_loc[i][1] = count_lines;
			count_lines = 0;

			fclose(backing_store_fp);
		}
	}

	for(int i=0; i<total_programs; i++) {
		printf("program %d--> %d lines\n", programs_loc[i][0], programs_loc[i][1]);
	}

	for(int j=0; j<counter; j++) {
		printf("frame: --%s--\n", frameStore[j]);
	}
	
	return counter;	
}

void rr_function(char file_array[5][100], int prog_count, int counter) {
	int tot = 0;
	int c1 = programs_loc[0][0], c2 = programs_loc[1][0], c3 = programs_loc[2][0];
	
	while((tot)<counter) {
		if(c1 < programs_loc[0][0]+programs_loc[0][1]) {
			for(int i=0; i<2 && c1<programs_loc[0][0]+programs_loc[0][1]; i++) {
				printf("%s\n",frameStore[c1++]);
				tot++;
			}
		}

		if(c2 < programs_loc[1][0]+programs_loc[1][1]) {
			for(int i=0; i<2 && c2<programs_loc[1][0]+programs_loc[1][1];i++) {
				printf("%s\n",frameStore[c2++]);
				tot++;
			}
		}

		if(c3 < programs_loc[2][0]+programs_loc[2][1]) {
			for(int i=0; i<2 && c3<programs_loc[2][0]+programs_loc[2][1];i++) {
				printf("%s\n",frameStore[c3++]);
				tot++;
			}
		}
	}
}

int load_main_function(char file_array[5][100], int prog_count, int counter) {
	int tot = 0;
	int c1 = programs_loc[0][0], c2 = programs_loc[1][0], c3 = programs_loc[2][0];
	int page = 0;
	
	while(page<2) {
		if(c1 < programs_loc[0][0]+programs_loc[0][1]) {
			//frame[page].fileNumber=0;
			//frame[page].isLoadedMemory=0;
			for(int i=0; i<3; i++) {
				if(c1 < programs_loc[0][0]+programs_loc[0][1]) {
					strcpy(main_frame_Store[tot],frameStore[c1++]);
				} else {
					strcpy(main_frame_Store[tot],"\0");
				}
				//strcat(main_frame_Store[tot],
				//printf("%s\n",frameStore[c1++]);
				tot++;
			}
		}

		if(c2 < programs_loc[1][0]+programs_loc[1][1]) {
			//frame[page].fileNumber=1;
			//frame[page].isLoadedMemory=0;
			for(int i=0; i<3; i++) {
				if(c2 < programs_loc[1][0]+programs_loc[1][1]) {
					strcpy(main_frame_Store[tot],frameStore[c2++]);
				} else {
					strcpy(main_frame_Store[tot],"\0");
				}
				//strcpy(frame[page].lines[i],frameStore[c2++]);
				//printf("%s\n",frameStore[c2++]);
				tot++;
			}
		}

		if(c3 < programs_loc[2][0]+programs_loc[2][1]) {
			for(int i=0; i<3; i++) {
				if(c3 < programs_loc[2][0]+programs_loc[2][1]) {
					strcpy(main_frame_Store[tot],frameStore[c3++]);
				} else {
					strcpy(main_frame_Store[tot],"\0");
				}
				//strcpy(frame[page].lines[i],frameStore[c3++]);
				//printf("%s\n",frameStore[c3++]);
				tot++;
			}
		}

		page++;
	}

	return tot;
}

void disp_main_function(int tot) {
	for(int i=0; i<tot; i++) {
		printf("%s\n",main_frame_Store[i]);
	}
}