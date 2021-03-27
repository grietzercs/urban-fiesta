int mutexValue, waitValue;

struct CS {
    int val;
    sem_t B1; //semaphore
    sem_t B2; //mutex
};

//The mutex in the pdf is considered binary sem, which should be treated as mutex
void semWait(struct CS *givenSem) {
    // printf("waitValue: %d val: %d mutexValue: %d\n", sem_getvalue(
    //     &givenSem->B1, &waitValue), givenSem->val, 
    //     sem_getvalue(&givenSem->B2, &mutexValue));

    sem_wait(&givenSem->B1);
    sem_wait(&givenSem->B2);
    givenSem->val -= 1;

    // printf("waitValue: %d val: %d mutexValue: %d\n", sem_getvalue(
    //     &givenSem->B1, &waitValue), givenSem->val, 
    //     sem_getvalue(&givenSem->B2, &mutexValue));
    
    if (givenSem->val > 0) {
        sem_post(&givenSem->B1);
    }
    sem_post(&givenSem->B2);
}

void semPost(struct CS *givenSem) {

    sem_wait(&givenSem->B2);
    givenSem->val += 1;
    if (givenSem->val = 1) {
        sem_post(&givenSem->B1);
    }
    sem_post(&givenSem->B2);
}

/* BSem wait() [otherwise "B1"] will be initialized here as 0 
Bsem mutex() [otherwise "B2"] will be initialized here as 1
*/
void semInit(struct CS *givenSem, int k) {
    givenSem->val = k;
    if (givenSem->val >= 1) {
        sem_init(&givenSem->B1, 0, 1);
    } else {
        sem_init(&givenSem->B1, 0, 0); //not allowed to call this for counting sem
    }
    sem_init(&givenSem->B2, 0, 1);
}