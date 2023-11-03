// Запись и чтение в неименованный pipe
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    int pipefd[2]; // имя массива дескрипторов
    char buf;
    pid_t cpid[2];
    char *msg[15] = {"aaa\n", "cccccnnnnnnnnnnnn\n"}; // размер столбцов 15

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    for(int	i=0;i<2;i++)
    {
        cpid[i] = fork();
        if (cpid[i] == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (cpid[i] == 0) {
            printf("Child %d послал %s \n", getpid(), msg[i]);
            close(pipefd[0]);
            write(pipefd[1], msg[i], strlen(msg[i])); 
            close(pipefd[1]);
            _exit(EXIT_SUCCESS);

        } else {

        }
    }
        wait(NULL);
        close(pipefd[1]);

        while (read(pipefd[0], &buf, 1) > 0)
            write(STDOUT_FILENO, &buf, 1);

        // write(STDOUT_FILENO, "\n", 1);
        close(pipefd[0]);
}