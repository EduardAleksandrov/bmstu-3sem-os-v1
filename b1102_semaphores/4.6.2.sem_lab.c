/*
    TASK Лаба 4: Семафор и разделяемая память, производство/потребление
    Пишем в буфер, а потом читаем из него

    !комментарии преподавателя
    одна разделяемая память(первый сегмент cur_prod, второй cur_cons, третий letter, четвертый очередь)
    выделены в функции
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
#define Q_SIZE 10

// se = 0, sf = 1, sb = 2;
struct sembuf prod_start[2] = {{0,-1, 0}, {2,-1, 0}};
struct sembuf prod_end[2] = {{2, 1, 1}, {1, 1, 1}};
struct sembuf cons_start[2] = {{1,-1, 0}, {2,-1, 0}};
struct sembuf cons_end[2] = {{2, 1, 1}, {0, 1, 1}};

int producer(int fd, char *letter, char **cur_prod, char *begin_addr, char *end_addr);
int consumer(int fd, char **cur_cons, char **cur_prod, char *begin_addr, char *end_addr);

short flag = 1;
void signal_handler(int sig_num)
{
    printf("\n");
    printf("sig num : %d, pid: %d, ppid: %d \n", sig_num, getpid(), getppid());
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
    char *begin_addr = letter + sizeof(char); // начало очереди
    (*cur_prod) = begin_addr;
    (*cur_cons) = begin_addr;

    char *end_addr = begin_addr + Q_SIZE; // конец очереди

    (*letter) = 'a';

//---
// установка семафора
    int fd = semget(99, 3, IPC_CREAT | perms);
    if(fd==-1)
    {
        perror("semop");
        exit(EXIT_FAILURE);
    }
    
    if(semctl(fd, 0, SETVAL, 5) < 0) //установка начальных значений для семафоров  se = 0, sf = 1, sb = 2;
    {
        printf("Ошибка 1\n");
        exit(EXIT_FAILURE);
    }
    if(semctl(fd, 1, SETVAL, 0) < 0)
    {
        printf("Ошибка 2\n");
        exit(EXIT_FAILURE);
    }
    if(semctl(fd, 2, SETVAL, 1) < 0)
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
            if(i % 2 == 0) producer(fd, letter, cur_prod, begin_addr, end_addr);
            else consumer(fd, cur_cons, cur_prod, begin_addr, end_addr);

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
    {
        perror("shmdt");
    }
    if(shmctl(shmid_one, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
    }
//---

// удаляем группу семафоров
    if (semctl(fd, 0, IPC_RMID) == -1) 
    {
        perror("semctl IPC_RMID");
    }
//---

    return 0;
}

int producer(int fd, char *letter, char **cur_prod, char *begin_addr, char *end_addr)
{
    int j = 0;
    while(flag)
    {
        semop(fd, prod_start, 2);

        (*letter) = 'a' + j;
        j++;
        if(j == 26) j = 0;
        
        (**cur_prod) = (*letter);
        (*cur_prod)++;
        if(end_addr == (*cur_prod)) (*cur_prod) = begin_addr; // циклическая очередь

        sleep(1);

        semop(fd, prod_end, 2);
    }
    return 0;
}

int consumer(int fd, char **cur_cons, char **cur_prod, char *begin_addr, char *end_addr)
{
    while(flag)
    {
        semop(fd, cons_start, 2);

        if((*cur_cons) != (*cur_prod))
        {   
            printf("%c", **cur_cons);
            fflush(stdout);
            (*cur_cons)++;
            if(end_addr == (*cur_cons)) (*cur_cons) = begin_addr; // циклическая очередь
        }

        sleep(1);
        
        semop(fd, cons_end, 2);
    }
    return 0;
}
