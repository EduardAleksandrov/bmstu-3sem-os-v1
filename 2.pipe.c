// взаимодействие между процессами через потоки (так же есть еще через разделяемую память и сокеты) - работает
#define _GNU_SOURCE // для работы WCOREDUMP, WIFCONTINUED

#include <stdio.h>
#include <sys/types.h> //getpid(), getppid()
#include <stdlib.h> //exit
#include <unistd.h> //fork, exec
#include <sys/wait.h> //wait
// #include <fcntl.h> //O_WRONLY


int main(void)
{
    int p[2];
    if(pipe(p) == -1)
    {
        perror("cannot p1 pipe");
        exit(1);
    } 

    int childpid;
    if(((childpid = fork())) == -1)
    {
        perror("cannot fork");
        exit(1);
    } else if(childpid == 0) // дочерний процесс (child)
    {
        printf("Hello from child\n");
        close(p[1]);
        int x;
        if(read(p[0], &x, sizeof(x)) == -1) exit(1);
        printf("Received: %d \n", x);
        close(p[0]);
    } else {// родительский процесс (parent)
        printf("Hello from parent\n");

        close(p[0]);
        int x = 5;
        if(write(p[1], &x, sizeof(x)) == -1) exit(1);
        close(p[1]);
        wait(NULL);
        
    }


    return 0;
}