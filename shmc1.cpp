#include "registration.h"
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <memory.h>
#include <pthread.h>
#include <semaphore.h>

using namespace std;

CLASS *class_ptr;//Pointer for our struct CLASS
void *memptr;//Pointer for shared memory segment
char *pname;//Process name
int shmid, ret;//ID for shared memory and the return var for detaching the segment
void rpterror(char *), srand(), perror(), sleep();
void sell_seats(); //sell_seats() prototypes

int main(int argc, char* argv[]){
	//Checks if there are at least 2 arguments
	if(argc < 2){
		fprintf(stderr, "Usage:, %s shmid\n", argv[0]);
		exit(1);
	}
	//Process name is the first argument
	pname = argv[0];
	//Convert the shared memory segment ID to decimal int 
	sscanf (argv[1], "%d", &shmid);

	//Attach the first address space of the shm segment to our memory pointer
	memptr = shmat(shmid, (void *)0, 0);

	//Check to see if the shm segment attachment failed
	if(memptr == (char *)-1){
		rpterror((char *)"shmat failed");
		exit(2);
	}

	//Assign memptr to the struct CLASS pointer
	class_ptr = (struct CLASS *)memptr;

	//Call the funtion to sell the available seats
	sell_seats();
	//Detach shm segment from memptr, value of ret details if the attempt was successful or not
	ret = shmdt(memptr);
	exit(0);
}

//Helper function to sell the seats within the class
void sell_seats(){
	int all_out = 0;
	//Creates a random number using the pid as the seed
	srand((unsigned) getpid());
	while(! all_out){
		if(class_ptr->seats_left > 0){
			//Puts the process to sleep for a random number between 1 and 5
			sleep((unsigned)rand()%5 + 1);
			//"Sells" a seat
			class_ptr->seats_left--;
			//Puts the process to sleep for a random number between 1 and 5
			sleep((unsigned)rand()%5 + 1);
			//Indicates that a seat was sold and shows remaining amount
			cout << pname << " SOLD SEAT -- "
				<< class_ptr->seats_left << " left" << endl;
		}
		else{
			all_out++;
			cout << pname << " sees no seats left" << endl;
		}
		//Puts the process to sleep for a random number betweeen 1 and 10
		sleep((unsigned)rand()%10 + 1);
	}
}

/*Helper function to print the resulting error message caused by 
the given parameter*/
void rpterror(char* string){
	char errline[50];
	sprintf(errline, "%s %s",  string, pname);
	perror(errline);
}

