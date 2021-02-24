#include <stdio.h>
#include <math.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    //creates shared memory
    int shmStrID = shmget(7999, 1024, 0666 | IPC_CREAT);
    int shmIntID = shmget(9999, 1024, 0666 | IPC_CREAT);
    
    //attaches string to the shared memory
    int *sharedInt = (int *) shmat(shmIntID, NULL, 0);
    char *sharedStr = (char *) shmat(shmStrID, NULL, 0);

    char *shared = "shared";
    char *memory = "memory";

    *sharedInt = 1;
    char *compareStr;
    compareStr = "shared";

    while(*sharedInt != 2) {
        sleep(1);
    }

    sleep(1);
    printf("Process B wrote in the following value: ");
    while(*sharedStr != '\0') {
        printf("%c", *sharedStr++);
    } printf("\n");

    printf("%d", strcmp(sharedStr, shared));

    sleep(3);
    while(*sharedStr != '\0') {
        printf("%c", *sharedStr++);
    } printf("\n"); sleep(3);

    printf("Process C wrote in the following value: ");
    while(*sharedStr != '\0') {
        printf("%c", sharedStr++);
    } printf("\n");
    
    printf("Value of sharedInt: %d", *sharedInt);

    printf("GoodBye");
    shmdt(sharedInt); shmdt(sharedStr);
    shmctl(shmIntID, IPC_RMID, NULL); shmctl(shmStrID, IPC_RMID, NULL);
    return 0;
}