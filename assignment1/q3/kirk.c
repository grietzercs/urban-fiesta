/*
** kirk.c -- writes to a message queue
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

//this is the item that is passed into the mailbox
struct my_msgbuf {
	long mtype;
	char mtext[200];
};

int main(void)
{
	struct my_msgbuf buf;
	int msqid;
	key_t key;

	//ftok generates system defined unique key
	if ((key = ftok("kirk.c", 'B')) == -1) {
		perror("ftok");
		exit(1);
	}

	//msgget creates message queue, equivalent to shm_get
	if ((msqid = msgget(key, 0644 | IPC_CREAT)) == -1) {
		perror("msgget");
		exit(1);
	}
	
	printf("Enter lines of text, ^D to quit:\n");

	//don't worry about this line
	buf.mtype = 1; /* we don't really care in this case */

	//expects string input from the user
	//puts strings into message queue (separated by new line)
	while(fgets(buf.mtext, sizeof buf.mtext, stdin) != NULL) {
		int len = strlen(buf.mtext);

		/* ditch newline at end, if it exists and replaces with terminal char*/
		if (buf.mtext[len-1] == '\n') buf.mtext[len-1] = '\0';

		//send manipulated strings into the message queue
		if (msgsnd(msqid, &buf, len+1, 0) == -1) /* +1 for '\0' */
			perror("msgsnd");
	}

	if (msgctl(msqid, IPC_RMID, NULL) == -1) {
		perror("msgctl");
		exit(1);
	}

	return 0;
}

