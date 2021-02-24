#include <stdio.h>
#include <math.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    printf("Hello World\n");

    int five = 5;
    int three = 3;
    int sum = five + three;
    
    printf("Addition: %d + %d = %d\n", five, three, sum);
    printf(" This\n");

    //creates shared memory
    int shmid = shmget(5999, 1024, 0666 | IPC_CREAT);
    int testid = shmget(8999, 1024, 0666 | IPC_CREAT);
    
    //attaches string to the shared memory
    char *str = (char *) shmat(shmid, NULL, 0);
    char *sharedStr = (char *) shmat(testid, NULL, 0);
    
    char c = 'x'; *str = c;
    putchar(*str);
    printf("\n");

    char *str2;
    str2 = "shared";
    printf("This is str2: ");
    while(*str2 != '\0') {
        printf("%c", *str2++);
    } printf("\n");

    char *str4;
    str4 = "shared";
    printf("%d\n", strcmp(str2, str2));
    printf("\n");
    while(*sharedStr != '\0') {
        printf("%c", *sharedStr++);
    } printf("\n");
    return 0;
}