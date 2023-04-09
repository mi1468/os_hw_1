#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 4
#define LOOP_COUNT 100000

void *thread_func(void *arg) {
    int *data = (int *)arg;
    int i;
    for (i = 0; i < LOOP_COUNT; i++) {
        madvise(data, sizeof(int), MADV_HUGEPAGE);
    }
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    int num_threads = atoi(argv[1]);
    pthread_t threads[num_threads];
    int thread_args[num_threads];
    int i, rc;
    void *status;
    int *data = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, num_threads);

    // Create threads
    for (i = 0; i < num_threads; i++) {
        thread_args[i] = i;
        rc = pthread_create(&threads[i], NULL, thread_func, (void *)&data);
        if (rc) {
            printf("Error creating thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    // Wait for threads to complete
    for (i = 0; i < num_threads; i++) {
        rc = pthread_join(threads[i], &status);
        if (rc) {
            printf("Error joining thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    // Cleanup
    pthread_barrier_destroy(&barrier);
    munmap(data, sizeof(int));
    pthread_exit(NULL);

    return 0;
}
