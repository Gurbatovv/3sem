#include <memory.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>

#define MAX_MESSAGE_LENGTH 125

void *ThreadFunctionServerRcv(void *args);
void *ThreadFunctionClientRcv(void *args);
int help();

typedef struct
{
	int isClientActive;
	int activeSockDesc;
	
}clientDesc;

clientDesc clients[10];

int main(int argc, char *argv[])
{
	
	if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0) || (argc != 4))
	{
		printf("argc: %d\n", argc);
		help();
		return 0;
	}
	char serverIp[32];
	int serverPort;
	strcpy(serverIp, argv[2]);
	serverPort = atoi(argv[3]);

	if (strcmp(argv[1], "-s") == 0)
	{
		//server code
		printf("I'm server\n");
		int passiveSockDesc = socket(AF_INET, SOCK_STREAM, 0); // 1 аргумент - сипользование TCP/IP, 2 - использование потока данных (TCP)
		printf("Passive socket desc: %d\n", passiveSockDesc);
		struct sockaddr_in myAddress;
		memset(&myAddress, 0, sizeof(myAddress)); // Заполняет нулями
		myAddress.sin_family = AF_INET; // Адрес семейства TCP/IP
		myAddress.sin_port = htons(serverPort); // Порт нужно перевести в систему, использующуюся в сетевой карте
		if (!inet_aton(serverIp, &(myAddress.sin_addr))) // 127.0.01 - всегда ссылается сам на себя
			printf("Address conversion error\n");
		int bindRes = bind(passiveSockDesc, (struct sockaddr *)&myAddress, sizeof(myAddress));
		printf("bindRes: %d\n", bindRes);
		listen(passiveSockDesc, 10); // переводит сокет в пассивный режим, 10 - будет поддерживаться 10 клиентов
		
		int i = 0;
		for (i = 0; i < 10; i++)
		{
			clients[i].isClientActive = 0;
			clients[i].activeSockDesc = 0;
		}

		pthread_t threadId;
		int threadCounter = 0;
		while(1)
		{
			int activeSockDesc = accept(passiveSockDesc, 0, 0); // Блокирующий вызов - ожидание следующего клиента
			int activeSocketNumber;
			for (i = 0; i < 10; i++)
			{
				if(clients[i].isClientActive == 0)
				{
					clients[i].isClientActive = 1;
					clients[i].activeSockDesc = activeSockDesc;
					activeSocketNumber = i;
					
					break;
				}
				if (i == 9)
				{
					printf("10 clients now on the server\n");
					while(1);
				}
			}
			int threadServerRcvArg[3];
			threadServerRcvArg[1] = activeSockDesc;
			threadServerRcvArg[2] = passiveSockDesc;
			threadServerRcvArg[3] = activeSocketNumber;
			
			pthread_create(&threadId, 0, ThreadFunctionServerRcv, &threadServerRcvArg);
		}
		close(passiveSockDesc);
	}
	else if (strcmp(argv[1], "-c") == 0)
	{
		// client code
		printf("I'm client\n");
		int socketDesc = socket(AF_INET, SOCK_STREAM, 0); // 1 аргумент - сипользование TCP/IP, 2 - использование потока данных (TCP)
		printf("Passive socket desc: %d\n", socketDesc);
		struct sockaddr_in serverAddress;
		memset(&serverAddress, 0, sizeof(serverAddress)); // Заполняет нулями
		serverAddress.sin_family = AF_INET; // Адрес семейства TCP/IP
		serverAddress.sin_port = htons(serverPort); // Порт нужно перевести в систему, использующуюся в сетевой карте
		if (!inet_aton(serverIp, &(serverAddress.sin_addr))) // 127.0.0.1 - всегда ссылается сам на себя
			printf("Address conversion error\n");
		int connectRes = connect(socketDesc, (struct sockaddr *)&serverAddress, sizeof(serverAddress));  // Подсоединяет сокет дескриптор к серверу с данным адресом. Блокируется до тех пор, пока не будет создан сервер с таким ip
		printf("connectRes: %d\n", connectRes);
        
        char str[MAX_MESSAGE_LENGTH], name[MAX_MESSAGE_LENGTH], sendline[MAX_MESSAGE_LENGTH];
        bzero(&name, strlen(name) + 1);
		printf("Enter your name\n");
        gets(name);
		
        
        pthread_t threadId;
		pthread_create(&threadId, 0, ThreadFunctionClientRcv, &socketDesc); // messages rcv and print
		
		
        
		while(1) // message send
		{
            strcat(sendline, name);
            strcat(sendline, ": ");
			gets(str);
            strcat(sendline, str);
			write(socketDesc, sendline, strlen(sendline) + 1);
            bzero(&sendline, strlen(sendline) + 1);
		}
		close(socketDesc);
	}
	else
		help();
	return 0;
}

void *ThreadFunctionServerRcv(void *args)
{
	int activeSockDesc = ((int *)args)[1];
	int passiveSockDesc = ((int *)args)[2];
	int activeSocketNumber = ((int *)args)[3];
	activeSockDesc = clients[activeSocketNumber].activeSockDesc;
	char buf[MAX_MESSAGE_LENGTH];
    //bzero(&buf, strlen(buf) + 1);
	int i = 0;
	while(1)
	{
		if(read(activeSockDesc, buf, sizeof(buf)))
		{
			printf("%s\n", buf);
			for (i = 0; i < 10; i++)
			{
				if (i == activeSocketNumber)
					break;
				if (clients[i].isClientActive == 1)
					write(clients[i].activeSockDesc, buf, strlen(buf) +1);
			}
		}
		else
			break;
		
	}
	close(activeSockDesc);
	
}

void *ThreadFunctionClientRcv(void *args) 
{
    printf("Client Sender is ready\n");
	int socketDesc = *((int *)args);
	char buf[MAX_MESSAGE_LENGTH];
    //bzero(&buf, strlen(buf) + 1);
	while(1)
	{
		//printf("read\n");
		read(socketDesc, buf, sizeof(buf));
		printf("%s\n", buf);
	}
}

int help()
{
	printf("Help:\n\
\t1 arg: -s to run server, -c to run client\n\
\t2 arg: server IP\n\
\t3 arg: server port\n");
	return 0;
}
