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

void display(char *prog, char *bytes, int n);

int main(){
	const char *name = "/shm-example"; /* file name */
	const int SIZE = 4096; /* file size */
	const char *message0 = "Studying ";
	const char *message1 = "Operating Systems ";
	const char *message2 = "Is Fun!";
	const char *msg_end = "\n";

	int shm_fd; /*file descriptor, from shm_open()*/
	char *shm_base; /*base address, from mmap()*/
	void *ptr; /*shm_base is fixed, ptr is movable*/
	
	/*create the shared memory segment */
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	if(shm_fd == -1){
		printf("prod: Shared memory failed: %s\n", strerror(errno));
		exit(1);
	}
	/*Configure the size of the shard memory segment */
	ftruncate(shm_fd, SIZE);

	/*Map shared memory segment in the address space of the process */
	shm_base = mmap(0, SIZE, PROT_READ |PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if(shm_base == MAP_FAILED){
		printf("prod: Map failed: %s\n", strerror(errno));
		/*close and shm_unlink*/
		exit(1);
	}
	/*Now write to the shared memory region*/
	/*Note we must increment value of ptr after each write*/
	display("prod: ", shm_base, 64);
	ptr = shm_base;
	sprintf(ptr,"%s", message0);
	ptr += strlen(message0);
	sprintf(ptr,"%s", message1);
	ptr += strlen(message1);
	sprintf(ptr,"%s", message2);
	ptr += strlen(message2);
	sprintf(ptr,"%s", msg_end);
	display("prod: ", shm_base, 64);

	/*remove the mapped memory segment from the address space of the process*/
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

void display(char *prog, char *bytes, int n){
	printf("display: %s\n", prog);
	for(int i = 0; i < n; i++){
		printf("%02x%c", bytes[i], ((i+1)%16) ? ' ' : '\n');
	}
	printf("\n");
}
