#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

/*
This program provides a possible solution for first readers writers problem using mutex and semaphore.
I have used 10 readers and 5 producers to demonstrate the solution. You can always play with these values.
*/

sem_t wrt;
pthread_mutex_t mutex;
int cnt = 1;
int numreader = 0;
int sleepCount = 0;

void *writer(void *wno)
{   
    int value;
    sem_getvalue(&wrt, &value);
    printf("Before sem value: %d\n", value);
    sem_wait(&wrt);
    cnt = cnt*2;
    printf("Writer %d modified cnt to %d\n",(*((int *)wno)),cnt);
    sem_post(&wrt);
    sem_getvalue(&wrt, &value);
    printf("After sem value: %d\n", value);

}
void *reader(void *rno)
{   
    int value;
    usleep(200);
    // Reader acquire the lock before modifying numreader
    pthread_mutex_lock(&mutex);
    numreader++;
    if(numreader == 1) {
        sem_getvalue(&wrt, &value);
        printf("Before sem value: %d\n", value);
        sem_wait(&wrt); // If this id the first reader, then it will block the writer
    }
    pthread_mutex_unlock(&mutex);
    // Reading Section
    sem_getvalue(&wrt, &value);
    printf("Reader %d: read cnt as %d sem value: %d\n",*((int *)rno),cnt,value);

    // Reader acquire the lock before modifying numreader
    pthread_mutex_lock(&mutex);
    numreader--;
    if(numreader == 0) {
        sem_post(&wrt); // If this is the last reader, it will wake up the writer.
        sem_getvalue(&wrt, &value);
        printf("After sem value: %d\n", value);
    }
    pthread_mutex_unlock(&mutex);
}

int main()
{   

    pthread_t read[10],write[5];
    pthread_mutex_init(&mutex, NULL);
    sem_init(&wrt,0,1);
    int value; sem_getvalue(&wrt, &value);
    printf("Main sem value: %d\n\n", value);

    int a[10] = {1,2,3,4,5,6,7,8,9,10}; //Just used for numbering the producer and consumer

    for(int i = 0; i < 10; i++) {
        pthread_create(&read[i], NULL, (void *)reader, (void *)&a[i]);
    }
    for(int i = 0; i < 5; i++) {
        pthread_create(&write[i], NULL, (void *)writer, (void *)&a[i]);
    }

    // for(int i = 0; i < 10; i++) {
    //     pthread_join(read[i], NULL);
    //     if (i < 5) {
    //         pthread_join(write[i], NULL);
    //     }
    // }
    for(int i = 0; i < 5; i++) {
        pthread_join(write[i], NULL);
        pthread_join(read[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    sem_destroy(&wrt);

    return 0;
    
}