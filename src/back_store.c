#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include "back_store.h"
#include "shell.h"

#define BACKING_STORE_DIR "backing_store"

int programs_loc[3][2];
char frameStore[FRAME_STORE_SIZE][100];
char main_frame_Store[FRAME_STORE_SIZE][MAX_LINE_SIZE];
char main_frame_Store1[FRAME_STORE_SIZE][MAX_LINE_SIZE];
int cache_status[FRAME_STORE_SIZE];
int cache_status1[FRAME_STORE_SIZE];
int cache_pageno[FRAME_STORE_SIZE]; //keep track of processed commands
int cache_pageno1[FRAME_STORE_SIZE]; //keep track of processed commands
int c1_main=0,c2_main=0,c3_main=0;
int gc1_main=0,gc2_main=0,gc3_main=0;
int main_counter=0;
int program_count=0;

// char main_frame_Store[FRAME_STORE_SIZE][MAX_LINE_SIZE];
//int rr_function(char file_array[5][100], int prog_count, int counter);
int  load_main_function(char file_array[5][100], int prog_count, int counter);
void disp_main_function(int tot);
struct Variable * front=NULL,*rear=NULL;
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
	program_count=total_programs;                    //GLOBAl VARIABLE
	Variable* varStoreHead = NULL;
	DIR *dir = opendir(BACKING_STORE_DIR);

    if (dir) {
		for(int i=0;i<total_programs;i++) {
			
		//	printf("%s\n",file_array[i]);
		
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
						j+=2; //skip ;
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

	// for(int i=0; i<total_programs; i++) {
	// 	printf("program %d--> %d lines\n", programs_loc[i][0], programs_loc[i][1]);
	// }

	// for(int j=0; j<counter; j++) {
	// 	printf("frame: --%s--\n", frameStore[j]);
	// }
main_counter=counter;
	return counter;	
}

int rr_function(int counter) {
	// int tot = 0;
	// int c1 = programs_loc[0][0], c2 = programs_loc[1][0], c3 = programs_loc[2][0];
	int tot = 0;
	int t=0;
	int c1=0,c2=0,c3=0,c1_end=0,c2_end=0,c3_end=0;
	int page=0 ;
	//printf("\n prog_count=%d\n",prog_count);
	
	c1 = programs_loc[0][0];
	c1_end = c1+programs_loc[0][1];
	
	if(program_count == 2){
		c2 = programs_loc[1][0];
		c2_end=c2+programs_loc[1][1];
	}

	if(program_count == 3){
		c2 = programs_loc[1][0];
		c2_end=c2+programs_loc[1][1];
		c3=programs_loc[2][0];
		c3_end=c3+programs_loc[2][1];
	}

	int flag=0;
	
	//disp_main_frame_store();

	while(tot<counter){
		if(c1 < c1_end) {
			for(int i=0; i<2 && c1<c1_end; i++) {
				
				if(read_cache (frameStore[c1]) == 0&& flag==0) 
				{
					//printf("found %s\n", frameStore[c1]);
					t++;
					cache_status[tot] = 1; //line processed
					 int errorCode = parseInput(frameStore[c1]);
					if (errorCode == -1) exit(99);
				}
				else
				{
					flag=1;
					//printf("\n---1%s---%d\n",frameStore[c1],tot);
					add_to_queue(frameStore[c1]);
					
					
					//return tot;
				}
				
				c1++;
				// printf("%s\n", frameStore[c1++]);
				tot++;
			}
			if(tot<FRAME_STORE_SIZE)
			{
				
			}
		}

		if(c2 < c2_end) {
			for(int i=0; i<2 && c2<c2_end;i++) {
				if(read_cache (frameStore[c2]) == 0&& flag==0) 
				{
					t++;
					int errorCode = parseInput(frameStore[c2]);
				if (errorCode == -1) exit(99);
					cache_status[tot] = 1; //line processed
				}
				else
				{
					flag=1;
					add_to_queue(frameStore[c2]);
						// printf("\n---2%s---\n",frameStore[c2]);
					//return tot;
				}
				c2++;
				//printf("%s\n", frameStore[c2++]);
				tot++;
			}
		}

		if(c3 < c3_end) {
			for(int i=0; i<2 && c3<c3_end;i++) {
				if(read_cache (frameStore[c3]) == 0 && flag==0) 
				{
					t++;
					cache_status[tot] = 1; //line processed
					int errorCode = parseInput(frameStore[c3]);
				if (errorCode == -1) exit(99);
				}
				else
				{
					flag=1;
					add_to_queue(frameStore[c3]);
						//printf("\n---3%s---\n",frameStore[c3]);
					//return tot;
				}
				c3++;
				// printf("%s\n", frameStore[c3++]);
				tot++;
				
			}
		}
		page++;	
	}
	// printf("vaule of total %d--counter ->%d\n",t,counter);
	/*while(tot<FRAME_STORE_SIZE)
	{
		for(int i=0; i<3; i++) {
			if(c1 < c1_end) {
				strcpy(main_frame_Store[tot],frameStore[c1++]);
				cache_status[tot] = 0;
				cache_pageno[tot]=page;
				
			} else {
				strcpy(main_frame_Store[tot],"\0");
				cache_status[tot] = 0;
				cache_pageno[tot]=page;
			}
			//strcat(main_frame_Store[tot],
			//printf("%s\n",frameStore[c1++]);
			tot++;
		}
		
	}*/
	return t;
}

int load_main_function(char file_array[5][100], int prog_count, int counter) {
	int tot = 0;
	int c1=0,c2=0,c3=0,c1_end=0,c2_end=0,c3_end=0;
	// printf("\n prog_count=%d\n",prog_count);
	 c1 = programs_loc[0][0];
	 c1_end=c1+programs_loc[0][1];
	if(prog_count==2){
	c2 = programs_loc[1][0];
	c2_end=c2+programs_loc[1][1];
	}
	if(prog_count== 3){
	c2 = programs_loc[1][0];
	c2_end=c2+programs_loc[1][1];
	c3=programs_loc[2][0];
	c3_end=c3+programs_loc[2][1];
	}
	int page = 0;
	
	//while(tot<FRAME_STORE_SIZE)
	while(page<2)	{
		if(tot>=counter)return tot;
		// printf("c1 =%d c1_end=%d",c1,c1_end);
		// printf("c2 =%d c2_end=%d",c2,c2_end);
		// printf("c3 =%d c3_end=%d",c3,c3_end);
		if(c1 < c1_end) {
			//frame[page].fileNumber=0;
			//frame[page].isLoadedMemory=0;
			for(int i=0; i<3; i++) {
				if(c1 < c1_end) {
					strcpy(main_frame_Store[tot],frameStore[c1++]);
					cache_status[tot] = 0;
					cache_pageno[tot]=page;
					
				} else {
					strcpy(main_frame_Store[tot],"\0");
					cache_status[tot] = 0;
					cache_pageno[tot]=page;
				}
				//strcat(main_frame_Store[tot],
				//printf("%s\n",frameStore[c1++]);
				tot++;
			}
		}

		if(c2 < c2_end) {
			//frame[page].fileNumber=1;
			//frame[page].isLoadedMemory=0;
			for(int i=0; i<3; i++) {
				if(c2 < c2_end) {
					strcpy(main_frame_Store[tot],frameStore[c2++]);
					cache_status[tot] = 0;
					cache_pageno[tot]=page;
					
				} else {
					strcpy(main_frame_Store[tot],"\0");
					cache_status[tot] = 0;
					cache_pageno[tot]=page;
					
				}
				//strcpy(frame[page].lines[i],frameStore[c2++]);
				//printf("%s\n",frameStore[c2++]);
				tot++;
			}
		}

		if(c3 < c3_end) {
			for(int i=0; i<3; i++) {
				if(c3 < c3_end) {
					strcpy(main_frame_Store[tot],frameStore[c3++]);
					cache_pageno[tot]=page;
					
				} else {
					strcpy(main_frame_Store[tot],"\0");
					cache_status[tot] = 0;
					cache_pageno[tot]=page;
					
				}
				//strcpy(frame[page].lines[i],frameStore[c3++]);
				//printf("%s\n",frameStore[c3++]);
				tot++;
			}
		}

		page++;
	}
	int rrval=rr_function(counter);
	// printf("\n before goig in side coounter %d rr -> %d",counter,rrval);
	if(FRAME_STORE_SIZE==21)
	{//printf("\n goin inside if condiotn\n");
		if(c1 < c1_end) {
			//frame[page].fileNumber=0;
			//frame[page].isLoadedMemory=0;
			for(int i=0; i<3; i++) {
				if(c1 < c1_end) {
					strcpy(main_frame_Store[tot],frameStore[c1++]);
					cache_status[tot] = 0;
					cache_pageno[tot]=page;
					
				} else {
					strcpy(main_frame_Store[tot],"\0");
					cache_status[tot] = 0;
					cache_pageno[tot]=page;
				}
				//strcat(main_frame_Store[tot],
				//printf("%s\n",frameStore[c1++]);
				tot++;
			}
		}

		else if(c2 < c2_end) {
			//frame[page].fileNumber=1;
			//frame[page].isLoadedMemory=0;
			for(int i=0; i<3; i++) {
				if(c2 < c2_end) {
					strcpy(main_frame_Store[tot],frameStore[c2++]);
					cache_status[tot] = 0;
					cache_pageno[tot]=page;
					
				} else {
					strcpy(main_frame_Store[tot],"\0");
					cache_status[tot] = 0;
					cache_pageno[tot]=page;
					
				}
				//strcpy(frame[page].lines[i],frameStore[c2++]);
				//printf("%s\n",frameStore[c2++]);
				tot++;
			}
		}

		else if(c3 < c3_end) {
			for(int i=0; i<3; i++) {
				if(c3 < c3_end) {
					strcpy(main_frame_Store[tot],frameStore[c3++]);
					cache_pageno[tot]=page;
					
				} else {
					strcpy(main_frame_Store[tot],"\0");
					cache_status[tot] = 0;
					cache_pageno[tot]=page;
					
				}
				//strcpy(frame[page].lines[i],frameStore[c3++]);
				//printf("%s\n",frameStore[c3++]);
				tot++;
			}
		}
	
	char temp[30];
	strcpy(temp,front->name);
	strcpy(front->name,front->next->name);
	strcpy(front->next->name,temp);
	
	}
	
	c1_main=c1;
	c2_main=c2;
	c3_main=c3;
	// printf("\n end of load main store\n");
	// printf("\nQueue\n");
	// queue_elements();
	return tot;
}

int read_cache (char command[30]) {
	
	for(int i=0; i<FRAME_STORE_SIZE; i++) {
		if(cache_status[i]==0)
		if(strcmp(main_frame_Store[i], command) == 0 ){
			cache_status[i]=1;
			return 0;
		}
	}	

	// printf("Page fault! Victim page contents:\n");
	return 1; //page fault occurred
}

void disp_main_function(int tot) {
	for(int i=0; i<tot; i++) {
		printf("%s-->%d\n",frameStore[i],cache_pageno[i]);
	}
}

int demand_page_replacement(char file_array[5][100], int prog_count, int counter, int fno)
{
	int tot=0;
	int tot1 =0;
	int c1 = programs_loc[0][0], c2 = programs_loc[1][0], c3 = programs_loc[2][0];
	// int c1=c1_main;
	// int c2=c2_main;
	// int c3=c3_main;
	int page = 0;
	// struct Variable* ptr=front;

	while(page<1) {
		
		if(c1 < programs_loc[0][0]+programs_loc[0][1]) {
			printf("Page fault! Victim page contents:\n\n");
			
			//frame[page].fileNumber=0;
			//frame[page].isLoadedMemory=0;
			for(int i=0; i<3; i++) {
				if(c1 < programs_loc[0][0]+programs_loc[0][1]) {
					printf("%s\n",frameStore[c1++]);
					strcpy(main_frame_Store[tot],frameStore[c1_main+i]);
					cache_status[tot] = 0;
				} else {
					strcpy(main_frame_Store[tot],"\0");
					cache_status[tot] = 0;
				}
				//strcat(main_frame_Store[tot],
				
				tot++;
			}
			printf("\nEnd of victim page contents.");
		}
		

		if(c2 < programs_loc[1][0]+programs_loc[1][1]) {
			printf("Page fault! Victim page contents:\n\n");
			
			//frame[page].fileNumber=1;
			//frame[page].isLoadedMemory=0;
			for(int i=0; i<3; i++) {
				if(c2 < programs_loc[1][0]+programs_loc[1][1]) {
					printf("%s\n",frameStore[c2++]);
					strcpy(main_frame_Store[tot],frameStore[c2_main+i]);
					cache_status[tot] = 0;
				} else {
					strcpy(main_frame_Store[tot],"\0");
					cache_status[tot] = 0;
				}
				//strcpy(frame[page].lines[i],frameStore[c2++]);
				
				tot++;
			}
			printf("\nEnd of victim page contents.");
		}

		if(c3 < programs_loc[2][0]+programs_loc[2][1]) {
			printf("Page fault! Victim page contents:\n\n");
			
			for(int i=0; i<3; i++) {
				if(c3 < programs_loc[2][0]+programs_loc[2][1]) {
					printf("%s\n",frameStore[c3++]);
					strcpy(main_frame_Store[tot],frameStore[c3_main+i]);
					cache_status[tot] = 0;
				} else {
					strcpy(main_frame_Store[tot],"\0");
					cache_status[tot] = 0;
				}
				//strcpy(frame[page].lines[i],frameStore[c3++]);
				
				tot++;
			}
			printf("\nEnd of victim page contents.");
		}

		page++;
	}
	
	

	return tot;
}

/*int rr_function1(char file_array[5][100], int prog_count, int counter,int t) {
	int tot = t; //index of main frame store element
	//int c1 = programs_loc[0][0], c2 = programs_loc[1][0], c3 = programs_loc[2][0];
	int c1=c1_main;
	int c2=c2_main;
	int c3=c3_main;
	int top=0;
	while((tot)<counter) {
		if(c1 < programs_loc[0][0]+programs_loc[0][1]) {
			for(int i=0; i<2 && c1<programs_loc[0][0]+programs_loc[0][1]; i++) {
				if(read_cache (frameStore[c1]) == 0) 
				{
					cache_status[tot] = 1; //line processed
				}
				else
				{
					printf("demamd for %s",frameStore[c1]);
				//	return tot;
				}
				 //int errorCode = parseInput(frameStore[tot++]);
			//	if (errorCode == -1) exit(99);
				// printf("%s\n", frameStore[c1++]);
				tot++;
				c1++;
			}
		}

		if(c2 < programs_loc[1][0]+programs_loc[1][1]) {
			for(int i=0; i<2 && c2<programs_loc[1][0]+programs_loc[1][1];i++) {
				if(read_cache (frameStore[c2]) == 0) 
				{
					cache_status[tot] = 1; //line processed
				}
				else
				{
					printf("demamd for %s",frameStore[c2]);
					return tot;
				}
				int errorCode = parseInput(frameStore[tot++]);
				if (errorCode == -1) exit(99);
				//printf("%s\n", frameStore[c2++]);
				tot++;
			}
		}

		if(c3 < programs_loc[2][0]+programs_loc[2][1]) {
			for(int i=0; i<2 && c3<programs_loc[2][0]+programs_loc[2][1];i++) {
				if(read_cache (frameStore[c3]) == 0) 
				{
					cache_status[tot] = 1; //line processed
				}
				else
				{
					printf("demamd for %s",frameStore[c3]);
					return tot;
				}
				int errorCode = parseInput(frameStore[tot++]);
				if (errorCode == -1) exit(99);
				// printf("%s\n", frameStore[c3++]);
				tot++;
			}
		}
	}
	return -1;
}*/
void add_to_queue(char line[MAX_LINE_SIZE])
{
	struct Variable* ptr= (struct Variable*) malloc(sizeof(struct Variable));
	strcpy(ptr->name,line);
	ptr->next=NULL;
	if(front==NULL)
	{
		front=ptr;
		rear=ptr;
	}
	else
	{
		rear->next=ptr;
		rear=ptr;
	}
}
int count_queue_elements()
{	int c=0;
		struct Variable* ptr=front;
		while(ptr)
		{
			// printf("\n%s->",ptr->name);
			ptr=ptr->next;
			c++;
		}
		return c;
	
}
void queue_elements()
{
		struct Variable* ptr=front;
		while(ptr)
		{
			printf("\n%s->",ptr->name);
			ptr=ptr->next;
		}
}
void disp_main_frame_store()
{
	for(int i=0;i<main_counter;i++)
	{
		printf("\n%s -- status-> %d --- pageno -->%d ",main_frame_Store[i],cache_status[i],cache_pageno[i]);
	}
}

void fun_after_demand(int t)
{ int counter=0; 
if(FRAME_STORE_SIZE==21)
	counter=0;
else
	counter=9;
int flag=0;
	struct Variable* ptr=front;
	while (ptr)
	{
		//printf("\n counter-%d\n",counter);
		if(read_cache(ptr->name)==0)
		{
			int errorCode = parseInput(ptr->name);
				if (errorCode == -1) exit(99);
			
			flag=0;
		}
		else{
			flag=1;
			if(FRAME_STORE_SIZE==21)
			{	
			char temp[30];
			strcpy(temp,ptr->name);
			strcpy(ptr->name,ptr->next->name);
			strcpy(ptr->next->name,temp);
			
			//printf("\n%s with %s",temp,ptr->name);
			}			
			//move_end(ptr);
			printf("Page fault! Victim page contents:\n\n");
			for(int i=counter;i<(counter+3);i++)
			{
				printf("%s\n",main_frame_Store[i]);
				strcpy(main_frame_Store[i],main_frame_Store1[i]);
				cache_status[i]=0;
				
			}
			printf("\nEnd of victim page contents.\n");
			counter+=3;
			
		}
		if(flag==0)
		ptr=ptr->next;
		remove_from_queue();
		if(count_queue_elements()==0)return ;
		if(counter==FRAME_STORE_SIZE )
		{
			printf("\n now counter=0\n\n");
			counter=0;
			
			printf("\n now displaying queue elment\n");
			queue_elements();
			
			//disp_main_frame_store();
			//printf("\n now removing visited nodes");
			remove_from_queue();
			//printf("\n displaying frame main1\n");
			//disp_main_frame_store1();
			//printf("\n updating frame main2\n");
			load_main_function2(3, t); 
			//printf("\n displaying frame main updated\n");
			//disp_main_frame_store1();
			ptr=front;
		}
		
		 
	}
	// printf("\n at the exit now displaying main frame\n");
	//disp_main_frame_store();
	
}
void fun_after_demand1(int t)
{ int counter=0; 
// if(FRAME_STORE_SIZE==21)
	// counter=0;
// else
	// counter=9;
int flag=0;
	struct Variable* ptr=front;
	while (ptr)
	{
		// printf("\n counter-%d\n",counter);
		if(read_cache(ptr->name)==0)
		{
			// printf("command: %s\n", ptr->name);
			int errorCode = parseInput(ptr->name);
				if (errorCode == -1) exit(99);
			
			flag=0;
		}
		else{
		
			flag=1;
			if(FRAME_STORE_SIZE==21)
			{	
			char temp[30];
			strcpy(temp,ptr->name);
			strcpy(ptr->name,ptr->next->name);
			strcpy(ptr->next->name,temp);
			
			//printf("\n%s with %s",temp,ptr->name);
			}			
			//move_end(ptr);
			printf("Page fault! Victim page contents:\n\n");
			for(int i=counter;i<(counter+3);i++)
			{
				printf("%s\n",main_frame_Store[i]);
				strcpy(main_frame_Store[i],main_frame_Store1[i]);
				cache_status[i]=0;
				
			}
			printf("\nEnd of victim page contents.\n");
			counter+=3;
			
		}
		if(flag==0)
		{
		ptr=ptr->next;
		}
		remove_from_queue();
		if(count_queue_elements()==0)return ;
		if(counter==FRAME_STORE_SIZE && FRAME_STORE_SIZE==21)
		{
			// printf("\n now counter=0\n\n");
			counter=0;
			
			// printf("\n now displaying queue elment\n");
			// queue_elements();
			
			//disp_main_frame_store();
			//printf("\n now removing visited nodes");
			remove_from_queue();
			//printf("\n displaying frame main1\n");
			//disp_main_frame_store1();
			//printf("\n updating frame main2\n");
			load_main_function2(3, t); 
			//printf("\n displaying frame main updated\n");
			//disp_main_frame_store1();
			ptr=front;
		}
		
		 
	}
	// printf("\n at the exit now displaying main frame\n");
	// disp_main_frame_store();
	
}


int demand_frame1_replacement( int fno)///have to replce parameter
{
	int tot=fno;
	int tot1 =0;
	//int c1 = c1_main, c2 = programs_loc[1][0], c3 = programs_loc[2][0];
	int c1=c1_main;
	int c2=c2_main;
	int c3=c3_main;
	int page = 0;
	// struct Variable* ptr=front;

	while(page<1) {
		
		if(c1 < programs_loc[0][0]+programs_loc[0][1]) {
			printf("\nPage1 fault! Victim page contents:\n\n");
			
			//frame[page].fileNumber=0;
			//frame[page].isLoadedMemory=0;
			for(int i=0; i<3; i++) {
				printf("%s %d\n",main_frame_Store[c1],c1);
				if(c1 < programs_loc[0][0]+programs_loc[0][1]) {
					
					strcpy(main_frame_Store[c1],frameStore[tot]);
					cache_status[tot] = 0;
				} else {
					strcpy(main_frame_Store[tot],"\0");
					cache_status[tot] = 0;
				}
				//strcat(main_frame_Store[tot],
				c1++;
				tot++;
			}
			printf("\nEnd of victim page contents.");
		}
		
	
		else if(c2 < programs_loc[1][0]+programs_loc[1][1]) {
			printf("\nPage2 fault! Victim page contents:\n\n");
			
			//frame[page].fileNumber=1;
			//frame[page].isLoadedMemory=0;
			for(int i=0; i<3; i++) {
				if(c2 < programs_loc[1][0]+programs_loc[1][1]) {
					printf("%s\n",main_frame_Store[c2]);
					strcpy(main_frame_Store[c2++],frameStore[c2_main++]);
					// printf("%s\n",frameStore[c2++]);
					// strcpy(main_frame_Store[tot],frameStore[c2_main++]);
					cache_status[tot] = 0;
				} else {
					strcpy(main_frame_Store[tot],"\0");
					cache_status[tot] = 0;
				}
				//strcpy(frame[page].lines[i],frameStore[c2++]);
				
				tot++;
			}
			printf("\nEnd of victim page contents.");
		}

		else if(c3 < programs_loc[2][0]+programs_loc[2][1]) {
			printf("\nPage3 fault! Victim page contents:\n\n");
			
			for(int i=0; i<3; i++) {
				if(c3 < programs_loc[2][0]+programs_loc[2][1]) {
					printf("%s\n",main_frame_Store[c3]);
					strcpy(main_frame_Store[c3++],frameStore[c3_main++]);
					cache_status[tot] = 0;
				} else {
					strcpy(main_frame_Store[tot],"\0");
					cache_status[tot] = 0;
				}
				//strcpy(frame[page].lines[i],frameStore[c3++]);
				
				tot++;
			}
			printf("\nEnd of victim page contents.");
		}

		page++;
	}
	
	

	return tot;
}

void remove_from_queue()
{
	for(int i=0;i<FRAME_STORE_SIZE;i++)
	{
		if(cache_status[i]==1)
		{
			remove_element(main_frame_Store[i]);
		}
		
	}
		

	
}

void remove_element(char line[100])
{
	
	struct Variable* ptr=front;
	struct Variable* ptrchild=front->next;
	//printf("%s",line);
	if(strcmp(ptr->name,line)==0)
	{
		front=front->next;
		free(ptr);
	}
	else
	{
		while(ptrchild)
		{
			if(strcmp(ptrchild->name,line)==0)
			{
				ptr->next=ptrchild->next;
				free(ptrchild);
				break;
			}
			ptr=ptr->next;
			ptrchild=ptrchild->next;
			
		}
		
	}	
}

void remove_visited_element()
{
	
	struct Variable* ptr=front;
	struct Variable* ptrchild=front->next;
	//printf("%s",line);
	if(find_cache(ptr->name)==0)
	{
		front=front->next;
		free(ptr);
	}
	else
	{
		while(ptrchild)
		{
			if(find_cache(ptrchild->name)==0)
			{
				ptr->next=ptrchild->next;
				free(ptrchild);
				break;
			}
			ptr=ptr->next;
			ptrchild=ptrchild->next;
			
		}
		
	}	
}

int find_cache (char command[30]) {
	for(int i=0; i<FRAME_STORE_SIZE; i++) {
		if(strcmp(main_frame_Store[i], command) == 0 && cache_status[i]==1 ){
			
			return 0;
		}
	}	

	// printf("Page fault! Victim page contents:\n");
	return 1; //page fault occurred
}
int load_main_function1(char file_array[5][100], int prog_count, int counter) {
	int tot = 0;
	int c1=0,c2=0,c3=0,c1_end=0,c2_end=0,c3_end=0;
	// printf("\n prog_count=%d\n",prog_count);
	c1 = programs_loc[0][0];
	 c1_end=c1+programs_loc[0][1];
	if(prog_count==2){
	c2 = programs_loc[1][0];
	c2_end=c2+programs_loc[1][1];
	}
	if(prog_count== 3){
	c2 = programs_loc[1][0];
	c2_end=c2+programs_loc[1][1];
	c3=programs_loc[2][0];
	c3_end=c3+programs_loc[2][1];
	}
	int page = 0;
	c1=c1_main;
	c2=c2_main;
	c3=c3_main;
	while(tot<FRAME_STORE_SIZE) {
		if(tot>=counter)return tot;
		// printf("c1 =%d c1_end=%d",c1,c1_end);
		// printf("c2 =%d c2_end=%d",c2,c2_end);
		// printf("c3 =%d c3_end=%d",c3,c3_end);
		if(c1==c1_end && c2==c2_end && c3==c3_end)break;
		if(c1 < c1_end) {
			//frame[page].fileNumber=0;
			//frame[page].isLoadedMemory=0;
			for(int i=0; i<3; i++) {
				if(c1 < c1_end) {
					strcpy(main_frame_Store1[tot],frameStore[c1++]);
					cache_status1[tot] = 0;
					cache_pageno1[tot]=page;
					
				} else {
					strcpy(main_frame_Store1[tot],"\0");
					cache_status1[tot] = 0;
					cache_pageno1[tot]=page;
				}
				//strcat(main_frame_Store[tot],
				//printf("%s\n",frameStore[c1++]);
				tot++;
			}
		}

		if(c2 < c2_end) {
			//frame[page].fileNumber=1;
			//frame[page].isLoadedMemory=0;
			for(int i=0; i<3; i++) {
				if(c2 < c2_end) {
					strcpy(main_frame_Store1[tot],frameStore[c2++]);
					cache_status1[tot] = 0;
					cache_pageno1[tot]=page;
					
				} else {
					strcpy(main_frame_Store1[tot],"\0");
					cache_status1[tot] = 0;
					cache_pageno1[tot]=page;
					
				}
				//strcpy(frame[page].lines[i],frameStore[c2++]);
				//printf("%s\n",frameStore[c2++]);
				tot++;
			}
		}

		if(c3 < c3_end) {
			for(int i=0; i<3; i++) {
				if(c3 < c3_end) {
					strcpy(main_frame_Store1[tot],frameStore[c3++]);
					cache_pageno1[tot]=page;
					
				} else {
					strcpy(main_frame_Store1[tot],"\0");
					cache_status1[tot] = 0;
					cache_pageno1[tot]=page;
					
				}
				//strcpy(frame[page].lines[i],frameStore[c3++]);
				//printf("%s\n",frameStore[c3++]);
				tot++;
			}
		}

		page++;
	}
	 gc1_main=c1;
	 gc2_main=c2;
	 gc3_main=c3;
	// printf("\n end of load main store1\n");
	return tot;
}
void disp_main_frame_store1()
{
	for(int i=0;i<FRAME_STORE_SIZE;i++)
	{
		printf("\n%s -- status-> %d --- pageno -->%d ",main_frame_Store1[i],cache_status1[i],cache_pageno1[i]);
	}
}
int load_main_function2(int prog_count, int counter) {
	int tot = 0;
	int c1=0,c2=0,c3=0,c1_end=0,c2_end=0,c3_end=0;
	// printf("\n---------------- total =%d\n",FRAME_STORE_SIZE);
	c1 = programs_loc[0][0];
	 c1_end=c1+programs_loc[0][1];
	if(prog_count==2){
	c2 = programs_loc[1][0];
	c2_end=c2+programs_loc[1][1];
	}
	if(prog_count== 3){
	c2 = programs_loc[1][0];
	c2_end=c2+programs_loc[1][1];
	c3=programs_loc[2][0];
	c3_end=c3+programs_loc[2][1];
	}
	int page = 0;
	c1=gc1_main;
	c2=gc2_main;
	c3=gc3_main;
	while(page<2) {
	 if(tot>=FRAME_STORE_SIZE)return tot;
		// printf("c1 =%d c1_end=%d",c1,c1_end);
		// printf("c2 =%d c2_end=%d",c2,c2_end);
		// printf("c3 =%d c3_end=%d",c3,c3_end);
		if(c1 < c1_end) {
					printf("\n inside 1\n");
			//frame[page].fileNumber=0;
			//frame[page].isLoadedMemory=0;
			for(int i=0; i<3; i++) {
				
				if(c1 < c1_end) {
					strcpy(main_frame_Store1[tot],frameStore[c1++]);
					cache_status1[tot] = 0;
					cache_pageno1[tot]=page;
					
				} else {
					strcpy(main_frame_Store1[tot],"\0");
					cache_status1[tot] = 0;
					cache_pageno1[tot]=page;
				}
				//strcat(main_frame_Store[tot],
				//printf("%s\n",frameStore[c1++]);
				tot++;
			}
		}

		if(c2 < c2_end) {
					
			//frame[page].fileNumber=1;
			//frame[page].isLoadedMemory=0;
			for(int i=0; i<3; i++) {
				
				if(c2 < c2_end) {
					strcpy(main_frame_Store1[tot],frameStore[c2++]);
					cache_status1[tot] = 0;
					cache_pageno1[tot]=page;
					
				} else {
					strcpy(main_frame_Store1[tot],"\0");
					cache_status1[tot] = 0;
					cache_pageno1[tot]=page;
					
				}
				//strcpy(frame[page].lines[i],frameStore[c2++]);
				//printf("%s\n",frameStore[c2++]);
				tot++;
			}
		}

		if(c3 < c3_end) {
			
			for(int i=0; i<3; i++) {
				
				if(c3 < c3_end) {
					strcpy(main_frame_Store1[tot],frameStore[c3++]);
					cache_pageno1[tot]=page;
					
				} else {
					strcpy(main_frame_Store1[tot],"\0");
					cache_status1[tot] = 0;
					cache_pageno1[tot]=page;
					
				}
				//strcpy(frame[page].lines[i],frameStore[c3++]);
				//printf("%s\n",frameStore[c3++]);
				tot++;
			}
		}

		page++;
	}
	c1_main=c1;
	c2_main=c2;
	c3_main=c3;
	// printf("\n end of load main store2\n");
	return tot;
}
void move_end(struct Variable* ptr)
{
	printf("\n%s",ptr->name	);
	struct Variable* ptr1=front;
	struct Variable* ptr2=front->next;
	if(strcmp(ptr1->name,ptr->name)==0)
	{
		front=front->next;
		rear->next=ptr;
		rear=ptr;
	}
	else
	{
		while(ptr2)
		{
			if(strcmp(ptr2->name,ptr->name)==0)
			{
				ptr1->next=ptr2->next;
				rear->next=ptr;
				rear=ptr;
				
			}
			ptr1=ptr1->next;
			ptr2=ptr2->next;
		}
		
	}
}