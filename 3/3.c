#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>

#define MEM_SIZE 4096
#define NUM_THREADS 4

char* shared_mem;
pthread_mutex_t lock;

void* threadA(void* arg) {
    char* local_mem = (char*) malloc(MEM_SIZE);
    sprintf(local_mem, "Hello from thread A");
    memcpy(shared_mem, local_mem, MEM_SIZE);
    free(local_mem);
    pthread_exit(NULL);
}

void* threadB(void* arg) {
    int fd = open("/dev/zero", O_RDWR);
    char* mapped_mem = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    close(fd);
    
    pthread_mutex_lock(&lock);
    
    switch(*((int*) arg)) {
        case 1: {
            printf("Thread B mapping memory for case A\n");
            memcpy(mapped_mem, shared_mem, MEM_SIZE);
            break;
        }
        case 2: {
            printf("Thread B mapping memory for case B\n");
            memcpy(mapped_mem, shared_mem, MEM_SIZE);
            mapped_mem[0] = '!';
            memcpy(shared_mem, mapped_mem, MEM_SIZE);
            break;
        }
        case 3: {
            printf("Thread B mapping memory for case C\n");
            char* tmp_mem = (char*) malloc(MEM_SIZE);
            memcpy(tmp_mem, shared_mem, MEM_SIZE);
            memcpy(shared_mem, mapped_mem, MEM_SIZE);
            memcpy(mapped_mem, tmp_mem, MEM_SIZE);
            free(tmp_mem);
            break;
        }
    }
    
    pthread_mutex_unlock(&lock);
    munmap(mapped_mem, MEM_SIZE);
    pthread_exit(NULL);
}

void* threadC(void* arg) {
    char* local_mem = (char*) malloc(MEM_SIZE);
    while(1) {
        pthread_mutex_lock(&lock);
        memcpy(local_mem, shared_mem, MEM_SIZE);
        pthread_mutex_unlock(&lock);
        printf("Thread C reading from shared memory: %s\n", local_mem);
        usleep(1000000);
    }
    free(local_mem);
    pthread_exit(NULL);
}

void* threadD(void* arg) {
    while(1) {
        pthread_mutex_lock(&lock);
        shared_mem[0] = '!';
        printf("Thread D writing to shared memory\n");
        pthread_mutex_unlock(&lock);
        usleep(1000000);
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];
    
    int fd = open("/dev/zero", O_RDWR);
    shared_mem = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    
    pthread_mutex_init(&lock, NULL);
    
    pthread_create(&threads[0], NULL, threadA, NULL);
    
    thread_args[0] = 1;
    pthread_create(&threads[1], NULL, threadB, &thread_args[0]);
    
    pthread_create(&threads[2], NULL, threadC, NULL);
    
    pthread_create(&threads[3], NULL, threadD, NULL);
    
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_mutex_destroy(&lock);
    munmap(shared_mem, MEM_SIZE);
    
    return 0;
}