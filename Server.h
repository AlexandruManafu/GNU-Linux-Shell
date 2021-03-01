#ifndef SERVER_H
#define SERVER_H

#include "HelperFunctions.h"

struct QueueNode
{
	int val;
	struct QueueNode *next;
	
};
struct Queue
{
	struct QueueNode *top; //NULL by default
	struct QueueNode *bottom;
	int size;
};

void initQueue(struct Queue *q)
{
	q->top=NULL;
	q->bottom=NULL;
	q->size=0;
}

void enque(int val, struct Queue *q)
{
	struct QueueNode *temp = malloc(sizeof(struct QueueNode));
	temp->val = val;
				
	if(q->size==0)
	{
		q->top = temp;
		q->bottom = temp;
	}
	else if(q->size==1)
	{
		q->bottom->next = temp;
		q->top = temp;
	}
	else
	{
		struct QueueNode *prevTop;
		prevTop = q->top;
		prevTop->next = temp;
		q->top = temp;
	}
	q->size++;
	
	
}
int deque(struct Queue *q)
{
	int result;
	struct QueueNode *toFree;
	if(q->bottom!=NULL)
	{
		result = q->bottom->val;
		toFree = q->bottom;
		
		q->bottom = q->bottom->next;
		q->size--;
		
		free(toFree);
		return result;
	}
	else if(q->bottom==NULL)
	{
		return -1;
	}
		
}

void displayQueue(struct Queue *q)
{
	while(q->bottom!=NULL && q->size>0)
	{
		printf("%d\n",deque(q)); 
	}
}

char* receiveCommandFd(int fd)
{
	int size;
	char* result;
	char buff[100];
	while(1)
	{
		emptyArray(buff,100);
		size = read(fd,buff,99);
		
		if(strcmp(buff,"No input #!@#$^@#$^")==0)
			break;
	
		buff[strlen(buff)]='\0';		
		result=concatArg(result,buff);
		
		if(size<100)
			break;
		break;
	}
	return result;
}

void* runCommandRemote(void* pClientSocketD)
{
	int clientSocketD = *(int*)pClientSocketD;
	char toReceive[1000];
	emptyArray(toReceive,strlen(toReceive));
		
	int len = read(clientSocketD, &toReceive, 1000);
	toReceive[len]='\0';
		
	{
		int p=fork();
		if(p==0)
		{
			dup2(clientSocketD,STDOUT_FILENO);
			dup2(clientSocketD,STDERR_FILENO);
			splitExecuteRemote(toReceive);
			printf("No output #!@#$^@#$^");
			exit(0);
		}
		wait(NULL);
		
		return NULL;
	}	
}

//GLOBAL
struct Queue q;
pthread_mutex_t qMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;

void* waitForTask()
{
	while(1)
	{
		int clientSocketD;
		//Make q thread-safe
		pthread_mutex_lock(&qMutex);//------------------
		
		if((clientSocketD = deque(&q)) == -1)
		{
			pthread_cond_wait(&condition, &qMutex);
			
			clientSocketD = deque(&q);
		}
		
		pthread_mutex_unlock(&qMutex);//-----------------
		
		if(clientSocketD != -1)
		{
			runCommandRemote(&clientSocketD);
		}
		
	}
	
}

void createThreadsWait(int nrThreads)
{
	pthread_t threadPool[nrThreads];
	for(int i=0;i<nrThreads;i++)
	{
		pthread_create(&threadPool[i], NULL, waitForTask, NULL);
	}
}

