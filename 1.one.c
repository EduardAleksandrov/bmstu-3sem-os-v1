#include <stdio.h>
// #include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    int childpid;
    if(((childpid = fork())) == -1) //программа раздваиваивается и та и другая выполняет этот код
    {
        perror("cannot fork");
        exit(1);
    } else if(childpid == 0) // дочерний процесс (child)
    {
        printf("Hello from child\n");
        execl("./build/1.oneexec", "-lh", "/home", NULL); //передает дочернему процессу исполняемый файл

    } else {// родительский процесс (parent)
        printf("Hello from parent\n");
    }
    printf("Hello from main file\n");
    // execl("./build/1.oneexec", "-lh", "/home", NULL);   // он назначается и дочернему и родительскому поэтому запускается два раза

    return 0;
}