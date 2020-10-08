#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/ipc.h>
#include "myShm.h"

int main(int argc, char *argv[]){
	const char *name = "/my_shm"; /*file name*/
	const int SIZE = 4096; /*file size*/
	
	int shm_fd; /*file descriptor, from shm_open()*/
	struct CLASS *shm_base; /*base address, from mmap()*/
	//void *ptr; /*shm_base is fixed, ptr is movable*/

	/*create the shared memory segment*/
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	//error handling
	if(shm_fd == -1){
		printf("Master: Shared memory failed: %s\n", strerror(errno));
		exit(1);
	}
	/*Configure the size of the shared memory segment*/
	ftruncate(shm_fd, SIZE);

	/*Map shared memory segment in the address space of the process*/
	shm_base = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd,0);

	//error handling
	if(shm_base == MAP_FAILED){
		printf("Master: Map failed: %s\n", strerror(errno));
		/*close and shm_unlink*/
		exit(1);
	}

	pid_t pid;
	printf("Master created %s", argv[1]);
	printf(" child processes to execute slave\n");
	int y = atoi(argv[1]);
	/*Now write into the shared memory region by running through the child process*/
	for(int i = 0; i < y; i++){
		pid = fork();
		if(pid < 0){
			printf("Fork failed");
			return 1;
		}
		if(pid == 0){
			int num = i+1;
			char snum[5];
			sprintf(snum, "%d", num);
			printf("Enters child");
			char *cmd = "./slave";
			char *argv[4];
			argv[0] = "./slave";
			argv[1] = "my_shm";
			argv[2] = snum;
			argv[3] = NULL;
			execvp(cmd, argv);
			exit(0);
		}
		else{
			//Allow child processes to terminate before parent process continues
			wait(NULL);
		}
	}
	printf("Master recived termination signals from all %s", argv[1]);
	printf(" processes\n");
	printf("Content of shared memory filled by child processes:\n");
	for(int i = 0; i < y; i++){
		printf("Array element %d", i);
		printf(" : %d", shm_base->response[i]);
		printf("\n");
	}
	
	
	printf("Master removed shared memory segment, and is exiting\n");
	//Attempt to unlink at the end
	if(munmap(shm_base, SIZE) == -1){
		printf("prod: Unmap failed: %s\n", strerror(errno));
		exit(1);
	}
	/* close shared memory segment as if it was a file */
	if (close(shm_fd) == -1){
		printf("prod: Close failed: %s\n", strerror(errno));
		exit(1);
	}
	return 0;
}