int runServer(int argc, char** argv)
{	
	int socketD;
	int maxConcurentCon=0;
	
	initQueue(&q);
	
	if(argc>2)
	{
		maxConcurentCon=atoi(argv[2]);
		
		if(maxConcurentCon>2)
			createThreadsWait(maxConcurentCon);
	}
	
	socketD = socket(AF_INET,SOCK_STREAM,0);
	if(socketD==-1)
	{
		printf("Socket creation failed");
	}
	
	int port = atoi(argv[1]);
	
	struct sockaddr_in serverAdress;
	serverAdress.sin_family = AF_INET;
	serverAdress.sin_port = htons(port);
	serverAdress.sin_addr.s_addr = INADDR_ANY;
	
	int status = bind(socketD, (struct sockaddr*) &serverAdress, sizeof(serverAdress));
	if(status<0)
	{
		printf("Error: bind\n");
		return -1;
	}
	printf("Awaiting commands from clients\n");
		
	while (1)
	{
		listen(socketD, 100);
		int clientSocketD;
		clientSocketD = accept(socketD,NULL,NULL);
	
		if(clientSocketD<0)
			printf("Error: accept");
		
		if(maxConcurentCon>1)
		{	
			pthread_mutex_lock(&qMutex);
			
			enque(clientSocketD,&q); 
			pthread_cond_signal(&condition);//now threads know there is a conection
			
			pthread_mutex_unlock(&qMutex);
		}
		else if(maxConcurentCon==1)
		{
			runCommandRemote(&clientSocketD);
		}
		else if(maxConcurentCon==0)
		{
			pthread_t currentThread;
			pthread_create(&currentThread, NULL, runCommandRemote, &clientSocketD);
		}
		
	}

	
	close(socketD);
	return 0;
}

void displayFd(int fd)
{
	int size=1;
	int stop=0;
	char buff[1000];
	int sizeEnd = strlen("No output #!@#$^@#$^");
	while(size>0)
	{
		size = read(fd,buff,1000);
		if(strcmp(buff,"No output #!@#$^@#$^")==0)
		{
			break;
		}
		else
		{			
			buff[size] = '\0';
			if(strstr(buff,"No output #!@#$^@#$^"))
			{
				printf("%.*s",size-sizeEnd,buff);
				stop=1;
			}
			else
				printf("%s",buff);
		}
		if(stop)
		{
			return;
		}
	}
}

int createConRunCommand(char* serverIp, char* serverPort, char* command)
{
	int socketD;
	int port = atoi(serverPort);
	socketD = socket (AF_INET,SOCK_STREAM,0);
	if(socketD==-1)
	{
		printf("Socket creation failed");
	}
	
	struct sockaddr_in serverAdress;
	serverAdress.sin_family = AF_INET;
	serverAdress.sin_port = htons(port);
	serverAdress.sin_addr.s_addr = inet_addr(serverIp);
			
	int success = connect(socketD, (struct sockaddr*) &serverAdress, sizeof(serverAdress));
	if(success<0)
	{
		printf("Connection failed\n");
		return -1;
	}
	
	write(socketD,command,strlen(command));
		
	displayFd(socketD);
	
}

void measureRuntime(char** argv,int numberRequests, char* command)
{
	struct timeval start;
	struct timeval end;
	double sec,microSec;
			
	gettimeofday(&start,NULL);
			
	for(int i=0;i<numberRequests;i++)
	{
		sleep(1); //just to slow things down
		createConRunCommand(argv[1], argv[2], command);
	}
			
	gettimeofday(&end,NULL);
	sec = end.tv_sec - start.tv_sec;
	microSec = absolute(end.tv_usec - start.tv_usec);
	printf("Runtime : %f seconds\n\t  %f micro seconds\n",sec,microSec);
}

int runClient(int argc, char** argv)
{	
	char* prompt;
	char* command;	
	using_history();	
	while(1)
	{
		prompt = concatArg(getenv("USERNAME"),"@");
  		prompt = concatArg(prompt,argv[1]);
  		prompt = concatArg(prompt,">_\b");
  	
  		command = readline(prompt);
  		if(strcmp(command,"")!=0)
			add_history(command);
		
		if(strcmp(command,"exit")==0)
		{
			//printf("exiting\n");
			return 0;
		}
		else if(strcmp(command,"runtime")==0)
		{
			measureRuntime(argv,10, "ls | tac | grep a > temp");
		}
		else
		{
			createConRunCommand(argv[1], argv[2], command);
		}		
	}
	return 0;
}

#endif
