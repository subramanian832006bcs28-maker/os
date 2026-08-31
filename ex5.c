[24bcs036@mepcolinux ex5]$cat common.h
// common.h
#ifndef COMMON_H
#define COMMON_H

// Add these two lines at the absolute top for POSIX compatibility in C99
#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define SHM_NAME "/shm_producer_consumer"
#define SEM_MUTEX "/sem_mutex"
#define SEM_EMPTY "/sem_empty"
#define SEM_FULL "/sem_full"

typedef struct {
    int buffer[BUFFER_SIZE];
    int in;
    int out;
} shared_data_t;

#endif

[24bcs036@mepcolinux ex5]$cat producer.c
// producer.c
#include "common.h"

int main() {
    // 1. Create or open shared memory
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(shared_data_t));
    shared_data_t *shared = (shared_data_t *)mmap(NULL, sizeof(shared_data_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // Initialize buffer pointers
    shared->in = 0;
    shared->out = 0;

    // 2. Initialize Semaphores
    sem_t *mutex = sem_open(SEM_MUTEX, O_CREAT, 0666, 1);
    sem_t *empty = sem_open(SEM_EMPTY, O_CREAT, 0666, BUFFER_SIZE);
    sem_t *full = sem_open(SEM_FULL, O_CREAT, 0666, 0);

    printf("Producer started. (Enter -1 to exit)\n\n");

    int input_item;
    while (1) {
        printf("Enter an integer to produce: ");
        if (scanf("%d", &input_item) != 1) {
            printf("Invalid input. Please enter numbers only.\n");
            // Clear input buffer
            while (getchar() != '\n');
            continue;
        }

        // Check for termination signal
        if (input_item == -1) {
            // Send exit signal (-1) to the consumer through the buffer
            sem_wait(empty);
            sem_wait(mutex);
            shared->buffer[shared->in] = -1;
            shared->in = (shared->in + 1) % BUFFER_SIZE;
            sem_post(mutex);
            sem_post(full);
            break;
        }

        printf("[Waiting] Checking if buffer has empty space...\n");
        sem_wait(empty); // Blocks here if the buffer is completely full
        sem_wait(mutex); // Enter critical section

        // Place user item into the buffer
        shared->buffer[shared->in] = input_item; 
        printf(">> [Success] Produced: %d at index %d\n\n", shared->buffer[shared->in], shared->in);
        shared->in = (shared->in + 1) % BUFFER_SIZE;

        sem_post(mutex); // Leave critical section
        sem_post(full);  // Notify consumer
    }

    // Clean up local handles
    munmap(shared, sizeof(shared_data_t));
    close(shm_fd);
    sem_close(mutex);
    sem_close(empty);
    sem_close(full);

    printf("Producer terminated successfully.\n");
    return 0;
}

[24bcs036@mepcolinux ex5]$cat consumer.c
// consumer.c
#include "common.h"

int main() {
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Shared memory opening failed. Run producer first");
        exit(1);
    }
    shared_data_t *shared = (shared_data_t *)mmap(NULL, sizeof(shared_data_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sem_t *mutex = sem_open(SEM_MUTEX, 0);
    sem_t *empty = sem_open(SEM_EMPTY, 0);
    sem_t *full = sem_open(SEM_FULL, 0);

    printf("Consumer started. Waiting for items...\n\n");

    while (1) {
        sem_wait(full);  // Blocks here if the buffer is completely empty
        sem_wait(mutex); // Enter critical section

        int item = shared->buffer[shared->out];
        
        // Break out of the loop if producer sent exit code
        if (item == -1) {
            sem_post(mutex);
            break;
        }

        printf("<< [Success] Consumed: %d from index %d\n", item, shared->out);
        shared->out = (shared->out + 1) % BUFFER_SIZE;

        sem_post(mutex); // Leave critical section
        sem_post(empty); // Notify producer that a slot freed up

        sleep(1); // Simulate consumption processing time
    }

    // Clean up and unlink shared system resources completely
    munmap(shared, sizeof(shared_data_t));
    close(shm_fd);
    sem_close(mutex);
    sem_close(empty);
    sem_close(full);

    shm_unlink(SHM_NAME);
    sem_unlink(SEM_MUTEX);
    sem_unlink(SEM_EMPTY);
    sem_unlink(SEM_FULL);

    printf("\nConsumer detected exit signal. Resources cleared.\n");
    return 0;
}

[24bcs036@mepcolinux ex5]$./producer
Producer started. (Enter -1 to exit)

Enter an integer to produce: 10
[Waiting] Checking if buffer has empty space...
>> [Success] Produced: 10 at index 0

Enter an integer to produce: 20
[Waiting] Checking if buffer has empty space...
>> [Success] Produced: 20 at index 1

Enter an integer to produce: 30
[Waiting] Checking if buffer has empty space...
>> [Success] Produced: 30 at index 2

Enter an integer to produce: 40
[Waiting] Checking if buffer has empty space...
>> [Success] Produced: 40 at index 3

Enter an integer to produce: -1
Producer terminated successfully.
[24bcs036@mepcolinux ex5]$./consumer
Consumer started. Waiting for items...

<< [Success] Consumed: 10 from index 0
<< [Success] Consumed: 20 from index 1
<< [Success] Consumed: 30 from index 2
<< [Success] Consumed: 40 from index 3

Consumer detected exit signal. Resources cleared.
[24bcs036@mepcolinux ex5]$exit
