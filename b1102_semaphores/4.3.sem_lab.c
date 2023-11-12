/*
    TASK Лаба 7: Семафор и разделяемая память
    без указателей - работает
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
#define Q_SIZE 30

bool addElementToQueue(char*, char*, char*, char*, char, char*added);
bool deleteElementFromQueue(char*, char*, char*, char*, char*);

int addLastElement = 0;

// se = 0, sf = 1, sb = 2;
struct sembuf prod_start[2] = {{0,-1, 0}, {2,-1, 0}};
struct sembuf prod_end[2] = {{2, 1, 1}, {1, 1, 1}};
struct sembuf cons_start[2] = {{1,-1, 0}, {2,-1, 0}};
struct sembuf cons_end[2] = {{2, 1, 1}, {0, 1, 1}};

int producer(int fd, char *data, char *startPos, char *endPosition, char *fpos, char **lpos);
int consumer(int fd, char *data, char *startPos, char *endPosition, char **fpos, char **lpos);

int main()
{
    pid_t cpid[2], w;
    int wstatus;

// установка разделяемой памяти
    char s1[52] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    int shmid_one;
    char *data;
    if((shmid_one = shmget(100, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(1);
    }
    if ((data = shmat(shmid_one, NULL, 0)) == (void *)-1) {
        perror("shmat");
        exit(1);
    }
    strcpy(data, s1);
// очередь
    int shmid_two;
    char *startPos;
    if((shmid_two = shmget(101, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(1);
    }
    if ((startPos = (char*)shmat(shmid_two, NULL, 0)) == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    int shmid_three;
    int *start;
    if((shmid_three = shmget(102, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(1);
    }
    if ((start = (int*)shmat(shmid_three, NULL, 0)) == (void *)-1) {
        perror("shmat");
        exit(1);
    }
    *start = 0;

    int shmid_four;
    int *end;
    if((shmid_four = shmget(103, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(1);
    }
    if ((end = (int*)shmat(shmid_four, NULL, 0)) == (void *)-1) {
        perror("shmat");
        exit(1);
    }
    *end = 0;

    

    char *endPosition;
    endPosition = startPos + Q_SIZE - 1;
    
    char *fpos;
    fpos = startPos;

    char *lpos;
    lpos = startPos;

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
            if(i % 2 == 0)
            {
                // producer(fd, data, startPos, endPosition, fpos, &lpos);
                char value;
                int j = 0;
                while(1)
                {
                    semop(fd, prod_start, 2);
                    value = data[j++];
                    if(j == 51) j = 0;

                    // printf("%d %c", i, value);
                    // fflush(stdout);
                    startPos[*start] = value;
                    *start+=1;
                    if(*start == 10) *start = 0; 
                    

                    sleep(1);

                    semop(fd, prod_end, 2);
                }
            } else {
                // consumer(fd, data, startPos, endPosition, &fpos, &lpos);
                bool check = 0;
                char dr_v;
                char *dropedValue = &dr_v;
                while(1)
                {
                    semop(fd, cons_start, 2);

                    if(*start != *end)
                    {
                        printf("%c", startPos[*end]);
                        fflush(stdout);
                        *end+=1;
                        if(*end == 10) *end = 0;
                    }
                    
                    

                    sleep(1);
                    
                    semop(fd, cons_end, 2);
                }
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
//---
    return 0;
}

int producer(int fd, char *data, char *startPos, char *endPosition, char *fpos, char **lpos)
{
    // bool check = 0;
    // char value;
    // int i = 0;
    // char added_v;
    // char*added =&added_v;
    // for(int j = 0; j < 100; j++)
    // {
    //     semop(fd, prod_start, 2);
    //     value = data[i++];
    //     if(i == 51) i = 0;

    //     // printf("%d %c", i, value);
    //     // fflush(stdout);

    //     check = addElementToQueue(fpos, &lpos, startPos, endPosition, value, added);
    //     if(check == 1)
    //     {
    //         printf("Очередь полна");
    //         fflush(stdout);
    //     }
    //     // printf("%s", added);
    //     // fflush(stdout);

    //     sleep(1);

    //     semop(fd, prod_end, 2);
    // }
    return 0;
}

int consumer(int fd, char *data, char *startPos, char *endPosition, char **fpos, char **lpos)
{
    // bool check = 0;
    // char dr_v;
    // char *dropedValue = &dr_v;
    // while(1)
    // {
    //     semop(fd, cons_start, 2);
        
    //     check = deleteElementFromQueue(&fpos, &lpos, startPos, endPosition, dropedValue);
    //     if(check == 1)
    //     {
    //         printf("Очередь пуста");
    //         fflush(stdout);
    //     }
    //     printf("%s", dropedValue);
    //     fflush(stdout);
    //     sleep(1);
        
    //     semop(fd, cons_end, 2);
    // }
    return 0;
}

bool addElementToQueue(char *fpos, char *lpos, char *startPos, char *endPosition, char value, char*added)
{
    if((lpos == endPosition) && (startPos != fpos))
    {
        lpos = startPos;
        if(addLastElement == 0)
        {
            *endPosition = value;
        }
        addLastElement = 0;
    } else if((lpos == endPosition) && (startPos == fpos))
    {
        if(addLastElement == 0) 
        {
            *lpos = value;
            *added = *lpos;
            addLastElement = 1;
            return 0;
        }
        return 1;
    } else if(lpos+1 == fpos)
    {
        if(addLastElement == 0)
        {
            *lpos = value;
            *added = *lpos;
            addLastElement = 1;
            return 0;
        }
        // std::cout << "Очередь полна" << std::endl;
        return 1;
    } else {
        lpos+=1;
        *(lpos - 1) = value;
        *added = *(lpos - 1);
    }
    
    // printf("%c", *(*lpos - 1));
    // fflush(stdout);
    return 0;
}

bool deleteElementFromQueue(char *fpos, char *lpos, char *startPos, char *endPosition, char *dropedValue)
{
    if(lpos == fpos)
    {
        // std::cout << "Очередь пуста" << std::endl;
        return 1;
    }
    int dropedElement = *fpos;
    // *fpos = 0;

    // if(*lpos+1 != *fpos) addLastElement = 0;

    if((lpos == endPosition) && (startPos == fpos) && (addLastElement == 1))
    {
        lpos = startPos;
        addLastElement = 0;
    } else if((lpos+1 == fpos)) 
    {
        if(addLastElement == 1) // указатель не проедет дальше при следующем удалении, останется на месте
        {
            lpos+=1;
            // addLastElement = 0;
        }
        
    }

    if(endPosition == fpos)
    {
        fpos = startPos;
    } else {
        fpos+=1;
    }
    *dropedValue = dropedElement;
    return 0;
}