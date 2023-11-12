/*
    TASK Лаба 7: Семафор и разделяемая память
    Пишем в буфер, а потом читаем из него
    без указателей - работает
    прикрутить обработчик сигнала - работает
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
#include <stdbool.h>

#define SHM_SIZE 1024
#define Q_SIZE 20

// se = 0, sf = 1, sb = 2;
struct sembuf prod_start[2] = {{0,-1, 0}, {2,-1, 0}};
struct sembuf prod_end[2] = {{2, 1, 1}, {1, 1, 1}};
struct sembuf cons_start[2] = {{1,-1, 0}, {2,-1, 0}};
struct sembuf cons_end[2] = {{2, 1, 1}, {0, 1, 1}};

int producer(int fd, char *data, char *startPos, int *start);
int consumer(int fd, char *startPos, int *start, int *end);

short flag = 1;
void signal_handler(int sig_num)
{
    printf("\n");
    printf("sig num : %d, pid: %d, ppid %d \n", sig_num, getpid(), getppid());
	flag = 0;
}

int main()
{
    pid_t cpid[2], w;
    int wstatus;
//сигнал
    if(signal(SIGINT, signal_handler)==SIG_ERR) // проверка на ошибку сигнала
    {
        perror("signal");
        exit(1);
    } 
//---

// установка разделяемой памяти
    char s1[52] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    int shmid_one;
    char *data;
    if((shmid_one = shmget(100, SHM_SIZE, 0644 | IPC_CREAT)) == -1) 
    {
        perror("shmget");
        exit(1);
    }
    if ((data = shmat(shmid_one, NULL, 0)) == (void *)-1) 
    {
        perror("shmat");
        exit(1);
    }
    strcpy(data, s1);

    // очередь
    int shmid_two;
    char *startPos;
    if((shmid_two = shmget(101, SHM_SIZE, 0644 | IPC_CREAT)) == -1) 
    {
        perror("shmget");
        exit(1);
    }
    if ((startPos = (char*)shmat(shmid_two, NULL, 0)) == (void *)-1) 
    {
        perror("shmat");
        exit(1);
    }

    int shmid_three;
    int *start;
    if((shmid_three = shmget(102, SHM_SIZE, 0644 | IPC_CREAT)) == -1) 
    {
        perror("shmget");
        exit(1);
    }
    if ((start = (int*)shmat(shmid_three, NULL, 0)) == (void *)-1) 
    {
        perror("shmat");
        exit(1);
    }
    *start = 0;

    int shmid_four;
    int *end;
    if((shmid_four = shmget(103, SHM_SIZE, 0644 | IPC_CREAT)) == -1) 
    {
        perror("shmget");
        exit(1);
    }
    if ((end = (int*)shmat(shmid_four, NULL, 0)) == (void *)-1) 
    {
        perror("shmat");
        exit(1);
    }
    *end = 0;

//---
// установка семафора
    int perms = S_IRWXU | S_IRWXG | S_IRWXO;
    int fd = semget(99, 2, IPC_CREAT | perms);
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
            if(i % 2 == 0) producer(fd, data, startPos, start);
            else consumer(fd, startPos, start, end);

            exit(EXIT_SUCCESS);
        } else {
            // printf("PP: childpid=%d\n", cpid[i]);
        }
    }
//---

//дожидаемся
    for(int	i = 0; i < 6; i++)
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

    if(shmdt((void*)data) == -1)
    {
        perror("shmdt");
    }
    if(shmctl(shmid_one, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
    }

    if(shmdt((void*)startPos) == -1)
    {
        perror("shmdt");
    }
    if(shmctl(shmid_two, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
    }

    if(shmdt((void*)start) == -1)
    {
        perror("shmdt");
    }
    if(shmctl(shmid_three, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
    }

    if(shmdt((void*)end) == -1)
    {
        perror("shmdt");
    }
    if(shmctl(shmid_four, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
    }
//---
    return 0;
}

int producer(int fd, char *data, char *startPos, int *start)
{
    char value;
    int j = 0;
    while(flag)
    {
        semop(fd, prod_start, 2);

        value = data[j++];
        if(j == 52) j = 0;

        startPos[*start] = value;
        *start += 1;
        if(*start == Q_SIZE) *start = 0; 
        sleep(1);

        semop(fd, prod_end, 2);
    }
    return 0;
}

int consumer(int fd, char *startPos, int *start, int *end)
{
    while(flag)
    {
        semop(fd, cons_start, 2);

        if(*start != *end)
        {
            printf("%c", startPos[*end]);
            fflush(stdout);
            *end += 1;
            if(*end == Q_SIZE) *end = 0;
        }
        sleep(1);
        
        semop(fd, cons_end, 2);
    }
    return 0;
}
