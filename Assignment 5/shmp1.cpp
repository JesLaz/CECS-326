#include "registration.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <memory.h>
#include <pthread.h>
#include <fcntl.h>
#include <semaphore.h>

using namespace std;

CLASS myCLASS = {"4321", "082620", "Operating Systems", 20};
#define NCHILD 3

//Function prototypes 
int shm_init(void *);
void wait_and_wrap_up(int [], void*, int);
void rpterror(char *, char *);


main(int argc, char *argv[]){
	int pid = getpid();
	key_t key = ftok(".", 'u');
	
	int ct = semctl(327680, 0, IPC_RMID);
	int semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
	char semidChar[10];
	sprintf(semidChar, "%d", semid);
	int ctl = semctl(semid, 0, SETVAL, 1);//init value

	/*creates var to stores pid for child processes, 
	i for the process number,
	and the shared memory segment ID*/
	int child[NCHILD], i , shmid;

	void *shm_ptr;//Pointer to shared memory segment
	char ascshmid[10], pname[14];//creates char array for shmid and for the process name

	shmid = shm_init(shm_ptr);//Initializes shared memory segment and assigns identifier
	sprintf(ascshmid, "%d", shmid);
	
	//Create NCHILD processes
	for(i = 0; i < NCHILD; i++){
		//Create process
		child[i] = fork();
		//Method for determining if the creation of the child process is successful or not
		switch (child[i]){
			//Child process failed
			case -1:
				sprintf(pname, "child%d", i+1);
				rpterror ((char *)"fork failed", pname);
				exit(1);
			//Child process successful
			case 0:
				//pass semid to child through exec
				sprintf(pname, "shmc%d", i+1);
				execl("shmc1", pname, ascshmid, semidChar, (char *)0);
				rpterror ((char *)"execl failed", pname);
				exit(2);
			}
		
		//Remove semaphore set
		
	}//end for loop
		//Utilies the helper function to wait for all process then properly terminate and detach memory
	wait_and_wrap_up (child, shm_ptr, shmid);
ctl = semctl(semid, 0, IPC_RMID);
		if(ctl == -1){
			cout << "Error removing semaphore" << endl;
	
		}
}//end main

/*Initialize shared memory segment and assign it to the given pointer
and returns the ID of the created memory*/
int shm_init(void *shm_ptr){
	int shmid;
	//Creates a shared memory segment that has size of strcut CLASS and permission flag to create
	shmid = shmget(ftok(".", 'u'), sizeof(CLASS), 0600 | IPC_CREAT);

	if (shmid == -1){
		perror("shmget failed");
		exit(3);
	}
	//Attaches the shared memory to the passed in pointer
	shm_ptr = shmat(shmid, (void *)0, 0);

	//Error catching if attach failed
	if(shm_ptr == (void *)-1){
		perror("shmat failed");
		exit(4);
	}
	//Data of struct CLASS is copied into the pointer as well as the size of the data in the struct
	memcpy(shm_ptr, (void *) &myCLASS, sizeof(CLASS));
	return(shmid);
}

/*Helper function that waits for all the running child processes to terminate then 
detaches the shared memory segment from its pointer and then removes that segment*/
void wait_and_wrap_up(int child[], void *shm_ptr, int shmid){
	int wait_rtn, w, ch_active = NCHILD;

	//Continues while there are still active processes running
	while(ch_active > 0){
		//Waits for process termination
		wait_rtn = wait((int *) 0);
		//Decrements ch_active when a processes is terminated
		for(w = 0; w < NCHILD; w++){
			if(child[w] == wait_rtn){
				ch_active--;
				break;
			}
		}
	}
	cout << "Parent removing shm" << endl;
	//Detaches the shared memory segment from the given pointer
	shmdt(shm_ptr);
	//Removes the shared memory segment identified by its ID and destroys it and its struct
	shmctl(shmid, IPC_RMID, (struct shmid_ds *) 0);

	exit(0);
}

/*Helper function to print the resulting error message caused by 
the given parameter*/
void rpterror(char *string, char *pname){
	char errline[50];
	sprintf(errline, "%s %s", string, pname);
	perror(errline);
}

