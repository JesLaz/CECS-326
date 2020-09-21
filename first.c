#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

int main(){
pid_t pid;

	/*for a child process*/
	pid = fork();
	
	if (pid < 0) {/*Error Occurred */
		fprintf(stderr, "Fork Failed");
		return 1;
	}
	else if (pid == 0) {/*Child Process*/
		execlp("/bin/ls", "ls", NULL);
	}
	else { /*Parent Process*/
		/*Parent will wait for the child to complete */
		wait(NULL);
		printf("Child Complete");
	}
	return 0;
}