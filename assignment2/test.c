#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "correct.h"

int main(int argc, int **argv) {
    struct CS fs, es;
    semInit(&fs, 5); semInit(&es, 0);

    semWait(&es);
    semPost(&fs);

    semWait(&fs);
    semPost(&es);

    return 0;
}