#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

/*
 * Старайтесь называть переменные так, чтобы из названия было ясно, зачем она нужна:
 * MAX_FILE_SIZE
 */
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
   /*
    * Форматирование: пробелы вокруг бинарных операций.
    */
   s = (char *)malloc(n*sizeof(char));
   int fd;
   /*
    * Форматирование: пробел после запятой.
    */
   fd = open("01a.c",O_RDONLY);
   for (i = 0; i < n; i++)
   {
       /*
        * Это нужно делать с помощью цикла while
        */
       read (fd, &(s[i]), 1);
   }

   /*
    * Магическое число 2500. Нужно объявить соответствующую константу.
    * Почему sizeof(int)? Вам же нужен массив char. Возможно вы и выделите сколько надо, но сторонний человек точно не поймёт, откуда int`ы появились.
    */
   if((shmid = shmget(key, 2500 * sizeof(int), 0666 | IPC_CREAT | IPC_EXCL)) < 0){    
       if(errno != EEXIST) {         
            printf("Can't create shared memory\n");
            exit(-1);
        } else {
          /*
           * Вы явно хотели не 3 написать.
           */
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
   /*
    * Вместо этого цикла воспользуйтесь ф-ей memcpy.
    */
   for (i = 0; i < n; i++)
   {
       array[i] = s[i];       
   }
   /*
    * Уверены?
    * Нужно ведь просто
    * close(fd);
    */
   int close(int fd);
}   
   
