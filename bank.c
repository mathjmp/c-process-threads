#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/wait.h>
#include <semaphore.h>

#define STACK_SIZE 65536
#define NUMBER_OF_THREADS 2
#define SECONDS 1000000

sem_t mutex;

void up(sem_t *sem) {
    sem_wait(sem);
}

void down(sem_t *sem) {
    sem_post(sem);
}


typedef struct Account {
    char *name;
    double balance;
} Account;

typedef struct Transaction {
    Account *account;
    char *type;
    double amount;
} Transaction;

Account a1;

Account get_account() {
    return a1;
}

void write_account(Account account) {
    a1 = account;
}

static int deposit(void *arg) {

    Account account;
    up(&mutex);
    double amount = 200;
    account = get_account();
    printf("balance=%.2f\n", a1.balance);
    usleep(1000);
    a1.balance += amount;
    printf("deposit %.2f successfully, balance=%.2f\n", amount, a1.balance);
    down(&mutex);
}

static int withdraw(void *arg) {

    Account account;
    up(&mutex);
    double amount = 100;
    account = get_account();
    printf("balance=%.2f\n", a1.balance);

    usleep(1000);
    a1.balance -= amount;
    printf("refund %.2f successfully, balance=%.2f\n", amount, a1.balance);
    down(&mutex);
}

int main() {

    sem_init(&mutex, 1, 1);
    int pids[NUMBER_OF_THREADS];
    int flags = CLONE_VM | SIGCHLD;
    void *stack1, *stack2;
    a1.name = "Jhon Doe";
    a1.balance = 500;
    
    stack1 = malloc(STACK_SIZE);
    void *c_stack1 = stack1 + STACK_SIZE;
    pids[0] = clone(deposit, c_stack1, flags, NULL);
    
    stack2 = malloc(STACK_SIZE);
    void *c_stack2 = stack2 + STACK_SIZE;
    pids[1] = clone(withdraw, c_stack2, flags, NULL);
    
    waitpid(pids[0], 0, 0);
    waitpid(pids[1], 0, 0);
    
    usleep(SECONDS * 5);
    printf("account balance: %.2f\n", a1.balance);
    return 0;
}