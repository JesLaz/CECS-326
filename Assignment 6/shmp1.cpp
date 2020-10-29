#include "registration.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <memory.h>
#include <pthread.h>
#include <fcntl.h>
#include <semaphore.h>

#Jessie Lazo
using namespace std;

CLASS myCLASS = {"4321", "082620", "Operating Systems", 20};
#define NCHILD 3
#define semName "mySem" //create name for semaphore


//Function prototypes 
int shm_init(void *);
void wait_and_wrap_up(int [], void*, int, sem_t *);
void rpterror(char *, char *);


main(int argc, char *argv[]){
	/*creates var to stores pid for child processes, 
	i for the process number,
	and the shared memory segment ID*/
	int child[NCHILD], i , shmid;

	sem_t *sem; //Pointer to semaphore for use in sem_post

	sem = sem_open(semName, O_CREAT, 0644, 1); //Initialize semaphore and assign to variable sem
	if(sem == SEM_FAILED){
		perror("Error Initializing Semaphore");
		exit(1);
	}

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
				sprintf(pname, "shmc%d", i+1);
				execl("shmc1", pname, ascshmid, semName, (char *)0); //add semaphore name to argument list
				rpterror ((char *)"execl failed", pname);
				exit(2);
			}
		}
		//Utilies the helper function to wait for all process then properly terminate and detach memory
		wait_and_wrap_up (child, shm_ptr, shmid, sem); //add the semaphore pointer to method call
	}

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
/*For assignment 6, add the process of closing and unlinking the semaphore to the
method*/
void wait_and_wrap_up(int child[], void *shm_ptr, int shmid, sem_t *sem){
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

	//Added code
	sem_close(sem); //close the semaphore referred to by the pointer
	sem_unlink(semName); //remove a named semaphore
	
	exit(0);
}

/*Helper function to print the resulting error message caused by 
the given parameter*/
void rpterror(char *string, char *pname){
	char errline[50];
	sprintf(errline, "%s %s", string, pname);
	perror(errline);
}
