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
  
   if((key = ftok(pathname , 0)) < 0){ 
     printf("Can\'t generate key\n");
     exit(-1);
   }
   
   char *s;
   s = (char *)malloc(n*sizeof(char));
   int fd;
   fd = open("01a.c",O_RDONLY);
   for (i = 0; i < n; i++)
   {
       read (fd, &(s[i]), 1);
   }
  
   if((shmid = shmget(key, 2500 * sizeof(int), 0666 | IPC_CREAT | IPC_EXCL)) < 0){    
       if(errno != EEXIST) {         
            printf("Can't create shared memory\n");
            exit(-1);
        } else {
          if((shmid = shmget(key, 3 * sizeof(int), 0)) < 0){
            printf("Can't find shared memory\n");
            exit(-1);
         }
         new = 0;
        }
    }
    
    if((array = (char *)shmat(shmid, NULL, 0)) == (char *)(-1)){
      printf("Can't attach shared memory\n");
      exit(-1);
   }
   for (i = 0; i < n; i++)
   {
       array[i] = s[i];       
   }  
   int close(int fd);
}   
   
