#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define  k 1e2

int a[3] = {0, 0, 0};
struct sembuf mybuf;
int semid;
   
    
void* my_thread1(void* dummy) 
{
    pthread_t my_thread_id;
    int i;
  
    for (i = 0; i < k; i++)
    {    
        mybuf.sem_op = 1;
        mybuf.sem_flg = 0;
        mybuf.sem_num = 0;
        /*
         * Если вы не вызвали системный вызов semop, то семафор не изменит своего значения.
         * Ваша задача, чтобы к общей на оба потока глобальной переменной a[0] одновременно нельзя было обратиться сразу из двух потоков.
         * Как это делается?
         * Представьте, что семафор - это некая эстафетная палка: у кого она есть, тот и бежит (работает с общей переменной a[0]).
         * Пусть изначально значение семафора 1. Далее какая-то из 2х нитей первой забирает себе эстафетную палку (вызов sem_op = -1), и значение семафора становится 0.
         * Второй нити приходится ждать, пока 1я вернёт эстафетную палку на место (вызов sem_op = +1).
         * 
         * Т.е. до a[0]++; надо вызвать
         * 
         * mybuf.sem_op  = -1;
           mybuf.sem_flg =  0;
           mybuf.sem_num =  0;
           semop(semid , &mybuf , 1);
           
           а после вернуть значение семафора обратно
           
           mybuf.sem_op  = 1;
           mybuf.sem_flg = 0;
           mybuf.sem_num = 0;
           semop(semid , &mybuf , 1);
           
           Для второй нити тоже самое.
           
           И главная ваша ошибка! У вас переменная mybuf тоже глобальная, т.е. общая на обе нити. За неё нитям тоже приходится "соревноваться"!
           Её необходимо сделать локальной для каждой нити.
         */
        a[0]++;
       /* mybuf.sem_op = 1;
        mybuf.sem_flg = 0;
        mybuf.sem_num = 0;*/
        a[1]++;
        
    } 
    if (semop(semid , &mybuf , 1) < 0) {
        printf("Can`t wait for condition\n");
        exit(-1);}
    return NULL;
}

void* my_thread2(void* dummy) 
{   
    pthread_t my_thread_id;
    int i;
    
    for (i = 0; i < k; i++)
    {
        mybuf.sem_op = -1;
        mybuf.sem_flg = 0;
        mybuf.sem_num = 0;
        a[0]++;
        /*mybuf.sem_op = 1;
        mybuf.sem_flg = 0;
        mybuf.sem_num = 0;*/
        a[2]++;
        
    } 
     if (semop(semid , &mybuf , 1) < 0) {
        printf("Can`t wait for condition\n");
        exit(-1);}
    return NULL;
}

int main()
{
    const char pathname[] = "s06e01a.c";
    key_t key;
    int numb = 0;
      
    /*key = ftok(pathname , 0);
    semid = semget(key , 1 , 0666 | IPC_CREAT);*/
    if ( (key = ftok(pathname , 0)) < 0 ) {
        printf("Can`t generate key\n");
        exit(-1);
    }
    
    if ((semid = semget(key , 1 , 0666 | IPC_CREAT)) < 0) {
        printf("Can`t get semid\n");
        exit(-1);
    }

    mybuf.sem_op = 1;
    mybuf.sem_flg = 0;
    mybuf.sem_num = 0;
    
    
    semop(semid, &mybuf, 1);
    
    
    pthread_t thread_id1 , thread_id2, my_thread_id;
    
    int result;
    
    result = pthread_create(&thread_id1 , 
                            (pthread_attr_t *)NULL , 
                            my_thread1 ,
                            NULL);
    
    result = pthread_create(&thread_id2 , 
                            (pthread_attr_t *)NULL , 
                            my_thread2 ,
                            NULL);
    
    pthread_join(thread_id1 , (void **) NULL);
    pthread_join(thread_id2 , (void **) NULL);
    
    /*
     После того, как ваша программа отработает, значение семафора будет равным 1.
     Т.е. если вы повторно запустите свою программу, то перед запуском нитей его значение уже станет 2, и никакая блокировка в нитях уже не будет работать.
     Чтобы таких сложностей не возникло, в конце программы удалите семафор с помощью вызова semctl.
     */
    
    printf("%d %d %d %d\n" ,a[0], a[1], a[0] + a[1], a[2]);
    return 0;
}    
