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
#include <stdio.h>
#include <memory.h>
#include <pthread.h>
#include <fcntl.h>
//#include <semaphore.h>

	
//Jessie Lazo
	
int main(int argc, char *argv[]){
	const char *shm_name;
	shm_name = argv[3];
	const int SIZE = 4096;

	int shm_fd;
	struct ACCOUNTS *base;

	printf("Master begins execution\n");

	//Create shared memory segment
	shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
	//error handling
	if(shm_fd == -1){
		printf("Master: Shared memory failed: %s\n", strerror(errno));
		exit(1);
	}
	ftruncate(shm_fd, SIZE);
	base = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

	//error handling
	if(base == MAP_FAILED){
		printf("Master: Map failed: %s\n", strerror(errno));
		/*Close and shm_unlink*/
		exit(1);
	}

	base->nAccounts = 2;
	base->accounts[0] = atoi(argv[1]);
	base->accounts[1] = atoi(argv[2]);
	
	printf("Account 1 Balance: $%d\n", base->accounts[0]);
	printf("Account 2 Balance: $%d\n", base->accounts[1]);
	
	
	//Previously argv[4] was the commandline argument used as the name for semaphore 1
	//Initialize semaphore at index 0 of accountSems
	sem_init(&(base->accountSems[0]), 1, 1);

	/*Used for error checking but gives warnings so I commented out
	if(init1 == -1){
		perror("Unnamed semaphore 1 initialization failed");
		return 0;
	}
	*/

	//Previously argv[5] was the commandline argument used as the name for semaphore 2
	//Initialize semaphore at index 1 of accountSems
	sem_init(&(base->accountSems[1]), 1, 1);
	/*
	if(init2 == -1){
		perror("Unnamed semaphore 2 initialization failed");
		return 0;
	}
	*/
	
	/*
	semaphore2 = sem_open(argv[5], O_CREAT, 0644, 1);
	if(semaphore2 == SEM_FAILED){
		perror("Semaphore initialization failed");
		exit(1);
	}
	*/

	printf("Forking Child 1\n");
	pid_t pid;
	pid = fork();
	if(pid < 0){
		fprintf(stderr, "Fork Failed\n");
		return 1;
	}
	else if(pid == 0){
		//char *args[] = {"./transfer", argv[3], argv[4], argv[5], "1", "2", "50", NULL};
		char *args[] = {"./transfer", argv[3], "1", "2", "50", NULL};
		execv("transfer", args);
	}
	

	printf("Account 1 Balance: $%d\n", base->accounts[0]);
	printf("Account 2 Balance: $%d\n", base->accounts[1]);

	printf("Forking Child 2\n");
	pid = fork();
	if(pid < 0){
		fprintf(stderr, "Fork Failed\n");
		return 1;
	}
	else if(pid == 0){
		//char *args[] = {"./transfer", argv[3], argv[5], argv[4], "2", "1", "25", NULL};
		char *args[] = {"./transfer", argv[3], "2", "1", "25", NULL};
		execv("transfer", args);
	}
	wait(NULL);
	wait(NULL);

	printf("Account 1 Balance: $%d\n", base->accounts[0]);
	printf("Account 2 Balance: $%d\n", base->accounts[1]);

	if(munmap(base, SIZE) == -1){
		printf("Master Unmap Failed %s\n", strerror(errno));
		exit(1);
	}

	if(close(shm_fd) == -1){
		printf("Master Close Failed %s\n", strerror(errno));
		exit(2);
	}

	if(shm_unlink(shm_name) != 0){
		perror("Master unlink() Failed");
		exit(3);
	}
	
	/*
	sem_close(semaphore1);
	sem_unlink(argv[4]);
	sem_close(semaphore2);
	sem_unlink(argv[5]);
	*/
	printf("Destroying unnamed semaphores");

	int des1 = sem_destroy(&(base->accountSems[0]));
	if(des1 == -1){
		printf("Error destroying unnamed seamphore 1");	
		return 0;
	}
	int des2 = sem_destroy(&(base->accountSems[1]));
	if(des2 == -1){
		printf("Error destroying unnamed semaphore");
		return 0;
	}
	printf("Master Ending\n");

	return 0;
}
