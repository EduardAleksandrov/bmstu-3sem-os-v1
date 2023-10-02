// взаимодейсndbtвие между процессами через файл pipe
#define _GNU_SOURCE // для работы WCOREDUMP, WIFCONTINUED

#include <stdio.h>
#include <sys/types.h> //getpid(), getppid()
#include <stdlib.h> //exit
#include <unistd.h> //fork, exec
#include <sys/wait.h> //wait
#include <fcntl.h> //O_WRONLY


int main(void)
{
    // int p[2];
    // if(pipe(p) == -1)
    // {
    //     perror("cannot p1 pipe");
    //     exit(1);
    // } 

    int childpid;
    if(((childpid = fork())) == -1)
    {
        perror("cannot fork");
        exit(1);
    } else if(childpid == 0) // дочерний процесс (child)
    {
        execl("./build/3.pipeexec", "ls", "-l", NULL); //передает дочернему процессу исполняемый файл
    } else {// родительский процесс (parent)
        printf("Hello from parent\n");

        int fd = open("./b0901/mpipe", O_WRONLY);
        int x = 5;
        if(write(fd, &x, sizeof(x)) == -1) exit(1);
        close(fd);
        wait(NULL);
        
    }


    return 0;
}