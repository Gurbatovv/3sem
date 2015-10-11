#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#define n 5000


int main()
{
    char     *array;
    int     shmid, i; 
    int     new = 1;
    const char pathname[] = "01a.c";
    key_t   key;

    if((key = ftok(pathname,0)) < 0)
    {
        printf("Can\'t generate key\n");
        exit(-1);
    }
   
    shmid = shmget(key, 2500 * sizeof(int), 0666 | IPC_CREAT | IPC_EXCL);
    shmid = shmget(key, 3 * sizeof(int), 0);
    
    array = (char *)shmat(shmid, NULL, 0);
    
    printf("%s\n", array);
    
    if(shmdt(array) < 0)
    {
        printf("Can't detach shared memory\n");
        exit(-1);
        
    }

   return 0;
} 
