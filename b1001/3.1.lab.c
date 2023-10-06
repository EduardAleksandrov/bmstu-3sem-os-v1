// запускаем два дочерних файла
#define _GNU_SOURCE 
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>


int main()
{

    pid_t childpid[2], w;
    int wstatus;

    // static char *newargv[] = { NULL, "hello", "world", NULL };
    // static char *newenviron[] = { NULL };
    
    char *links[2] = {"./build/3.fileone", "./build/3.filetwo"};


    for(int	i=0;i<2;i++)
    {
        if((childpid[i]=fork())==-1)
        {
            perror("Can’t fork\n");
            exit(1);
        } else if(childpid[i]==0){
            // pause();
            // printf("Child %d process:pid=%d, ppid=%d, pgrp=%d\n", i, getpid(), getppid(), getpgrp());
            // sleep(2);
            execl(links[i], links[i], NULL);
            
            
            // printf("Child process(orphan):pid=%d, ppid=%d, pgrp=%d\n", getpid(), getppid(), getpgrp());
            exit(0);
        } else {
            // sleep(1);
            printf("Parent process:pid=%d, pgrp=%d, childpid=%d\n", getpid(), getpgrp(), childpid[i]);
        }
    }

    
    for(int	i=0;i<2;i++)
    {
        w = waitpid(childpid[i], &wstatus, WUNTRACED | WCONTINUED);
        if (w == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }

        if (WIFEXITED(wstatus)) {
            printf("exited, status=%d\n", WEXITSTATUS(wstatus));
        } else if (WIFSIGNALED(wstatus)) {
            printf("killed by signal %d\n", WTERMSIG(wstatus));
        } else if (WIFSTOPPED(wstatus)) {
            printf("stopped by signal %d\n", WSTOPSIG(wstatus));
        } else if (WIFCONTINUED(wstatus)) {
            printf("continued\n");
        }
    }
    
    return 0;
}