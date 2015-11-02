#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>

#define A 10 
#define B 5 
#define LAST_MESSAGE 255
#define N 3

int main()
{
    //int A, B;
    /*scanf("%d", &A);
    scanf("%d", &B);*/
  /* IPC дескриптор для очереди сообщений */
  int msqid; 

  /* Имя файла, использующееся для генерации ключа.
     Файл с таким именем должен существовать в текущей директории */
  char pathname[] = "client_service1.c"; 

  /* IPC ключ */
  key_t key; 

  /* Cчетчик цикла и длина информативной части сообщения */
  int i, len, maxlen; 

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

  /* Генерируем IPC ключ из имени файла 09-1a.c в текущей директории
  и номера экземпляра очереди сообщений 0. */
  if ((key = ftok(pathname, 0)) < 0)
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
  
    mybuf.mtype = 1;
    mybuf.info.a = A;
    mybuf.info.b = B;
    mybuf.info.id = getpid();
    len = sizeof(mybuf.info);

    /* Отсылаем сообщение. В случае ошибки сообщаем об этом и удаляем очередь сообщений из системы. */
    if (msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0)
    {
      printf("Can\'t send message to queue\n");
      msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);
      exit(-1);
    }
    
  mybuf.mtype = LAST_MESSAGE;
  len = 81;
  if (msgsnd(msqid, (struct msgbuf*)&mybuf, len, 0) < 0)
  {
    printf("Can\'t send message to queue\n");
    msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);
    exit(-1);
  }
    
    
    maxlen = sizeof(mybuf.info);

    /*
     * FIXIT: у вас сервер один, а клиентов много. Чтобы клиент знал, какое именно сообщение адресовано ему, мы специально
     * посылаем серверу свой уникальный pid, чтобы потом сервер послал в ответ клиенту сообщение с типом pid, и мы смогли извлечь из очереди именно
     * то, что нам предназначается. Вы же излекаете из очереди сообщение с типом 2, т.е. есть все шансы считать чужое сообщение.    
     */
    
    if ((len = msgrcv(msqid, (struct msgbuf *)&mybuf, maxlen, mybuf.info.id, 0)) < 0)
    {
      printf("Can\'t receive message from queue\n");
      exit(-1);
    }
    
  printf("result = %d\n", mybuf.info.result);
  return 0;
} 