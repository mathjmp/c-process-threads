#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int arg, char **argv, char *envp[])
{

    int pid = getpid();

    for (int index = 0; index < 3; index++)
    {

        int child_pid = fork();

        if (child_pid < 0) {
            perror("could not create subprocess\n");
            exit(1);
        }

        if (child_pid == 0) {

            int pid = getpid();
            printf("child process running with pid %d\n", pid);

            if (index == 0) {
                printf("-=-=-=-=-=-=-=-=-=-=-=-= ls -=-=-=-=-=-=---=-=-==-=\n");
                execve("/bin/ls", argv, envp);
            }

            if (index == 1) {
                printf("-=-=-=-=-=-=-=-=-=-=-=-= pwd -=-=-=-=-=-=---=-=-==-=\n");
                execve("/bin/pwd", argv, envp);
            }

            if (index == 2) {
                printf("-=-=-=-=-=-=-=-=-=-=-=-= id -=-=-=-=-=-=---=-=-==-=\n");
                execve("/bin/id", argv, envp);
            }
        } else {
            waitpid(child_pid, NULL, 0);
        }
    }

    printf("parent process running on pid %d\n", pid);
}