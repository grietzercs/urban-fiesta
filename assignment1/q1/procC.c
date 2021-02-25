#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#define SHMSZ 27

int main(void) {
    int shmStrID = shmget(7999, SHMSZ, 0666 | IPC_CREAT);
    int shmIntID = shmget(9999, SHMSZ, 0666 | IPC_CREAT);

    int *sharedInt = (int *) shmat(shmIntID, NULL, 0);
    char *sharedStr = (char *) shmat(shmStrID, NULL, 0);

    while(*sharedInt != 2) {
        sleep(1);
    }
    
    sleep(1);
    strcpy(sharedStr, "memory");
    sleep(1);

    *sharedInt = 3;
    sleep(1);

    //test print
    printf(""); printf("\n"); sleep(1);

    shmdt(sharedInt); shmdt(sharedStr);
    return 0;   
}