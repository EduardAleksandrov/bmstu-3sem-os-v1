#define _GNU_SOURCE // для работы WCOREDUMP, WIFCONTINUED

#include <stdio.h>
#include <sys/types.h> //getpid(), getppid()
#include <stdlib.h> //exit
#include <unistd.h> //fork, exec
#include <sys/wait.h> //wait


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
        execl("./build/1.oneexec", "ls", "-l", NULL); //передает дочернему процессу исполняемый файл

    } else {// родительский процесс (parent)
        printf("Hello from parent\n");
    }

// --- ожидание окончания процесса
    int status, pid;
    // while(wait(NULL)>0) // ждет пока все дочерние процессы завершатся
    while((pid = waitpid(childpid, &status, WUNTRACED)) > 0) //Возвращает идентификатор дочернего процесса, -1 ошибка
    { 
        if(WIFEXITED(status))
        {
            printf( "Процесс %d умер с кодом %d\n", pid, WEXITSTATUS(status));
        } else if(WIFSIGNALED(status))
        {
            printf( "Процесс %d убит сигналом %d\n", pid, WTERMSIG(status));
            if(WCOREDUMP(status)) printf( "Образовался core\n" );
            /* core - образ памяти процесса для отладчика adb */
        } else if(WIFSTOPPED(status))
        {
            printf( "Процесс %d остановлен сигналом %d\n", pid, WSTOPSIG(status));
        } else if(WIFCONTINUED(status))
        {
            printf( "Процесс %d продолжен\n", pid);
        }
    }
// --- ожидание окончания процесса конец

    printf("Hello from main file\n");
    // execl("./build/1.oneexec", "-lh", "/home", NULL);   // он назначается и дочернему и родительскому поэтому запускается два раза

// --- запуск файла по пути
    char *binaryPath = "/bin/ls";
    char *arg1 = "ls";
    char *arg2 = "-l";

    execl(binaryPath, arg1, arg2, NULL);
// --- запуск файла по пути

    return 0;
}