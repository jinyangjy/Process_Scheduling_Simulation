/*
Name: Jin Yang Chong
Student ID: 32893043
Start Date: 1st October
Last Edit: 9th October
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum{
	READY,
	RUNNING,
	EXIT
}process_state_t;

/*
Sequence for reading, opening and writing for each processes has very much similar process, 
but for round robin there's an additional variable for quantum time. The process of reading to writing from a file to another file 
was taken from the previous assignment, thus the similarity for each scheduling process will be pretty much the same except for 
the process where the algorithm's being exeecuted, when scheduling a process. 
*/
typedef struct{
	char process_name[11];
	
	int entryTime;		//The time process entered system	
	int serviceTime;	//The total CPU time required by the process
	int remainingTime;	// Remaining service time until completion
	
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
	FILE *fptr;		//file pointer
	FILE *fptr_write;	//file pointer for output file, writing to output file
	
	char buffer[2086];	//where the source file is being read 
	
	int temp[100];
	int arrival_time_temp[100];
	int service_time_temp[100];
	
	char arrival_time[10];
	char service_time[10];
	char deadline[10];
	
	int time = 0;
	int current_process;
	
	int quantum_Time = 2;
	
	int pointer_name = 0;		//keeping track the index of the buffer for process name
	int pointer_arrival_time = 0;	//keeping track the index of the buffer arrival time
	int pointer_service_time = 0;	//keeping track the index of the buffer service time....
	int pointer_deadline = 0;	//keeping track the index of the buffer deadline
	
	int pointer_vertical = 0;	//keeping track which line we're reading 
	int pointer_process = 0;	//keeping track which row we're reading
	int number_of_processes = 0;	//number of processes provided by the source file
	int nop, i;			//nop will later on be used the number of processes, which will be differentiated from the variable above, while nop is used for looping through the schedule
	int total;			//total will be considered as the time
	int min_deadline, min_idex;	//the minimum deadline will be called min_deadline and the minimum index of the deadline will be called min_index
	pcb_t processes[100];		//array obstruct, assume at most the number of processes will be 100
	
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
	/*
	if there are no errors found, we'll open the "processes.txt" file and read it, then writing the results formulated through
	the algorithm to the "output.txt" file
	*/
	fread(&buffer, sizeof(char), 2086, fptr);
	fptr_write = fopen("output.txt", "w");
	
	
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
			//accesses to the attributes within the process_name
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
			arrival_time_temp[pointer_process] = atoi(arrival_time);
			processes[pointer_process].serviceTime = atoi(service_time);
			service_time_temp[pointer_process] = atoi(service_time);
			temp[pointer_process] = atoi(service_time);
			processes[pointer_process].remainingTime = atoi(deadline);
			
			// entry and completed will be set to false initially before the next line of process comes into play 
			processes[pointer_process].entry = false;
			processes[pointer_process].completed = false;
			
			/*
			Pointer process will never be reset back to 0, because each time we're done reading the file, it points
			to the next process.
			*/ 
			pointer_process += 1;
			
			//The pointers of which 
			pointer_arrival_time = 0;
			pointer_service_time = 0;
			pointer_deadline = 0;
			pointer_vertical = 0;
			pointer_name = 0;
			
			//number of processes increments each time a new line process is to be read 
			number_of_processes += 1;
		}
	}
	
	//we'll let nop be the number of processes 
	nop = number_of_processes;
	
	/*
	In this loop we'll keep looping till the number of processes left within the "processes.txt" is 0. 
	When it has turn to 0, it goes to show that it has completed simulating all tasks. 
	*/
	while (nop != 0){
		//will loop from 0, till the number of processes that are located in the source file. 
		//for loop to check for process.
		for (int i = 0; i < number_of_processes; i++){
			//will check each and every entrytime/ arrivial time is equals to the time and process..
			//to check whether the process's pass into the system. Once it is passed into .entry it 
			// will turn true. 
			if (processes[i].entryTime == time && processes[i].entry == false){
				processes[i].entry = true;
				processes[i].state = READY;
				printf("Time %d: %s has entered the system.\n", time, processes[i].process_name);
			}
			//check whether it is running, if it isn't running, it's telling us that it's still at the first process 
			if (processes[min_idex].state != RUNNING){
				//if the minimum deadline is 0, the first process will then be the minimum deadline
				if (min_deadline == 0 && processes[i].entry == true && processes[i].completed == false){
					min_deadline = processes[i].remainingTime;
					min_idex = i;
				
				}
				//if minimum deadline is not 0, then the process which is smaller than the minimum deadline, will be substituted with the new process.
				else if (min_deadline != 0 && processes[i].entry == true && processes[i].completed == false){
					if (min_deadline > processes[i].remainingTime){
						min_deadline = processes[i].remainingTime;
						min_idex = i;
					}
				}
				
			}		
		
		}
		
		//if the completion time of the processes minimum index is 0, then the completion time of the processeses 
		//minimum index will then add up the time and the service time 
		if (processes[min_idex].completion_time == 0){
			processes[min_idex].state = RUNNING;
			printf("Time %d: %s is in the running state.\n", time , processes[min_idex].process_name);
			processes[min_idex].completion_time = time + processes[min_idex].serviceTime;
			
		}
		//if completion time of the processes minimum index is equivalent to the time, it goes to show that the 
		// program has successfully finish it's exeuction and ready to exit. 
		if (processes[min_idex].completion_time == time){
			processes[min_idex].state = EXIT;
			processes[min_idex].completed = true;
			printf("Time %d: %s has finished execution.\n", time, processes[min_idex].process_name);
			
			//formulas for turnaround time and wait time
			processes[min_idex].turnaround_time = processes[min_idex].completion_time - processes[min_idex].entryTime;
			processes[min_idex].wait_time = processes[min_idex].turnaround_time - processes[min_idex].serviceTime;
			
			//if the turnaround time is <= to the remaining time then the deadline has been met
			if (processes[min_idex].turnaround_time <= processes[min_idex].remainingTime){
				processes[min_idex].deadline_met = 1;
			}
			else{
				processes[min_idex].deadline_met = 0;
			}
			//we'll then be resetting the minimum deadline and also decerement the number of processes needed to be executed 
			min_deadline = 0;
			nop--;
			
			//we'll loop through every process to find the minimum deadline
			for (int i = 0; i < number_of_processes; i++){
				//if the minimum deadline is 0, entry is true and not completed
				//then the minimum index is thus the process
				if (min_deadline == 0 && processes[i].entry == true && processes[i].completed == false){
					min_deadline = processes[i].remainingTime;
					min_idex = i;
				
				}
				else if (min_deadline != 0 && processes[i].entry == true && processes[i].completed == false){
					if (min_deadline > processes[i].remainingTime){
						min_deadline = processes[i].remainingTime;
						min_idex = i;
					}
				}
			}
			processes[min_idex].state = RUNNING;
			printf("Time %d: %s is in the running state.\n", time , processes[min_idex].process_name);
			processes[min_idex].completion_time = time + processes[min_idex].serviceTime;
		}
		//after each iteration of the loop we'll be increasing the time taken
		time += 1;
	}
	
	// every process which has been looped, will have it's wait_time, turnaround_time and deadline met written into the output file 
	for (int i = 0; i < number_of_processes; i++){
		fputs(processes[i].process_name, fptr_write);
		fputc(' ', fptr_write);
		fprintf(fptr_write, "%d ", processes[i].wait_time);
		fprintf(fptr_write, "%d ", processes[i].turnaround_time);
		fprintf(fptr_write, "%d\n", processes[i].deadline_met);
	}
		
	fclose(fptr);	//closing source file
	fclose(fptr_write);	//closing output file
	return 0;
}

//function definition
bool all_process_done(pcb_t arr[100], int nop){
	//if one of the process returns false, it will return false right away, if all processes are not done.
	for (int i = 0; i < nop; i++){
		if (arr[i].completed != true){
			return false;
		}
	}
	//only returning true if all processes are done.
	return true;
}
