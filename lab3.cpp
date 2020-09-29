#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <iostream>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

struct my_msgbuf{
	long mtype; //required
	char greeting[50];
};

void child_proc_one(int qid){
	my_msgbuf msg;
	int size = sizeof(msg) - sizeof(long);
	
	msgrcv(qid, (struct my_msgbuf*)&msg, size, 113, 0);
	
	cout << getpid() << ": gets message" << endl;
	cout << "Message: " << msg.greeting << endl;

	strcat(msg.greeting, " and Adios");
	cout << getpid() << ": sends reply" << endl;
	msg.mtype = 114;
	msgsnd(qid, (struct my_msgbuf*)&msg, size, 0);
	cout << getpid() << ": now exits" << endl;
}

void child_proc_two(int qid){
	my_msgbuf  msg;
	int size = sizeof(msg) - sizeof(long);
	
	//sending message that will never be read
	msg.mtype = 12;
	strcpy(msg.greeting, "Fake message");
	msgsnd(qid, (struct my_msgbuf*)&msg, size, 0);

	//prepare message to send
	strcpy(msg.greeting, "Hello there");

	cout << getpid() << ": sends greeting" << endl;
	msg.mtype = 113;
	msgsnd(qid, (struct my_msgbuf*)&msg, size, 0);

	msgrcv(qid, (struct my_msgbuf*)&msg, size, 114, 0);
	cout << getpid() << ": gets reply" << endl;
	cout << "reply: " << msg.greeting << endl;
	cout << getpid() << ": now exits" << endl;
}

int main(int argc, char** argv){
	int qid = msgget(IPC_PRIVATE, IPC_EXCL|IPC_CREAT|0600);

	//spawning two child processses
	pid_t cpid = fork();
	if(cpid == 0){
		child_proc_one(qid);
		exit(0);
	}
	
	cpid = fork();
	if(cpid == 0){
		child_proc_two(qid);
		exit(0);
	}

	while(wait(NULL) != -1);

	msgctl(qid, IPC_RMID, NULL);

	cout << "parent proc: " << getpid() << " now exits" << endl;
	
	exit(0);
}

