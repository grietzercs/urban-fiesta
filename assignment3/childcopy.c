// @author Colden Grietzer

#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "correct.h"

#define SIZE 5

/*
    Shared memory semaphores
*/

int genRand() {
    int upper = 25; int lower = 0;
    int randNum = (rand() % (upper - lower + 1)) + lower;
}

int main() {
    countSem *fs, *es;
    
    pid_t child1 = fork();
    
    if (child1 == 0) {
        key_t fullsemKey = ftok("childcopy.c", 65);
        int semvalue;

        sleep(1);
        printf("Child1 getpid(): %d\n", getpid());
        printf("Child1 PPID: %d\n\n", getppid());
        
        int shmStrID = shmget(7999, 1024, 0666 | IPC_CREAT);
        int shmIntID = shmget(9999, 1024, 0666 | IPC_CREAT);

        int testID = shmget(8999, sizeof(fs), 066 | IPC_CREAT);
        fs = (countSem*)shmat(testID, NULL, 0);

        semInit(fs, SIZE);
        sem_getvalue(&fs->B2, &semvalue);
        printf("Sem value: %d\n", semvalue);

        int *sharedInt = (int *) shmat(shmIntID, NULL, 0);
        char *sharedStr = (char *) shmat(shmStrID, NULL, 0);

        // printf("Has not reached error");
        // sem_getvalue(&sem, semvalue);
        // printf("sem value before wait: %d\n", &semvalue);

        // while(*sharedInt != 1) { //previous code
        //     sleep(1);
        // }

        usleep(100);
        strcpy(sharedStr, "shared");
        usleep(100);

        *sharedInt = 2;
        usleep(100);
        printf("Child 1 value of sharedInt: %d\n", *sharedInt);

        printf("Child1 is writing: %s\n", sharedStr); sleep(1);

        shmdt(sharedInt); shmdt(sharedStr); shmdt(fs);

        printf("Child 1 closed shared memory segments and unlinked semaphore\n");
        
        exit(0);
    }
    else {
        pid_t child2 = fork();
        if (child2 == 0) {
            sleep(1);
            printf("Child2 getpid(): %d\n", getpid());
            printf("Child2 PPID: %d\n\n", getppid());

            int shmStrID = shmget(7999, 1024, 0666 | IPC_CREAT);
            int shmIntID = shmget(9999, 1024, 0666 | IPC_CREAT);

            int *sharedInt = (int *) shmat(shmIntID, NULL, 0);
            char *sharedStr = (char *) shmat(shmStrID, NULL, 0);

            while(*sharedInt != 2) {
                sleep(1);
            }

            usleep(100);
            strcpy(sharedStr, "memory");
            usleep(100);

            *sharedInt = 3;
            usleep(100);

            //test print
            printf("Child2 is writing: %s\n", sharedStr);sleep(1);

            shmdt(sharedInt); shmdt(sharedStr);
            exit(0);
        }
        
        printf("Parent PID: %d\n\n", getpid());

        int shmStrID = shmget(7999, 1024, 0666 | IPC_CREAT);
        int shmIntID = shmget(9999, 1024, 0666 | IPC_CREAT);

        // if (sharedSem == SEM_FAILED) {
        //     printf("Failed to create semaphore");
        // }

        //attaches string to the shared memory
        int *sharedInt = (int *) shmat(shmIntID, NULL, 0);
        char *sharedStr = (char *) shmat(shmStrID, NULL, 0);

        *sharedInt = 1;
        usleep(100);

        while(*sharedInt != 2) {
            sleep(1);
        }

        printf("Parent: Child 1 wrote: %s\n\n", sharedStr);
        usleep(100);

        while(*sharedInt != 3) {
            sleep(1);
        }

        printf("Parent: Child 2 wrote: %s\n\n", sharedStr);
        
        printf("Parent: Value of sharedInt: %d\n", *sharedInt);

        printf("Parent: GoodBye\n");
        shmdt(sharedInt); shmdt(sharedStr);
        shmctl(shmIntID, IPC_RMID, NULL); shmctl(shmStrID, IPC_RMID, NULL);

        wait(NULL);
    }

    return 0;
}
