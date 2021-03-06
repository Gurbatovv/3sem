#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define N 3
#define LAST_MESSAGE 255

int main()
{
  /* IPC дескриптор для очереди сообщений */
  int msqid; 

  /* Имя файла, использующееся для генерации ключа.
  Файл с таким именем должен существовать в текущей директории */
  char pathname[] = "client_service1.c"; 

  /* IPC ключ */
  key_t key; 

  /* Реальная длина и максимальная длина информативной части сообщения */
  int len, maxlen, i; 

  /* Ниже следует пользовательская структура для сообщения */
  struct mymsgbuf
  {
    long mtype;
    struct
    {
        int a;
        int b;
        int result;
        pid_t id;
     }info;
  } mybuf;

  int semid;
  const char pathname1[] = "client_service2.c";
  key_t key1;
  struct sembuf mybufs; /* Структура для семафорa */

    if ( (key1 = ftok(pathname1 , 0)) < 0 ) {
        printf("Can`t generate key\n");
        exit(-1);
    }
  
  if ((semid = semget(key1 , 1 , 0666 | IPC_CREAT)) < 0) {
        printf("Can`t get semid\n");
        exit(-1);
    }
  
  
  /* Генерируем IPC ключ из имени файла 09-1a.c в текущей директории
  и номера экземпляра очереди сообщений 0 */

  if((key = ftok(pathname, 0)) < 0)
  {
    printf("Can\'t generate key\n");
    exit(-1);
  }

  /* Пытаемся получить доступ по ключу к очереди сообщений, если она существует,
  или создать ее, если она еще не существует, с правами доступа
  read & write для всех пользователей */

  if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0)
  {
    printf("Can\'t get msqid\n");
    exit(-1);
  }

    /* В бесконечном цикле принимаем сообщения любого типа в порядке FIFO с 
     * максимальной длиной информативной части 81 символ до тех пор, пока не 
     * поступит сообщение с типом LAST_MESSAGE */
    mybufs.sem_op = N; 
    mybufs.sem_flg = 0;
    mybufs.sem_num = 0;
    semop(semid, &mybufs, 1);
    
  while (1)
  {
    maxlen = sizeof(mybuf.info);

    if ((len = msgrcv(msqid, (struct msgbuf *)&mybuf, maxlen, 1, 0)) < 0)
    {
      printf("Can\'t receive message from queue\n");
      exit(-1);
    }
    
    if (mybuf.mtype == LAST_MESSAGE)
    {
      msgctl(msqid, IPC_RMID, (struct msqid_ds *)NULL);
      exit(0);
    }

    //printf("message type = %ld, info = %d\n, %d, id %d\n", mybuf.mtype, mybuf.info.a, mybuf.info.b, mybuf.info.id);
  

    /*
     * FIXIT:
     * Инциализировать семафор числом N необходимо до цикла один раз.
     * 
     * У вас значение семафора - это то, сколько ещё процессов можно одновременно с уже работающими запустить.
     * Когда вы запускаете новую задачу, то должны уменьшить на 1 это значение.
     * Когда дочерний процесс доработал до конца, надо увеличить это значение на 1.
     */
    
    int pid = fork();

    if(pid == 0)
    {

      mybufs.sem_op = -1; 
      mybufs.sem_flg = 0;
      mybufs.sem_num = 0;
      semop(semid, &mybufs, 1);

      if(semop(semid, &mybufs, 1) < 0)
      {
        printf("Can't wait for condition\n");
      }
    
    
    /*
     * FIXIT:
     * Умножение чисел и отправку их обратно клиету должен также совершать дочерний процесс.
     * После того, как он всё это сделал, надо "прибить" дочерний процесс, вызвав в нем exit, иначе на следующей итерации уже и родитель и первый ребёнок будут принимать новые сообщения
     * от клиентов.
     */
    int m = 0;
    m = mybuf.info.a * mybuf.info.b;
    
    mybuf.info.result = m;    
    
    mybuf.mtype = mybuf.info.id;
    len = sizeof(mybuf.info);
        if (msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0)
    {
      printf("Can\'t send message to queue\n");
      msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);
      exit(-1);
    }
    mybufs.sem_op = 1;
    mybufs.sem_flg = 0;
    mybufs.sem_num = 0;
    semop(semid, &mybufs, 1);
    exit(0);
  }
  }
  
  return 0;
} 