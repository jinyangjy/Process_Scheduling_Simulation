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
	
	/*
	For round robin we'll use a temporary buffer for each arrival time and service time, to read the content of each line
	this'll be understood better as we go towards the RR algorithm below 
	*/
	int temp[100];			
	int arrival_time_temp[100];
	int service_time_temp[100];
	
	char arrival_time[10];		//arival time
	char service_time[10];		//service time
	char deadline[10];		//deadline
	
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
	int nop, i; 			//nop will later on be used the number of processes, which will be differentiated from the variable above
	int total, count;		
	
	
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
			
			//pointer resets back to 0, once one of the "role"/processes has been read. 
			pointer_arrival_time = 0;
			pointer_service_time = 0;
			pointer_deadline = 0;
			pointer_vertical = 0;		
			pointer_name = 0;
			number_of_processes += 1;
		}
	}
	
	//let nop be the number of processes 
	nop = number_of_processes;
	
	/* 
	This's where the fun begins, let's go round robinhood algorithm, the inspiration for this algorithm was taken from https://www.javatpoint.com/round-robin-program-in-c
	
	In this loop we'll keep looping till the number of processes left within the "processes.txt" is 0. 
	When it has turn to 0, it goes to show that it has completed simulating all tasks. 
	*/
	while (nop != 0){
		//To show us the process is ready, the entry time has to be less than the total and the entry will be false
		if (processes[i].entryTime <= total && processes[i].entry == false){
			processes[i].entry = true;
			processes[i].state = READY;
			printf("Time %d: %s has entered the system.\n", processes[i].entryTime, processes[i].process_name);
			
		}
		
		//To show us that the process is currently running, the service time is less than the quantum time and the service time is more than 0, 
		// which shows us that it is executing the last part of the process 
		if (temp[i] <= quantum_Time && temp[i] > 0){
			processes[i].state = RUNNING;
			printf("Time %d: %s is in the running state.\n", total, processes[i].process_name);
			total += temp[i];
			temp[i] = 0;
			count = 1;
		}
		
		//Second condition to show us that it's still executing, would be if the service time is more than 0, thus telling us that the process is 
		//still executing.
		else if (temp[i] > 0){
			processes[i].state = RUNNING;
			printf("Time %d: %s is in the running state.\n", total, processes[i].process_name);
			temp[i] = temp[i] - quantum_Time;
			total += quantum_Time;
		}
		
		//We are ready to EXIT!!!! only if the service time is 0 and the count is 1
		if (temp[i] == 0 && count == 1){
			nop--;
			processes[i].completed = true;
			processes[i].state = EXIT;
			processes[i].completion_time = total;
			printf("Time %d: %s has finished execution.\n", total, processes[i].process_name);
			processes[i].turnaround_time = processes[i].completion_time - processes[i].entryTime;
			processes[i].wait_time = processes[i].turnaround_time - processes[i].serviceTime;
			if (processes[i].turnaround_time <= processes[i].remainingTime){
				processes[i].deadline_met = 1;
				
			}
			else{
				processes[i].deadline_met = 0;
			}
			count = 0;
		}
		
		/*
		if the counter is equivalent to the number of processes deducted by 1, then it'll be reset back to the value 0, 
		or it could enter the system and be executed if the arrival time is less than equals to the total. Last resort,
		it will be resetting i back to 0. 
		*/
		if (i == number_of_processes-1){
			i = 0;
		}
		else if (arrival_time_temp[i+1] <= total){
			i++;
		}
		else{
			i = 0;
		}
	}
	
	//every process which has been looped, will have it's wait_time, turnaround_time and deadline met written into the output file 
	for (int i = 0; i < number_of_processes; i++){
		fputs(processes[i].process_name, fptr_write);
		fputc(' ', fptr_write);
		fprintf(fptr_write, "%d ", processes[i].wait_time);
		fprintf(fptr_write, "%d ", processes[i].turnaround_time);
		fprintf(fptr_write, "%d\n", processes[i].deadline_met);
	}
	
	
	fclose(fptr);	//closing source file
	fclose(fptr_write); // closing output file
}

//function definition
bool all_process_done(pcb_t arr[100], int nop){
	for (int i = 0; i < nop; i++){
	//if one of the process returns false, it will return false right away, if all processes are not done. 
		if (arr[i].completed == false){
			return false;
		}	
	//only returning true if all processes are done.
	return true;
	}
}
