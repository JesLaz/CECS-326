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
	const char *name = "/my_shm"; /*file name*/
	const int SIZE = 4096; /*file size*/
	
	int shm_fd; /*file descriptor, from shm_open()*/
	struct CLASS *shm_base; /*base address, from mmap()*/
	void *ptr; /*shm_base is fixed, ptr is movable*/

	/*create the shared memory segment*/
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	if(shm_fd == -1){
		printf("Master: Shared memory failed: %s\n", strerror(errno));
		exit(1);
	}
	/*Configure the size of the shared memroy segment*/
	ftruncate(shm_fd, SIZE);

	/*Map shared memory segment in the address space of the process*/
	shm_base = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd,0);
	shm_base->index = 0;
	if(shm_base == MAP_FAILED){
		printf("Master: Map failed: %s\n", strerror(errno));
		/*close and shm_unlink*/
		exit(1);
	}
	
	printf("Master: %d", shm_fd);
	//Write into shared memory to attempt to read from it from the slave process it's passed into
	int number;
	//sprintf(ptr, 6);
	//ptr += sizeof(int);
	int x;
	printf("Enter the number %d ", x);
	scanf("%d", &(shm_base->response[0]));
	printf("Number entered: %d: ", shm_base->response[0]);
	shm_base->index += 1;

	pid_t cpid = fork();
	if(cpid < 0){
		printf("Fork failed");
		return 1;
	}
	if(cpid == 0){
		int num = 5;
		char snum[5];
		sprintf(snum, "%d", x);
		char const *cNum = snum;
		printf("Enters child");
		execlp("./slave", "slave", cNum, NULL);
		exit(0);
	}
	/*Now write into the shared memory region by running through the child process*/
	/*Must increment the value of the pointer after each write*/
	
	//Attempt to unlink at the end
	return 0;
}
