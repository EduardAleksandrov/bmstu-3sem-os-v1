#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

void check_status(pid_t childpid,int stat_val)
{
    if(WIFEXITED(stat_val))
        printf("Child with PID %d exited normally with exit code%d\n", childpid, WEXITSTATUS(stat_val));
    else if(WIFSIGNALED(stat_val))
        printf("ChildwithPID %d ended with a non−intercepted signalnumber %d\n",childpid, WTERMSIG(stat_val));
    else if(WIFSTOPPED(stat_val))
        printf("Child with PID %d was stopped by asignal%d\n", childpid, WSTOPSIG(stat_val));
    else
        printf("Child terminated abnormally\n");
}

int main()
{
    int status;
    pid_t childpid[2];

    for(int	i=0;i<2;i++)
    {
        if((childpid[i]=fork())==-1)
        {
            perror("Can’tfork\n");
            exit(1);
        }
        else if(childpid[i]==0)
        {
            printf("Child process: pid=%d,ppid=%d,pgrp=%d\n", getpid(), getppid(), getpgrp());
            return 0;
        }
        else
        {
            printf("Parent process: pid=%d,pgrp=%d,childpid=%d\n", getpid(), getpgrp(), childpid[i]);
        }
    }

    for(int i = 0; i < 2; i++)
    {
        if(waitpid(childpid[i], &status, 0) == -1)
        {
                perror("Wait error\n");
                exit(1);
        } else {
            printf("Child has finished: PID = %d\n" , childpid[ i ]);
            check_status(childpid[ i ], status);
        }
    }
    return 0;
}
