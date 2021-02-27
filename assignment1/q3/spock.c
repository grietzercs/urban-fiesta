/*
** spock.c -- reads from a message queue
*/

//question to answer: why does pressing CTRL + D cause this code to stop?

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct my_msgbuf {
	long mtype;
	char mtext[200];
};

int main(void)
{
	struct my_msgbuf buf;
	int msqid;
	key_t key;	

	if ((key = ftok("kirk.c", 'B')) == -1) {  /* same key as kirk.c */
		perror("ftok");
		exit(1);
	}

	if ((msqid = msgget(key, 0644)) == -1) { /* connect to the queue */
		perror("msgget");
		exit(1);
	}
	
	printf("spock: ready to receive messages, captain.\n");

	//this infinite loop stops when it receives a value less than "-1"
	//the value is from CTRL + D
	for(;;) { /* Spock never quits! */
		if (msgrcv(msqid, &buf, sizeof(buf.mtext), 0, 0) == -1) {
			perror("msgrcv"); //receive call failed, there is nothing in the queue
			exit(1);
		}
		printf("spock: \"%s\"\n", buf.mtext);
	}

	return 0;
}

