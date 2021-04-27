int mutexValue, waitValue;

typedef struct CS {
    int val;
    sem_t gate; //semaphore
    sem_t mutex; //mutex
}countSem;

//The mutex in the pdf is considered binary sem, which should be treated as mutex
void semWait(struct CS *givenSem) {
    // printf("waitValue: %d val: %d mutexValue: %d\n", sem_getvalue(
    //     &givenSem->gate, &waitValue), givenSem->val, 
    //     sem_getvalue(&givenSem->mutex, &mutexValue));

    sem_wait(&givenSem->gate);
    sem_wait(&givenSem->mutex);
    givenSem->val -= 1;

    // printf("waitValue: %d val: %d mutexValue: %d\n", sem_getvalue(
    //     &givenSem->gate, &waitValue), givenSem->val, 
    //     sem_getvalue(&givenSem->mutex, &mutexValue));
    
    if (givenSem->val > 0) {
        sem_post(&givenSem->gate);
    }
    sem_post(&givenSem->mutex);
}

void semPost(struct CS *givenSem) {

    sem_wait(&givenSem->mutex);
    givenSem->val += 1;
    if (givenSem->val = 1) {
        sem_post(&givenSem->gate);
    }
    sem_post(&givenSem->mutex);
}

/* BSem wait() [otherwise "gate"] will be initialized here as 0 
Bsem mutex() [otherwise "mutex"] will be initialized here as 1
*/
void semInit(struct CS *givenSem, int k) {
    givenSem->val = k;
    if (givenSem->val >= 1) {
        sem_init(&givenSem->gate, 0, 1);
    } else {
        sem_init(&givenSem->gate, 0, 0); //not allowed to call this for counting sem
    }
    sem_init(&givenSem->mutex, 0, 1);
}