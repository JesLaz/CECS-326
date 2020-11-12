#include "accounts.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
//#include <iostream>
#include <stdio.h>
#include <memory.h>
#include <pthread.h>
#include <fcntl.h>
#include <semaphore.h>
	
//Jessie Lazo

void transfer(sem_t*, sem_t*, struct ACCOUNTS*, int, int, int);
	
int main(int argc, char *argv[]){
	printf("Child Beginning Execution\n");
	char *shmName = argv[1];
	char *sName1 = argv[2];
	char *sName2 = argv[3];
	int account1 = (atoi(argv[4])) - 1;
	int account2 = (atoi(argv[5])) - 1;
	int ammount = atoi(argv[6]);

	const int SIZE = 4096;
	int shm_f;
	char *shm_base;

	int shm_fd = shm_open(shmName, O_RDWR, 0666);
	if(shm_fd == -1){
		printf("Transfer Shared Memory Failed %s\n", strerror(errno));
		exit(1);
	}

	shm_base = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if(shm_base == MAP_FAILED){
		printf("Transer Map Failed %s\n", strerror(errno));
		exit(1);
	}

	struct ACCOUNTS* accounts = (struct ACCOUNTS*) shm_base;
	
	sem_t *semaphore1;
	sem_t *semaphore2;

	semaphore1 = sem_open(sName1, 0);
	if(semaphore1 == SEM_FAILED){
		perror("Semaphore 1 Open Failed");
		exit(1);
	}
	sleep(2);
	semaphore2 = sem_open(sName2, 0);
	if(semaphore2 == MAP_FAILED){
		perror("Semaphore 2 Open Failed");
		exit(1);
	}



	transfer(semaphore1, semaphore2, accounts, account1, account2, ammount);

	if(munmap(shm_base, SIZE) == -1){
		printf("Transfer Unmap Failed: %s\n", strerror(errno));
		exit(1);
	}
	if(close(shm_fd) == -1){
		printf("Transfer Close Failed: %s\n", strerror(errno));
		exit(1);
	}
	return 0;
}

void transfer(sem_t *s1, sem_t *s2, struct ACCOUNTS *a, int sender, int receiver, int ammount){
	sem_wait(s1);
	printf("Removing $%d from Account %d\n", ammount, sender + 1);
	a->accounts[sender] = (a->accounts[sender]) - ammount;
	sem_post(s1);
	sleep(3);
	sem_wait(s2);
	printf("Adding $%d to Account %d\n", ammount, receiver + 1);
	a->accounts[receiver] = (a->accounts[receiver]) + ammount;
	sem_post(s2);
}
