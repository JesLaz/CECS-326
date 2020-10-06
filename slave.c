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
	printf("Inside the slave process: ");
	printf("%s", argv[1]);
	const char* name = "my_shm";
	const int SIZE = 4096;
	int shm_fd;//File descriptor
	printf("Ending the slave process");
	return 0;
}
