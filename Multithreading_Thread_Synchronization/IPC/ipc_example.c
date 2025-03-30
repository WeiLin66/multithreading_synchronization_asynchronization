/**
 * This simple program demonstrates inter-thread communication using two threads.
 *
 * Steps:
 * 1. A callback function is registered to calculate the product of two numbers.
 * 2. Global variables (a, b) and mutexes are initialized.
 * 3. Thread A is created to send the variables to Thread B.
 * 4. Thread B is created to receive the variables and
 *    use the registered callback function to compute their product.
 * 5. Thread B returns the result to the main thread before terminating.
 *
 * The entire communication mechanism is based on TOC (Thread-Oriented Communication) using IPC.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <error.h>
#include <signal.h>
#include <pthread.h>

// Global variables
int a = 0;
int b = 0;
int result = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t thread_a, thread_b;

// declaration of the function pointer
typedef long (*callback_func)(long, long);
callback_func callback = NULL;

// Function to calculate the product of two numbers
long calculate_product(long a, long b) {
    return a * b;
}

// function declaration
static void* thread_a_func (void* arg);
static void* thread_b_func (void* arg);

// thread A function
static void* thread_a_func (void* arg) {
    (void)arg; // Explicitly cast arg to void to suppress unused parameter warning

    int numbers[2] = {a, b};
    
    printf("Thread A: Sending numbers to Thread B: %d, %d\n", numbers[0], numbers[1]);
    
    pthread_t thread_b;
    
    // Create thread B and pass the numbers to it, and then wait for the return value
    int* thread_b_result = NULL;
    if (callback == NULL) {
        fprintf(stderr, "Thread A: Callback function is not set.\n");
        return NULL;
    }
    if (pthread_create(&thread_b, NULL, thread_b_func, callback) != 0) {
        fprintf(stderr, "Thread A: Failed to create Thread B.\n");
        return NULL;
    }
    
    pthread_join(thread_b, (void*)&thread_b_result);
    if (thread_b_result) {
        printf("Thread A: Received result from Thread B: %d\n", *(int*)thread_b_result);
    }
    
    free(thread_b_result);

    return NULL;
}

// thread B function
static void* thread_b_func (void* arg) {
    if (!arg) {
        fprintf(stderr, "Thread B: callback function is not set.\n");
        return NULL;
    }
    callback_func temp_callback = (callback_func)arg;
    
    // Call the callback function to calculate the product
    long* result = (long*) calloc(0, sizeof(int));
    if (result == NULL) {
        fprintf(stderr, "Thread B: Failed to calculate the product.\n");
        return NULL;
    }
    printf("Thread B: Calculated product: %ld\n", *result);

    pthread_mutex_lock(&mutex);
    *result = temp_callback((long)a, (long)b);
    pthread_mutex_unlock(&mutex);
    return result;
}

int main () {
    // register the callback function
    callback = calculate_product;

    // initialize the global variables
    a = 5;
    b = 10;
    
    // create thread A
    if (pthread_create(&thread_a, NULL, thread_a_func, NULL) != 0) {
        fprintf(stderr, "Failed to create Thread A.\n");
        return EXIT_FAILURE;
    }

    // clean up
    pthread_mutex_destroy(&mutex);
    pthread_join(thread_a, NULL);

    return EXIT_SUCCESS;
}