#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>

void display(char *prog, char *bytes, int n);

int main(){
	const char *name = "shm-example"; /*file name*/
	const int SIZE = 4096; /*file size*/

	int shm_fd; /*file descriptoor, from shm_open() */
	char *shm_base; /*base address, from mmap() */
	/*open the shared memory segment as if it was a file */
	shm_fd = shm_open(name, O_RDONLY, 0666);
	if(shm_fd == -1){
		printf("Cons: shared memory failed: %s\n", strerror(errno));
		exit(1);
	}
	/*map the shared memory segment to the address spaced of the process*/
	shm_base = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
	if(shm_base == MAP_FAILED){
		printf("cons: Map failed: %s\n", strerror(errno));
		/*close and unlink */
		exit(1);
	}
	/*read from the shared mapped memory segment */
	display("cons", shm_base, 64); /*first as bytes, then as string*/
	printf("%s", shm_base);
	/*remove the mapped shared memory segment from the address space of the process*/
	if(munmap(shm_base, SIZE) == -1){
		printf("cons: Unmap falied: %s\n", strerror(errno));
		exit(1);
	}
	/*Close the shared memory segment as if it was a file */
	if(close(shm_fd) == -1){
		printf("cons: Close failed: %s\n", strerror(errno));
		exit(1);
	}
	return 0;
}

void display(char *prog, char *bytes, int n){
	printf("display: %s\n", prog);
	for(int i = 0; i < n; i++){
		printf("%02x%c", bytes[i], ((i+1)%16) ? ' ' : '\n');
	}
	printf("\n");
}
