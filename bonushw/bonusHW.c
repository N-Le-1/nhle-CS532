/*
Name: Noah Le
BlazerId: nhle
Project #: Homework 4
To compile: make
To run: ./bonusHW > output.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>

#define NUM_PRODUCERS 10
#define NUM_CONSUMERS 20
#define PRODUCER_ITEMS 500
#define CONSUMER_ITEMS 250
#define MAX_NUM 1000

// Shared Pipe File Descriptors
int pipe_fd[2];

// Mutexes for synchronization
pthread_mutex_t write_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t read_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

// Global flag for the graduate signal requirement
volatile sig_atomic_t start_reading = 0;

// Signal handler for child process
void signal_handler(int sig) {
    if (sig == SIGUSR1) {
        start_reading = 1;
    }
}

// Producer Thread Function
void* producer(void* arg) {
    int tid = *(int*)arg;
    free(arg); // Free dynamically allocated thread ID

    // Create an array of 0 to MAX_NUM to guarantee unique numbers
    int numbers[MAX_NUM + 1];
    for (int i = 0; i <= MAX_NUM; i++) {
        numbers[i] = i;
    }

    // Shuffle the array (Fisher-Yates) using a thread-safe seed
    unsigned int seed = time(NULL) ^ pthread_self();
    for (int i = MAX_NUM; i > 0; i--) {
        int j = rand_r(&seed) % (i + 1);
        int temp = numbers[i];
        numbers[i] = numbers[j];
        numbers[j] = temp;
    }

    // Write the first 500 unique numbers to the pipe
    for (int i = 0; i < PRODUCER_ITEMS; i++) {
        pthread_mutex_lock(&write_mutex);
        write(pipe_fd[1], &numbers[i], sizeof(int));
        pthread_mutex_unlock(&write_mutex);

        // Bonus: Progress indicator (printed to stderr to avoid corrupting output redirection)
        if ((i + 1) % 100 == 0) {
            pthread_mutex_lock(&print_mutex);
            fprintf(stderr, "[Producer %d] Progress: %d/%d written\n", tid, i + 1, PRODUCER_ITEMS);
            pthread_mutex_unlock(&print_mutex);
        }
    }

    pthread_mutex_lock(&print_mutex);
    fprintf(stderr, "[Producer %d] Finished generating and writing numbers.\n", tid);
    pthread_mutex_unlock(&print_mutex);

    return NULL;
}

// Consumer Thread Function
void* consumer(void* arg) {
    int tid = *(int*)arg;
    free(arg);

    long long thread_sum = 0;
    int current_number;

    for (int i = 0; i < CONSUMER_ITEMS; i++) {
        pthread_mutex_lock(&read_mutex);
        ssize_t bytes_read = read(pipe_fd[0], &current_number, sizeof(int));
        pthread_mutex_unlock(&read_mutex);

        if (bytes_read == sizeof(int)) {
            thread_sum += current_number;
        }
    }

    pthread_mutex_lock(&print_mutex);
    fprintf(stderr, "[Consumer %d] Finished. Sum = %lld\n", tid, thread_sum);
    pthread_mutex_unlock(&print_mutex);

    // Return the sum to the main child thread
    long long* ret = malloc(sizeof(long long));
    *ret = thread_sum;
    return (void*)ret;
}

int main() {
    // Initialize the pipe
    if (pipe(pipe_fd) == -1) {
        perror("Pipe failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } 
    else if (pid == 0) {
        // ---------------------------------------------------------
        // CHILD PROCESS: Consumers
        // ---------------------------------------------------------
        close(pipe_fd[1]); // Close unused write end

        // Set up signal handler for the CS 532 requirement
        struct sigaction sa;
        sa.sa_handler = signal_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction(SIGUSR1, &sa, NULL);

        fprintf(stderr, "[Child] Waiting for signal from parent to begin reading...\n");

        // Block until the parent sends SIGUSR1
        while (!start_reading) {
            pause(); 
        }

        fprintf(stderr, "[Child] Signal received! Spawning consumers...\n");

        pthread_t consumers[NUM_CONSUMERS];
        
        for (int i = 0; i < NUM_CONSUMERS; i++) {
            int* tid = malloc(sizeof(int));
            *tid = i + 1;
            pthread_create(&consumers[i], NULL, consumer, tid);
        }

        long long total_sum = 0;
        for (int i = 0; i < NUM_CONSUMERS; i++) {
            void* ret;
            pthread_join(consumers[i], &ret);
            total_sum += *(long long*)ret;
            free(ret);
        }

        double average = (double)total_sum / NUM_CONSUMERS;

        // Print final result to standard output (stdout) for text file redirection
        printf("Final Average of all %d consumer thread sums: %.2f\n", NUM_CONSUMERS, average);

        close(pipe_fd[0]);
        exit(EXIT_SUCCESS);
    } 
    else {
        // ---------------------------------------------------------
        // PARENT PROCESS: Producers
        // ---------------------------------------------------------
        close(pipe_fd[0]); // Close unused read end

        pthread_t producers[NUM_PRODUCERS];

        for (int i = 0; i < NUM_PRODUCERS; i++) {
            int* tid = malloc(sizeof(int));
            *tid = i + 1;
            pthread_create(&producers[i], NULL, producer, tid);
        }

        // Wait for all producer threads to complete
        for (int i = 0; i < NUM_PRODUCERS; i++) {
            pthread_join(producers[i], NULL);
        }

        // Close write end to safely terminate the pipe
        close(pipe_fd[1]);

        fprintf(stderr, "[Parent] All producers finished. Sending signal to child...\n");
        
        // Send signal to child process (CS 532 Requirement)
        kill(pid, SIGUSR1);

        // Wait for child process to finish
        wait(NULL);
        fprintf(stderr, "[Parent] Child process terminated. Exiting.\n");
    }

    return 0;
}