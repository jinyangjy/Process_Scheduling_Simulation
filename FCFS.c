
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum{
	READY,
	RUNNING,
	EXIT
}process_state_t;

typedef struct{
	char process_name[11];
	
	int entryTime;	//The time process entered system
	int serviceTime;	//The total CPU time required by the process
	int remainingTime;	// Remaining service time until completion
	
	int run_time;	//Time it takes to run a process
	int wait_time;	
	int turnaround_time;	
	int deadline_met;
	
	int completion_time; 
	
	bool entry;		// indicator to check if the process is already within the system. 
	bool completed;	//indicator to check if a process is completed 
	
	process_state_t state;

}pcb_t;

bool all_process_done(pcb_t processes[100], int);

int main(int argc, char* argv[]){

	FILE *fptr; //file pointer
	FILE *fptr_write;	//file pointer for output file, writing to output file
	
	
	
	char buffer[2086];	//Where source file is being read
	
	char arrival_time[10];	//Arrival time
	char service_time[10];	//Service time
	char deadline[10];	//Deadline
	
	int time = 0;
	int current_process;
	
	
	int pointer_name = 0;	//keeping track the index of the buffer for process name
	int pointer_arrival_time = 0;	//keeping track the index of the buffer arrival time
	int pointer_service_time = 0;	//keeping track the index of the buffer service time....
	int pointer_deadline = 0;	//keeping track the index of the buffer deadline
	
	int pointer_vertical = 0;	//keeping track which line we're reading 
	int pointer_process = 0;	//keeping track which row we're reading
	int number_of_processes = 0;	//number of processes provided by the source file
	
	pcb_t processes[100];	//array obstruct, assume at most the number of processes will be 100
	pcb_t FCFS[1];		//Rotation of queue or sequences shouldn't be bothered, thus the creation of struct array of size 1. 
	
	/*
	following the convention given in assignment1, since the library for stdio.h is allowed within the scope of this assignment, we're
	allowed the use for functions such as printf, here we're checking for errors within the argument and erros for opening file	
	*/
	if (argv[1] != NULL && argv[1][0] != '/'){
		printf("Error Argument Provided By User\n");
		exit(1);
	}
	
	if (argv[1] != NULL && argv[1][0] == '/'){
		fptr = fopen(argv[1], "r");
		if(fptr == NULL){
			printf("File Open Error\n");
			exit(1);
		}
	}
	else{
		fptr = fopen("processes.txt", "r");
		if(fptr == NULL){
			printf("Default File Open Error\n");
			exit(1);
		}
	}
	fread(&buffer, sizeof(char), 2086, fptr);
	
	fptr_write = fopen("result-1.txt", "w");
	
	if (fptr_write == NULL){
		printf("Error Creating New Text File\n");
		exit(1);
	}
	
	//to check the end of the character within the length of the buffer, which was previously set to 2086, so it'll be within.
	//Every index j will be checking within the "line" of information, where we're checking whether it is not a space or not a newline,
	//If it's not either, we'll be recording the information to it's respective struct attribute. 
	for (int j = 0; j < strlen(buffer); j++){
		if (buffer[j] != ' ' && buffer[j] != '\n'){
			//The size of the struct array for processes is a size of 100.
			//pointer name starts from 0, accessing the process of the pointer_process, then  the pointer name
			//accesses to the attributes within the process_name.
			if(pointer_vertical == 0){
				processes[pointer_process].process_name[pointer_name] = buffer[j];
				pointer_name += 1;
			}
			
			//pointer name then moves on to 1, accessing the process of the pointer_process, then  the pointer name
			//accesses to the attributes within the process_name.
			if (pointer_vertical == 1){
				arrival_time[pointer_arrival_time] = buffer[j];
				pointer_arrival_time += 1;
			}
			
			//pointer name then moves on to 2, accessing the process of the pointer_process, then  the pointer name
			//accesses to the attributes within the process_name.
			if (pointer_vertical == 2){
				service_time[pointer_service_time] = buffer[j];
				pointer_service_time += 1;	
			}
			//pointer name then moves on to 3, accessing the process of the pointer_process, then  the pointer name
			//accesses to the attributes within the process_name.
			if (pointer_vertical == 3){
				deadline[pointer_deadline] = buffer[j];
				pointer_deadline += 1;
			}
		}
		
		if (buffer[j] == ' '){
			pointer_vertical += 1;
		}
		
		if (buffer[j] == '\n'){
			/*
			Before changing from character to integer, we'll first have to store each variable into the object classes. We've previously used 
			char, now we're using atoi to convert each arrival_time, service_time and deadline to integers
			*/
			processes[pointer_process].entryTime = atoi(arrival_time);	
			processes[pointer_process].serviceTime = atoi(service_time);
			processes[pointer_process].remainingTime = atoi(deadline);
			
			//entry and completed will be set to false at each 
			processes[pointer_process].entry = false;
			processes[pointer_process].completed = false;
			/*
			Pointer process will never be reset back to 0, because each time we're done reading the file, it points
			to the next process.
			*/
			pointer_process += 1;
			
			//pointer resets back to 0, once one of the "role"/processes has been read. 
			pointer_arrival_time = 0;
			pointer_service_time = 0;
			pointer_deadline = 0;
			pointer_vertical = 0;		
			pointer_name = 0;
			number_of_processes += 1;
		}
	}
	
	//loops through every available process to check whether all process are done. It will keep looping if it's still "false", techincally 
	//till all process are done. 
	//while loop is for time
	while(all_process_done(processes, number_of_processes) == false){
		//will loop from 0, till the number of processes that are located in the source file. 
		//for loop to check for process.
		for (int i = 0; i < number_of_processes; i++){
			//will check each and every entrytime/ arrivial time is equals to the time and process..
			//to check whether the process's pass into the system. Once it is passed into .entry it 
			// will turn true. 
			if (processes[i].entryTime == time && processes[i].entry == false){
				processes[i].entry = true;
				processes[i].state = READY;
				//check whether it is in the system
				printf("Time %d: %s has entered the system.\n", time, processes[i].process_name);
				//if the fcfs buffer is not occupied by any process, then it'll go into fcfs queue and will be ready to RUNNN
				if (strlen(FCFS[0].process_name) == 0 && processes[i].state == READY){
					FCFS[0] = processes[i];
					processes[i].state = RUNNING;
					processes[i].run_time = time;
					processes[i].wait_time = processes[i].run_time - processes[i].entryTime;
					processes[i].completion_time = time + processes[i].serviceTime;
					processes[i].turnaround_time = processes[i].completion_time - processes[i].entryTime;
					current_process = i;
					printf("Time %d: %s is in the running state. \n", time, processes[i].process_name);
				}
			}
		}
		
		//When FCFS buffer is occupied by another process, then it checks whether the process fcfs queue is finished .
		//First Condition is to check whether fcfs is occupied by another process. Second Condtion for you to check the process's
		//completion time is equals to the current time, it means we're done, execution time is completed. 
		if (strlen(FCFS[0].process_name) != 0 && processes[current_process].completion_time == time){
				processes[current_process].completed = true;
				processes[current_process].state = EXIT;
				printf("Time %d: %s has finished execution. \n", time, processes[current_process].process_name);
				//writing into the output file, once execution has completed.
				fputs(processes[current_process].process_name, fptr_write);
				fputc(' ', fptr_write);
				fprintf(fptr_write, "%d ", processes[current_process].wait_time);
				fprintf(fptr_write, "%d ", processes[current_process].turnaround_time);
				//This condtion's to check whether the deadline has been met, by if turnaround time is lesser than or
				//remaining time, then the deadline is met. 
				if (processes[current_process].turnaround_time <= processes[current_process].remainingTime){
					processes[current_process].deadline_met = 1;
					fprintf(fptr_write, "%d\n", processes[current_process].deadline_met);
				}
				//The oposite of the conditon stated above! 
				else{
					processes[current_process].deadline_met = 0;
					fprintf(fptr_write, "%d\n", processes[current_process].deadline_met);
				}
				
				//This condition's after the current process is computed, it's to check whether the next process is ready to slot
				//into the fcfs queue.
				if (processes[current_process + 1].entry == true){
					current_process = current_process + 1;
					FCFS[0] = processes[current_process];
					processes[current_process].state = RUNNING;
					processes[current_process].run_time = time;
					processes[current_process].wait_time = processes[current_process].run_time - processes[current_process].entryTime;
					processes[current_process].completion_time = time + processes[current_process].serviceTime;
					processes[current_process].turnaround_time = processes[current_process].completion_time - processes[current_process].entryTime;
					printf("Time %d: %s is in the running state. \n", time, processes[current_process].process_name);
				}	
		}
		//end of the while loop the time is incremented by 1 
		time += 1;
	}
	
	fclose(fptr);	//closing source file
	fclose(fptr_write);	//closing output file
	
	return 0;	//end of program
}

//function definition
bool all_process_done(pcb_t arr[100], int y){
	for (int i = 0; i < y; i++){
		//if one of the process returns false, it will return false right away, if all processes are not done. 
		if (arr[i].completed == false){
			return false;
		}
	//only returning true if all processes are done. 
	}
	return true;
	
}

