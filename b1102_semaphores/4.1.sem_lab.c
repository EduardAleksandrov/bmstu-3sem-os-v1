/*
    TASK Лаба 4: Семафор и разделяемая память 
    только читаем, не пишем - работает
*/
#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h> // S_IRWXU
#define SHM_SIZE 1024

// se = 0, sf = 1, sb = 2;
struct sembuf prod_start[2] = {{0,-1, 0}, {2,-1, 0}};
struct sembuf prod_end[2] = {{2, 1, 1}, {1, 1, 1}};
struct sembuf cons_start[2] = {{1,-1, 0}, {2,-1, 0}};
struct sembuf cons_end[2] = {{2, 1, 1}, {0, 1, 1}};

int producer(int fd, char *data);
int consumer(int fd, char *data);

int main()
{
    pid_t cpid[6], w;
    int wstatus;

// установка разделяемой памяти
    char * s1 = "abcdefghijklmnopqrstuvwxyz";
    char * s2 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    int shmid[3];
    char *data[30];
    for(int i = 0; i < 3; i++)
    {
        if((shmid[i] = shmget(i+100, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
            perror("shmget");
            exit(1);
        }
        if ((data[i] = shmat(shmid[i], NULL, 0)) == (void *)-1) {
            perror("shmat");
            exit(1);
        }
    }
    strcpy(data[0], s1);
    strcpy(data[1], s2);
//---
// установка семафора
    int perms = S_IRWXU | S_IRWXG | S_IRWXO;
    int fd = semget(100, 2, IPC_CREAT | perms);
    if(fd==-1)
    {
        perror("semop");
        exit(EXIT_FAILURE);
    }
//---

// запускаем процессы
    for(int	i = 0; i < 6; i++)
    {
        cpid[i] = fork();
        if (cpid[i] == -1) 
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (cpid[i] == 0) 
        {
            if(i%2 == 0)
            {
                producer(fd, data[0]);
            } else {
                consumer(fd, data[1]);
            }
            exit(EXIT_SUCCESS);
        } else {
            // printf("PP: childpid=%d\n", cpid[i]);
        }
    }
//---

//дожидаемся
    for(int	i=0;i<6;i++)
    {
        w = waitpid(cpid[i], &wstatus, WUNTRACED | WCONTINUED);
        if (w == -1) 
        {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
        if(WIFEXITED(wstatus)) 
        {
            printf("exited, status=%d\n", WEXITSTATUS(wstatus));
        } else if(WIFSIGNALED(wstatus)) 
        {
            printf("killed by signal %d\n", WTERMSIG(wstatus));
        } else if(WIFSTOPPED(wstatus)) 
        {
            printf("stopped by signal %d\n", WSTOPSIG(wstatus));
        } else if(WIFCONTINUED(wstatus)) 
        {
            printf("continued\n");
        }
    }
//---

// отсоединяемся и удаляем разделяемую память
    for(int i = 0; i<3; i++)
    {
        if(shmdt((void*)data[i]) == -1)
        {
            perror("shmdt");
        }
        if(shmctl(shmid[i], IPC_RMID, NULL) == -1)
        {
            perror("shmctl");
        }
    }
//---
    return 0;
}

int producer(int fd, char *data)
{
    for(int i=0; i<26; i++){
        semop(fd, prod_start, 2);

        printf("%c", data[i]);
        fflush(stdout);
        sleep(1);

        semop(fd, prod_end, 2);
    }
    return 0;
}

int consumer(int fd, char *data)
{
    for(int i=0; i<26; i++){
        semop(fd, cons_start, 2);
        
        printf("%c", data[i]);
        fflush(stdout);
        sleep(1);
        
        semop(fd, cons_end, 2);
    }
    return 0;
}