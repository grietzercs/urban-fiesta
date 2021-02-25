#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#define SHMSZ 27

int main(void) {
    //creates shared memory
    int shmStrID = shmget(7999, SHMSZ, 0666 | IPC_CREAT);
    int shmIntID = shmget(9999, SHMSZ, 0666 | IPC_CREAT);
    
    //attaches string to the shared memory
    int *sharedInt = (int *) shmat(shmIntID, NULL, 0);
    char *sharedStr = (char *) shmat(shmStrID, NULL, 0);

    *sharedInt = 1;

    while(*sharedInt != 2) {
        sleep(1);
    }

    sleep(1);
    printf("Process B wrote: %s", sharedStr); printf("\n");

    while(*sharedInt != 3) {
        sleep(1);
    }

    printf("Process C wrote: %s", sharedStr); printf("\n");
    
    printf("Value of sharedInt: %d", *sharedInt); printf("\n");

    printf("GoodBye"); printf("\n");
    shmdt(sharedInt); shmdt(sharedStr);
    shmctl(shmIntID, IPC_RMID, NULL); shmctl(shmStrID, IPC_RMID, NULL);
    return 0;
}