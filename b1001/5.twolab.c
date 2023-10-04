#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#define LENMAX 64

short flag = 0;

void signal_handler(int sig_num)
{
	flag = 1;
}

void check_status(pid_t childpid,int stat_val)
{
    if(WIFEXITED(stat_val))
        printf("Child with PID %d exited normally with exit code %d\n", childpid, WEXITSTATUS(stat_val));
    else if(WIFSIGNALED(stat_val))
        printf("Child with PID %d ended with a non−intercepted signal number%d\n", childpid, WTERMSIG(stat_val));
    else if(WIFSTOPPED(stat_val))
        printf("Child with PID %d was stopped by a signal%d\n", childpid, WSTOPSIG(stat_val));
    else
        printf("Child terminated abnormally\n");
}

int main()
{
    int status;
    char messages[2][LENMAX]={"helloworld\n", "AbCdEfGhIjKlMnOpQrStUvWxYz\n"};

    pid_t childpid[2];
    int fd[2];

    signal(SIGINT,signal_handler);
    sleep(10);

    if(pipe(fd)==-1)
    {
        perror("Can’tpipe\n");
        exit(1);
    }

    for(int	i=0;i<2;i++)
    {
        if((childpid[i]=fork())==-1)
        {
            perror("Can’tfork\n");
            exit(1);
        }
        else if(childpid[i]==0)
        {
            printf("Child process:pid=%d, ppid=%d, pgrp=%d\n", getpid(), getppid(), getpgrp());
            if(flag)
            {
                close(fd[0]);
                write(fd[1], messages[i], strlen(messages[i]));
            }
            return 0;
        }
        else
        {
            printf("Parent process:pid=%d, pgrp=%d, childpid=%d\n", getpid(), getpgrp(), childpid[i]);
        }
    }

    for(int	i=0;i<2;i++)
    {
        if(waitpid(childpid[i],&status,0)==-1)
        {
            perror("Wait error\n");
            exit(1);
        }
        else
        {
            printf("Child has finished: PID=%d\n",childpid[i]);
            check_status(childpid[i],status);
        }
    }

    close(fd[1]);
    printf("Received messages:\n");
    for(int	i=0;i<2;i++)
    {
        strcpy(messages[i], "###################\n");
        read(fd[0], messages[i], strlen(messages[i]));
        printf("%s", messages[i]);
    }

return 0;
}