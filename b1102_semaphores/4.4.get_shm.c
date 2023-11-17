/*
    TASK Лаба 4: Получили доступ к разделяемой памяти из файла 4.4.sem_lab.c
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


int main()
{

// установка разделяемой памяти

    int shmid_one;
    char *data;
    int perms = S_IRWXU | S_IRWXG | S_IRWXO;
    if((shmid_one = shmget(100, SHM_SIZE, perms)) == -1) 
    {
        perror("shmget");
        exit(1);
    }
    if ((data = shmat(shmid_one, NULL, 0)) == (void *)-1) 
    {
        perror("shmat");
        exit(1);
    }

    printf("%c", data[5]);


// отсоединяемся от разделяемой памяти

    if(shmdt((void*)data) == -1)
    {
        perror("shmdt");
    }

    return 0;
}

