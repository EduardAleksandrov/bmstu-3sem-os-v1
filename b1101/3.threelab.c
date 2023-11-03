// обработчик сигнала SIGINT, строки задаем в child
#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

short flag = 0;

void signal_handler(int sig_num)
{
	flag = 1;
}

int main()
{
    int pipefd[2];
    char buf;
    pid_t cpid[2], w;
    int wstatus;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    if(signal(SIGINT, signal_handler)==SIG_ERR) // проверка на ошибку сигнала
    {
        perror("signal");
        exit(1);
    } // kill потомка вызов -(kill -INT pid)
     sleep(3);
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
            if(flag == 1)
            {
                close(pipefd[0]);
                
                char *msg;
                if(i==0) msg = "aaa\n"; // нет наследования
                else msg = "bbbb\n";

                write(pipefd[1], msg, strlen(msg));
                close(pipefd[1]);
            }
            
            exit(EXIT_SUCCESS);

        } else {
            printf("PP: childpid=%d\n", cpid[i]);
        }
    }
      //  wait(NULL);
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

        close(pipefd[1]);

        while (read(pipefd[0], &buf, 1) > 0)
            write(STDOUT_FILENO, &buf, 1);

        write(STDOUT_FILENO, "\n", 1);
        close(pipefd[0]);

        return 0;
}