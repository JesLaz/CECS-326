/* accounts.h */
/* Header file to be used with master.c and transfer.c */
struct ACCOUNTS{
	int nAccounts; /* number of active accounts */
	int accounts[50]; /* space to hold up to 50 accounts */
	sem_t accountSems[50]; /*Create an array of semaphores so that children won't need to be 
		passed the name of the semaphores*/
	/*
	nAccounts = 2
	accounts[200, 400, NUll NULL NULLL ...]
	
	sem_wait(accoutSems[0])
	sem_post(accountSems[0])
	*/
};
