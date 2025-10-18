#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/wait.h>

#define ARRAY_CAPACITY 5
#define STACK_SIZE 65536
#define WAIT_TIME 1000000

int counter = 0;

static int increment_counter(void *arg) {
    
    int id = *((int*) arg);
    printf("thread %d starting\n", id);

    for (int index = 0; index < ARRAY_CAPACITY; index++) {
        counter += 1;
        printf("thread %d increment counter to %d\n", id, counter);
        usleep(WAIT_TIME / (1 + id / 10.0));
    }

    printf("thread %d finishing...\n", id);
}

int main() {


    int tids[] = {1, 2, 3, 4, 5};
    int pids[ARRAY_CAPACITY];
    void *stack;

    for (int index = 0; index < ARRAY_CAPACITY; index++) {

        stack = malloc(STACK_SIZE);

        if (stack == 0) {
            perror("could not allocate memory");
            exit(1);
        }

        void *child_stack = stack + STACK_SIZE;
        int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD;
        pids[index] = clone(increment_counter, child_stack, flags, &(tids[index]));
    }


    printf("main thread waiting threads to execute\n");

    for (int index = 0; index < ARRAY_CAPACITY; index++) {

        int wait_result = waitpid(pids[index], 0, 0);

        if (wait_result == -1) {
            perror("waitpid error");
            exit(2);
        }
    }

    printf("main thread finishing...\n");
    return 0;
}