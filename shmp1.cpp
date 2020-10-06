#include "registration.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <memory.h>

using namespace std;

CLASS myCLASS = {"4321", "082620", "Operating Systems", 20};
#define NCHILD 3
int shm_init(void *);
void wait_and_wrap_up(int [], void*, int);
void rpterror(char *, char *);

int main(int argc, char *argv[]){
	int child[NCHILD], i , shmid;
	void *shm_ptr;
	char ascshmid[10], pname[14];

	shmid = shm_init(shm_ptr);
	sprintf(ascshmid, "%d", shmid);

	for(int i = 0; i < NCHILD; i++){
		child[i] = fork();
		switch (child[i]){
			case -1:
				sprintf(pname, "child%d", i+1);
				rpterror ((char *)"fork failed", pname);
				exit(1);
			case 0:
				sprintf(pname, "shmc%d", i+1);
				execl("shmc1", pname, ascshmid, (char *)0);
				rpterror ((char *)"execl failed", pname);
				exit(2);
			}
		}
		wait_and_wrap_up (child, shm_ptr, shmid);
	}

int shm_init(void *shm_ptr){
	int shmid;
	smhid = shmget(ftok(".", 'u'), sizeof(CLASS), 0600 | IPC_CREAT);
	if (shmid == -1){
		perror("shmget failed");
		exit(3);
	}
	shm_ptr = shmat(shmid, (void *)0, 0);
	if(shm_ptr == (void *)-1){
		perror("shmat failed");
		exit(4);
	}
	memcpy(shm_ptr, (void *) &myclass, sizeof(CLASS));
}

void wait_and_wrap_up(int child[], void *shm_ptr, int shmid){
	int wait_rtn, w, ch_active = NCHILD;

	while(ch_active > 0){
		wait_rtn = wait((int *) 0);
		for(int w = 0; w < NCHILD; w++){
			if(child[w] == wait_rtn){
				ch_active--;
				break;
			}
		}
	}
	cout << "Parent removing shm" << endl;
	shmdt(shm_ptr);
	shmctl(shmid, IPC_RMID, (struct shmid_ds *) 0);
	exit(0);
}

void rpterror(char *string, char *pname){
	char errline[50];
	sprintf(errline, "%s %s", string, pname);
	perror(errline);
}

