#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>

#define PAGE_SIZE 4096
#define MEM_SIZE 4*PAGE_SIZE

void* mem;
pthread_t threadA, threadB, threadC, threadD;

void* threadA_func(void* arg) {
    int ret = posix_memalign(&mem, PAGE_SIZE, MEM_SIZE);
    if (ret != 0) {
        perror("posix_memalign failed");
        exit(EXIT_FAILURE);
    }
    printf("Thread A: memory allocated\n");
    pthread_exit(NULL);
}

void* threadB_func(void* arg) {
    int result = mprotect(mem, PAGE_SIZE, PROT_READ);
    if (result == -1) {
        perror("mprotect failed");
        exit(EXIT_FAILURE);
    }
    printf("Thread B: memory protected from reading\n");
    pthread_exit(NULL);
}

void* threadC_func(void* arg) {
    int i;
    char* p = (char*)mem;
    for (i = 0; i < PAGE_SIZE; i++) {
        printf("Thread C: read from memory: %d\n", p[i]);
    }
    pthread_exit(NULL);
}

void* threadD_func(void* arg) {
    int i;
    char* p = (char*)mem;
    for (i = 0; i < PAGE_SIZE; i++) {
        p[i] = i % 256;
    }
    printf("Thread D: wrote to memory\n");
    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    int option;
    
    scanf("%d", &option);
    switch (option) {
        case 1:
            pthread_create(&threadA, NULL, threadA_func, NULL);
            pthread_join(threadA, NULL);
            pthread_create(&threadC, NULL, threadC_func, NULL);
            pthread_create(&threadD, NULL, threadC_func, NULL);
            pthread_create(&threadB, NULL, threadB_func, NULL);
            pthread_join(threadB, NULL);
            pthread_join(threadC, NULL);
            pthread_join(threadD, NULL);
            


            break;
        case 2:
            pthread_create(&threadA, NULL, threadA_func, NULL);
            pthread_join(threadA, NULL);
            pthread_create(&threadC, NULL, threadC_func, NULL);
            pthread_create(&threadD, NULL, threadD_func, NULL);
            pthread_create(&threadB, NULL, threadB_func, NULL);
            pthread_join(threadB, NULL);
            pthread_join(threadC, NULL);
            pthread_join(threadD, NULL);
            

            break;
      
        case 3:
            pthread_create(&threadA, NULL, threadA_func, NULL);
            pthread_join(threadA, NULL);
            pthread_create(&threadC, NULL, threadD_func, NULL);
            pthread_create(&threadD, NULL, threadD_func, NULL);
            pthread_create(&threadB, NULL, threadB_func, NULL);
            pthread_join(threadB, NULL);
            pthread_join(threadC, NULL);
            pthread_join(threadD, NULL);
            

        break;
    default:
        printf("Invalid option\n");
        exit(EXIT_FAILURE);
}
printf("Program finished successfully\n");
exit(EXIT_SUCCESS);
}
