// обработчик сигнала
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
    pid_t cpid[2];

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

            signal(SIGTERM, signal_handler); // kill потомка
            sleep(20);
            if(flag == 1)
            {
                close(pipefd[0]);
                
                char *msg;
                if(i==0) msg = "aaa\n";
                else msg = "bbbb\n";

                write(pipefd[1], msg, strlen(msg));
                close(pipefd[1]);
            }
            
            _exit(EXIT_SUCCESS);

        } else {

        }
    }
        wait(NULL);
        close(pipefd[1]);

        while (read(pipefd[0], &buf, 1) > 0)
            write(STDOUT_FILENO, &buf, 1);

        write(STDOUT_FILENO, "\n", 1);
        close(pipefd[0]);

        

}