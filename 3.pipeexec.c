#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h> //O_WRONLY



int main(int argc, char *argv[])
{
    sleep(2);
    int fd = open("./b0901/mpipe", O_RDONLY);
    int x;
    if(read(fd, &x, sizeof(x)) == -1) exit(1);
    printf("Received: %d \n", x);
    close(fd);
    return 0;
}