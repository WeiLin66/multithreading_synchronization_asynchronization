#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <pthread.h>
#include <signal.h>

static volatile int keep_running = 1;

static void sigint_handler (int sig) {
    keep_running = 0;
}

static void* pthread_func_cb (void* args) {

    char default_msg[32] = "No args are passed in.";
    char* msg = default_msg;
    pid_t pid = getpid();

    if (args) {
        msg = (char*) args;
    }

    while(keep_running) {
        printf("func : %s, PID : %d, message: %s\n", __func__, pid, msg);
        sleep(1);
    }
    return NULL;
}

int main (int argc, char* argv[]) {

    pthread_t phandle;
    int rc;
    char* msg_for_thread = "I'm a child thread!";

    signal(SIGINT, sigint_handler);

    if ((rc = pthread_create(&phandle, NULL, pthread_func_cb, msg_for_thread)) != 0) {
        printf("Error : pthread_create failed\n");
        exit(1);
    }

    printf("Pthread has been created...\n");
    pause();

    pthread_join(phandle, NULL);
    printf("Main thread exiting...\n");

    return 0;
}