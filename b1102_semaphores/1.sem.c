//особенности работы функции fork
#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
};

struct sembuf sm1 = {0, -1, SEM_UNDO};
struct sembuf sm2 = {0, +1, SEM_UNDO};


int main(){
//  создаем семафор    
    int ids = semget(5555, 1, 0666 | IPC_CREAT);
    if(ids < 0) {
        printf("Ошибка создания семафора\n");
        return 1;
    }
    union semun u;
    u.val = 1;
//  переводим семафор в состояние готовность 
    if(semctl(ids, 0, SETVAL, u) < 0)
    {
        printf("Ошибка\n");
        return 1;
    }
    pid_t t;
    char * s1 = "abcdefghijklmnopqrstuvwxyz";
    char * s2 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
//создаем процесс
    t=fork();
    if(t==-1){
       printf("Ошибка\n");
       return 1;
    }
    if(!t){ //  дочерний процесс
        for(int i=0; i<26; i++){
           semop(ids, &sm1, 1);
           printf("%c",s1[i]);
           fflush(stdout);
           sleep(1);
           printf("%c",s1[i]);
           fflush(stdout);
           semop(ids, &sm2, 1);
        }
    }else{ // родительский процесс
        for(int i=0; i<26; i++){
           semop(ids, &sm1, 1);
           printf("%c",s2[i]);
           fflush(stdout);
           sleep(1);
           printf("%c",s2[i]);
           fflush(stdout);
           semop(ids, &sm2, 1);
        }
    }
    return 0;
}