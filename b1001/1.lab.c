// смотрим как наследуется дочерний элемент child orphan ppid
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int main()
{

    pid_t childpid[2];

    for(int	i=0;i<2;i++)
    {
        if((childpid[i]=fork())==-1)
        {
            perror("Can’t fork\n");
            exit(1);
        } else if(childpid[i]==0){
            printf("Child %d process:pid=%d, ppid=%d, pgrp=%d\n", i, getpid(), getppid(), getpgrp());
            sleep(2);
            printf("Child process(orphan):pid=%d, ppid=%d, pgrp=%d\n", getpid(), getppid(), getpgrp());
            exit(0);
        } else {
            // sleep(1);
            printf("Parent process:pid=%d, pgrp=%d, childpid=%d\n", getpid(), getpgrp(), childpid[i]);
        }
    }



    return 0;
}
