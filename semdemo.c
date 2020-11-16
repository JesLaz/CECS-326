/*
POSIX Unnamed Semaphores

sem_init()
sem_wait()
sem_post()
sem_destroy()

Declare semaphroe and initialize to 1
Loop predefined number of times, lock, sleep, unlock each iteration
Destroy semaphore

*/

#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>

int main(){
	sem_t mySem;
	//sem_t mySem;
	//loop to initialize each semaphore in array
	//for i to numAccounts
	//sem_init(&mySem[i], 1, 1)

	int init = sem_init(&mySem, 1, 1);
	if(init == -1){
		printf("Error initializing");
		return 0;
	}
	
	for(int i = 0; i < 10; ++i){
		sem_wait(&mySem);
		printf("%d Waiting\t", getpid());
		sleep(1);
		sem_post(&mySem);
		printf("%d Signal\n", getpid());
	}
	return 0;
}
