#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#include <sys/shm.h>
#define SHM_SIZE 1024

int main()
{
    char buf = 'n';
    pid_t cpid[2], w;
    int wstatus;

// установка разделяемой памяти
    int shmid;
    int *data;
    if((shmid = shmget(100, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(1);
    }
    if ((data = shmat(shmid, NULL, 0)) == (void *)-1) {
        perror("shmat");
        exit(1);
    }
    // strcpy(data, 6);
    *data = 6;
//---
    for(int	i = 0; i < 2; i++)
    {
        cpid[i] = fork();
        if (cpid[i] == -1) 
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (cpid[i] == 0) 
        {
            // buf = 't';
            // printf("%c \n", buf);
            // if(i == 1) strcpy(data, 7);
            if(i == 1) *data = 7;
            printf("segment contains: \"%d\"\n", *data);
            
            exit(EXIT_SUCCESS);

        } else {
            printf("PP: childpid=%d\n", cpid[i]);
        }
    }
    for(int	i=0;i<2;i++)
    {
        w = waitpid(cpid[i], &wstatus, WUNTRACED | WCONTINUED);
        if (w == -1) 
        {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }

        if (WIFEXITED(wstatus)) 
        {
            printf("exited, status=%d\n", WEXITSTATUS(wstatus));
        } else if (WIFSIGNALED(wstatus)) 
        {
            printf("killed by signal %d\n", WTERMSIG(wstatus));
        } else if (WIFSTOPPED(wstatus)) 
        {
            printf("stopped by signal %d\n", WSTOPSIG(wstatus));
        } else if (WIFCONTINUED(wstatus)) 
        {
            printf("continued\n");
        }
    }

    // printf("%c", buf);

    // отсоединяемся и удаляем память
    if(shmdt((void*)data) == -1)
    {
        perror("shmdt");
    }
    
    if(shmctl(shmid, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
    }

}