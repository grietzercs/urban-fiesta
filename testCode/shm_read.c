#include <stdio.h>
#include <math.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    printf("This is reader code");

    int shmid = shmget(7654, NULL, 0666 | IPC_CREAT);

    int *sharedInt = (int *) shmat(shmid, NULL, 0);
    char *str = (char *) shmat(shmid, NULL, 0);

    char str2[4] = "test";
    for (int i = 0; i < strlen(str2); i++) {
        *str++ = str2[i];
    }

    *sharedInt = 6;

    printf("This is str: %d", *sharedInt);
}