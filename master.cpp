#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <iostream>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

using namespace std;

int main(){
	int qid = msgget(IPC_PRIVATE, IPC_EXCL|IPC_CREAT|0600);
	cout << "Master, PID: " << getpid() << " begins execution" << endl;
	cout << "Master acquired a message queue, ID " << qid << endl;
	
	pid_t cpid = fork();
	if(cpid < 0){
		cout << "Fork Failed" << endl;
		return 1;
	}
	else if(cpid == 0){
		cout << "enters sender" << endl;
		string qidString = to_string(qid);
		char const *qidc = qidString.c_str();
		cout << "qidc in sender " << qidc << endl;
		execlp("./sender", "sender", qidc, NULL);
		sleep(1);
		exit(0);
	}

	cpid = fork();
	if(cpid < 0){
		cout << "Fork Failed" << endl;
		return 1;
	}
	else if(cpid == 0){
		cout << "enters receiver" << endl;
		string qidString = to_string(qid);
		char const *qidc = qidString.c_str();
		cout << "qidc in receiver " << qidc << endl;
		execlp("./receiver", "receiver", qidc, NULL);
		exit(0);
	}
	
	cout << "Master waits for both child processes to terminate" << endl;
	while(wait(NULL) != -1);

	msgctl(qid, IPC_RMID, NULL);
	cout << "Master recevied terminatoin signals from both child processes, removed message queue, and terminates " << endl;
	return 0;
}
