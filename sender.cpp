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

void sendMessage(int qid){
	my_msgbuf msg;
	int size = sizeof(msg) - sizeof(long);
	msg.mtype = 113;
	string message;
	
	cout << "Sender received message queue id " << qid << endl;
	cout << "Sender: Please input your message: " << endl;
	cin >> message;
	strcpy(msg.msg, message.c_str());
	
	msgsnd(qid, (struct my_msgbuf*)&msg, size, 0);
	cout <<  "Sender sent message to message queue" << endl;
}

int main(int argc, const char *argv[]){
	cout << "Sender, PID: " << getpid() << " beings execution" << endl;
	int msg_qid = atoi(argv[1]);//Convert the command line argument into an integer
	sendMessage(msg_qid);
	cout << "Sender terminates" << endl;
	return 0;
}
