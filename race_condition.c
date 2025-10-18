#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>

#define STACK_SIZE 1024 * 64
#define NUM_ITERATIONS 100000

sem_t mutex;
int balance = 1000;


void up() {
    sem_wait(&mutex);
}

void down() {
    sem_post(&mutex);
}

int deposit(void *arg) {
    up();
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        int temp = balance;
        temp += 1;
        balance = temp;
    }
    down();
    return 0;
}

int withdraw(void *arg) {

    up();
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        int temp = balance;
        temp -= 1;
        balance = temp;
    }
    down();
    return 0;
}

int main() {
    sem_init(&mutex, 1, 1);
    void* stack1 = malloc(STACK_SIZE);
    void* stack2 = malloc(STACK_SIZE);

    if (!stack1 || !stack2) {
        perror("Failed to allocate stack");
        exit(1);
    }

    printf("Initial Balance: %d\n", balance);

    pid_t pid1 = clone(deposit, stack1 + STACK_SIZE, CLONE_VM | SIGCHLD, NULL);
    pid_t pid2 = clone(withdraw, stack2 + STACK_SIZE, CLONE_VM | SIGCHLD, NULL);

    if (pid1 == -1 || pid2 == -1) {
        perror("clone");
        exit(1);
    }

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    printf("Final Balance (expected 1000): %d\n", balance);

    free(stack1);
    free(stack2);

    return 0;
}
