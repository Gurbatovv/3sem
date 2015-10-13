#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define SIZE 2000

int main()
{
    char    *array;
    int     shmid;
      
    const char pathname[] = "01a.c";
    key_t   key;

    if((key = ftok(pathname,0)) < 0)
    {
        printf("Can\'t generate key\n");
        exit(-1);
    }
   
   
    shmid = shmget(key, SIZE * sizeof(char), 0);
    
    array = (char *)shmat(shmid, NULL, 0);
    
    printf("%s\n", array);
    
    if(shmdt(array) < 0)
    {
        printf("Can't detach shared memory\n");
        exit(-1);
    }

   return 0;
} 

