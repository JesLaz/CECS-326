#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/ipc.h>
#include "myShm.h"

int main(int argc, char *argv[]){
	printf("Slave begins execution\n");
	printf("I am child number %s", argv[2]);
	printf(", received memory name %s", argv[1]);

	const int SIZE = 4096;//File size
	/*As slave will do writing into the shared memory shm_open will be called with the flag
	O_RDWR as opposed to O_RDONLY otherwise write permission will not be granted*/
	int shm_fd = shm_open(argv[1], O_CREAT | O_RDWR, 0666);
	struct CLASS *shm_base; //Base address from mmap()
	//error handling
	if(shm_fd == -1){
		printf("Slave: shared memory failed: %s\n", strerror(errno));
		exit(1);
	}
	shm_base = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if(shm_base == MAP_FAILED){
		printf("Slave: Map failed: %s\n", strerror(errno));
		exit(1);
	}
	
	int x = atoi(argv[2]);//Convert the string argument to an int as response in int[]
	//Assign the the value of the child number to the response array
	shm_base->response[x-1] = x;

	printf("\nI have written my child number to shared memory\n");
	//Must increment the value of the pointer after each write
	shm_base->index ++;
	//Unmap and close access to the shared memory before the child process finishes
	if(munmap(shm_base, SIZE) == -1){
		printf("Slave: Unmap failed %s\n", strerror(errno));
		exit(1);
	}
	if(close(shm_fd) == -1){	
		printf("Slave: Close failed: %s\n", strerror(errno));	
		exit(1);
	}
	printf("Slave closed access to shared memory and terminates\n");
	return 0;
}
