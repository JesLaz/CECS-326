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

CLASS *class_ptr;
void *memptr;
char *pname;
int shmid, ret;
sem_t *sem; //Named Semaphore
char *semName; //Name for semaphore
void rpterror(char *), srand(), perror(), sleep();
void sell_seats(); //sell_seats() prototypes

main(int argc, char* argv[]){
	if(argc < 3){
		fprintf(stderr, "Usage:, %s shmid\n", argv[0]);
		exit(1);
	}
	pname = argv[0];
	sscanf (argv[1], "%d", &shmid);
	semName = argv[2];

	memptr = shmat(shmid, (void *)0, 0);

	if(memptr == (char *)-1){
		rpterror((char *)"shmat failed");
		exit(2);
	}
	class_ptr = (struct CLASS *)memptr;
	if((sem = sem_open(semName, 0)) == SEM_FAILED){
		perror("Semaphore failed");
		exit(1);
	}
	sell_seats();
	ret = shmdt(memptr);
	exit(0);
}

void sell_seats(){
	int all_out = 0;
	srand((unsigned) getpid());
	while(! all_out){
		if(class_ptr->seats_left > 0){
			sleep((unsigned)rand()%5 + 1);
			class_ptr->seats_left--;
			sleep((unsigned)rand()%5 + 1);
			cout << pname << " SOLD SEAT -- "
				<< class_ptr->seats_left << " left" << endl;
		}
		else{
			all_out++;
			cout << pname << " sees no seats left" << endl;
		}
		sem_post(sem);
		sleep((unsigned)rand()%10 + 1);
	}
}

void rpterror(char* string){
	char errline[50];
	sprintf(errline, "%s %s",  string, pname);
	perror(errline);
}

