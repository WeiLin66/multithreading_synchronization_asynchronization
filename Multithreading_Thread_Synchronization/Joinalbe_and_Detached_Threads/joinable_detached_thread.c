#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_THREAD_POOL_NUM    5

void* thread_func(void* arg) {
    int index = *((int*)arg);
    pid_t pid = getpid();
    printf("Detached Thread %d: (PID) %d\n", index + 1, pid);
    free(arg);
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    int rc = -1;
    pthread_t thread_pool[MAX_THREAD_POOL_NUM];
    pthread_attr_t attr = {0};

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    for (int i = 0; i < MAX_THREAD_POOL_NUM; ++i) {
        printf("around %d\n", i);
        int* index = malloc(sizeof(int));
        if (index == NULL) {
            perror("Failed to allocate memory");
            exit(EXIT_FAILURE);
        }
        *index = i;
        if ((rc = pthread_create(&thread_pool[i], &attr, thread_func, (void*)index))) {
            printf("Error: unable to create thread, return value: %d\n", rc);
            free(index);
        }
    }
    pthread_attr_destroy(&attr);

    sleep(1);

    printf("Main: all threads have completed\n");

    return 0;
}
