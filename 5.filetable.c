// параллельное открытие файла
#include <stdio.h>
#include <stdlib.h> //exit
#include <unistd.h> //fork, exec
#include <fcntl.h>
#include <sys/wait.h> //wait


int main(void)
{
    int childpid;

    if(((childpid = fork())) == -1)
    {
        perror("cannot fork");
        exit(1);
    } else if(childpid == 0) // дочерний процесс (child)
    {
        int fd = open("./5.file.txt", O_WRONLY | O_APPEND);
        char x = '7';
        if(write(fd, &x, sizeof(char)) == -1) exit(1);
        // getchar();

    } else {// родительский процесс (parent)
        printf("Hello from parent\n");

        int fd = open("./5.file.txt", O_WRONLY | O_APPEND);
        char x = '5';
        if(write(fd, &x, sizeof(char)) == -1) exit(1);
        
        // wait(NULL);
        // int z;
        // if(read(fd, &z, sizeof(z)) == -1) exit(1);
        // printf("Received: %d \n", z);
        // close(fd);
    }
    return 0;
}