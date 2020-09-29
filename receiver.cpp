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
	long mtype;
	char msg[50];
};

void recieveMessage(int qid){
	my_msgbuf msg;
	int size = sizeof(msg) - sizeof(long);
	
	msgrcv(qid, (struct my_msgbuf*)&msg, size, 113, 0);
	cout << "Receiver received message, queue id " << qid << " through commandline partner" << endl;
	cout << "Message: " << msg.msg << endl;
}

int main(int argc, const char *argv[]){
	cout << "Receiver, " << getpid() << " beings execution" << endl;
	int msg_qid = atoi(argv[1]);
	recieveMessage(msg_qid);
	cout << "Receiver terminates" << endl;
	return (0);
}
	
