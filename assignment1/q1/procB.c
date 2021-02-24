#include <stdio.h>
#include <math.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdlib.h>

int main(void) {
    printf("This is reader code");

    int shmStrID = shmget(7999, 1024, 0666 | IPC_CREAT);
    int shmIntID = shmget(9999, 1024, 0666 | IPC_CREAT);

    int *sharedInt = (int *) shmat(shmIntID, NULL, 0);
    char *sharedStr = (char *) shmat(shmStrID, NULL, 0);

    while(*sharedInt != 1) {
        sleep(1);
    }

    sleep(1);
    sprintf(sharedStr, "shared");
    sleep(1);

    *sharedInt = 2;
    sleep(1);

    shmdt(sharedInt); shmdt(sharedStr);
    return 0;
}