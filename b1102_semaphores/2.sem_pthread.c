#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>

sem_t sem;
int variable = 0;

void* increment(void* arg)
{
    while(1)
    {
        sem_wait(&sem);
        variable++;
        printf("inc");
        sem_post(&sem);
    }
}

void* decrement(void* arg)
{
    while(1)
    {
        sem_wait(&sem);
        variable--;
        printf("dec");
        sem_post(&sem);
    }
}

int main()
{
    sem_init(&sem, 0, 1);

    pthread_t tid1, tid2;

    pthread_create(&tid1, NULL, increment, NULL);
    pthread_create(&tid2, NULL, decrement, NULL);

    // Wait for the threads to finish
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    printf("The value of the variable is: %d\n", variable);

    sem_destroy(&sem);

    return 0;
}

