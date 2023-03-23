#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include "back_store.h"

#define BACKING_STORE_DIR "backing_store"
#define FRAME_STORE_SIZE  18

int programs_loc[3][2];
char frameStore[FRAME_STORE_SIZE][100];

void initialize_backing_store() {
    DIR *dir = opendir(BACKING_STORE_DIR);
    if (dir) {
        // Backing store directory already exists
        struct dirent *entry;
        while ((entry = readdir(dir))) {
            // Delete all contents of the directory
            char file_path[256];
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
            char file_path[256];
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
        char backing_store_file_name[50];

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

int load_frame(char file_arry[5][100], int no) {
	printf("inside");
	
	int counter=0;
	int col=0;
	int count_lines=0;
	Variable* varStoreHead = NULL;
	DIR *dir = opendir(BACKING_STORE_DIR);
    if (dir) {
		for(int i=0;i<no;i++) //open every file and store counter position
		{
			
			printf("%s\n",file_arry[i]);
		
		
			char file_path[256];
            snprintf(file_path, sizeof(file_path), "%s/%s", BACKING_STORE_DIR, file_arry[i]);
	
			FILE* backing_store_fp;
			char backing_store_file_name[50];
			sprintf(backing_store_file_name, "%s", file_path);
			backing_store_fp = fopen(backing_store_file_name, "r+");
			if (backing_store_fp == NULL) {
				printf("Error: Cannot open backing store file for %s\n", file_arry[i]);
				fclose(backing_store_fp);
				return;
			}
			char chunk[128];

            programs_loc[i][0]=counter; //store total lines
		   while(fgets(chunk, sizeof(chunk), backing_store_fp) != NULL) {
			   col=0;
			   // printf("from chunk %s\n",chunk);
				for(int j=0;j<strlen(chunk);j++)
				{
					if(chunk[j]==';')
					{
						frameStore[counter][col]='\0';
						counter++;
						count_lines++;
						col=0;
					}
					if(chunk[j]!='\n')
					frameStore[counter][col++]=chunk[j];
					
				}
				frameStore[counter][col]='\0';
				count_lines++;
				counter++;
			}

			programs_loc[i][1]=count_lines;
			count_lines=0;
			fclose(backing_store_fp);
		}
	}
	for(int i=0;i<no;i++)
	{
		printf("program %d--> have %d lines\n",programs_loc[i][0],programs_loc[i][1]);
	}
		for(int j=0;j<counter;j++)
		{
			printf("frame--%s--\n",frameStore[j]);
			
		}
return counter;	
}