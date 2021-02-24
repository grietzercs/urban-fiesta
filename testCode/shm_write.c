#include <stdio.h>
#include <math.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdlib.h>

int main(void) {
    // printf("Hello World\n");

    // int five = 5;
    // int three = 3;
    // int sum = five + three;
    
    // printf("Addition: %d + %d = %d\n", five, three, sum);
    // printf(" This\n");

    //creates shared memory
    int shmid = shmget(7654, 1024, 0666 | IPC_CREAT);
    
    //attaches string to the shared memory
    int *sharedInt = (int *) shmat(shmid, NULL, 0);
    char *str = (char *) shmat(shmid, NULL, 0);

    int x = 5; *sharedInt = x;

    printf("This is int: %d\n", *sharedInt);

    while(sharedInt != 6) {
        
    }

    while(*str == '\0') {
        sleep(1);
    }


    printf("%s", &str);
    // while(*str != '\0') {
    //     printf("%c", *str++);
    // }

    printf("done\n");
    return 0;
}