/*
    TASK Лаба 4: Семафор и разделяемая память, производство/потребление
    Пишем в буфер, а потом читаем из него

    !комментарии преподавателя
    одна разделяемая память(первый сегмент cur_prod, второй cur_cons, третий letter, четвертый очередь)
    выделены в функции
    !комментарии преподавателя
    работает
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
#include <time.h>

#define SHM_SIZE 1024

// se = 0, sf = 1, sb = 2;
struct sembuf prod_start[2] = {{0,-1, 0}, {2,-1, 0}};
struct sembuf prod_end[2] = {{2, 1, 1}, {1, 1, 1}};
struct sembuf cons_start[2] = {{1,-1, 0}, {2,-1, 0}};
struct sembuf cons_end[2] = {{2, 1, 1}, {0, 1, 1}};

void producer(int fd_sem, char *letter, char **cur_prod);
void consumer(int fd_sem, char **cur_cons);

short flag = 1;
void signal_handler(int sig_num)
{
    printf("\n");
    printf("sig num : %d, pid: %d \n", sig_num, getpid());
	flag = 0;
}

int main()
{
    pid_t cpid[6], w;
    int wstatus;
//сигнал
    if(signal(SIGINT, signal_handler)==SIG_ERR) // проверка на ошибку сигнала
    {
        perror("signal");
        exit(1);
    } 
//---

// установка разделяемой памяти

    int perms = S_IRWXU | S_IRWXG | S_IRWXO;

    int shmid_one;
    char *addr;
    if((shmid_one = shmget(100, SHM_SIZE, IPC_CREAT | perms)) == -1)  // 0644|IPC_CREAT     0666|IPC_CREAT
    {
        perror("shmget");
        exit(1);
    }
    if ((addr = shmat(shmid_one, NULL, 0)) == (void *)-1) 
    {
        perror("shmat");
        exit(1);
    }
    char **cur_prod;
    char **cur_cons;
    char *letter;

    cur_prod = (char **) addr;
    cur_cons = (char **) (addr + sizeof(char*));
    letter = (char *) (cur_cons + sizeof(char*));
    char *begin_addr = letter + sizeof(char);
    (*cur_prod) = begin_addr;
    (*cur_cons) = begin_addr;

    (*letter) = 'a';

//---
// установка семафора
    int fd_sem = semget(99, 3, IPC_CREAT | perms);
    if(fd_sem==-1)
    {
        perror("semop");
        exit(EXIT_FAILURE);
    }
    
    if(semctl(fd_sem, 0, SETVAL, 5) < 0) //установка начальных значений для семафоров  se = 0, sf = 1, sb = 2;
    {
        printf("Ошибка 1\n");
        exit(EXIT_FAILURE);
    }
    if(semctl(fd_sem, 1, SETVAL, 0) < 0)
    {
        printf("Ошибка 2\n");
        exit(EXIT_FAILURE);
    }
    if(semctl(fd_sem, 2, SETVAL, 1) < 0)
    {
        printf("Ошибка 3\n");
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
            if(i % 2 == 0) producer(fd_sem, letter, cur_prod);
            else consumer(fd_sem, cur_cons);

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
    if(shmdt((void*)addr) == -1)
        perror("shmdt");
    if(shmctl(shmid_one, IPC_RMID, NULL) == -1)
        perror("shmctl");
//---

// удаляем группу семафоров
    if (semctl(fd_sem, 0, IPC_RMID) == -1) 
        perror("semctl IPC_RMID");
//---

    return 0;
}

void producer(int fd_sem, char *letter, char **cur_prod)
{
    srand(time(NULL));
    int min = 100000, max = 999999; // микросекунды (0.1 second)

    while(flag)
    {
        int time_i = min + rand() % (max - min + 1); // interval

        semop(fd_sem, prod_start, 2);

        (**cur_prod) = (*letter);
        (*cur_prod)++;

        (*letter)++;
        if((*letter) == '{') (*letter) = 'a'; // { - знак после z

        usleep(time_i);

        semop(fd_sem, prod_end, 2);
    }
    exit(EXIT_SUCCESS);
}

void consumer(int fd_sem, char **cur_cons)
{
    while(flag)
    {
        semop(fd_sem, cons_start, 2);
 
        printf("%c", **cur_cons);
        fflush(stdout);
        (*cur_cons)++;
        
        semop(fd_sem, cons_end, 2);
    }
    exit(EXIT_SUCCESS);
}
