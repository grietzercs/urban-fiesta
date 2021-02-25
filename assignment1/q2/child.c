// @author Colden Grietzer

#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    pid_t child1 = fork();
    
    if (child1 == 0) {
        sleep(1);
        printf("Child1 getpid(): %d\n", getpid());
        printf("Child1 PPID: %d\n\n", getppid());
        
        int shmStrID = shmget(7999, 1024, 0666 | IPC_CREAT);
        int shmIntID = shmget(9999, 1024, 0666 | IPC_CREAT);

        int *sharedInt = (int *) shmat(shmIntID, NULL, 0);
        char *sharedStr = (char *) shmat(shmStrID, NULL, 0);

        while(*sharedInt != 1) {
            sleep(1);
        }

        usleep(100);
        strcpy(sharedStr, "shared");
        usleep(100);

        *sharedInt = 2;
        usleep(100);

        //test print
        printf("Child1 is writing: %s\n", sharedStr); sleep(1);

        shmdt(sharedInt); shmdt(sharedStr);
        
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
