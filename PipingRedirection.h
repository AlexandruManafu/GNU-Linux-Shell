#ifndef PIPING_REDIRECTION_H
#define PIPING_REDIRECTION_H

#include "LsImplementation.h"
#include "TacImplementation.h"


int displayStdin()
{
	char temp[1000];
	while(fgets(temp,1000,stdin)!=NULL)
	{
		printf("%s",temp);
		
	}
	return 0;
}

int displayStderr()
{
	char temp[1000];
	while(fgets(temp,1000,stderr)!=NULL)
	{
		printf("%s",temp);
		
	}
	return 0;
}

int writeStdin(char* fileName)
{
	char temp[1000];
	FILE* in;

	in = fopen(fileName,"w");
	if(in==NULL)
	{
		fprintf(stderr,"Error: File cannot be opened\n");
		return -1;
	}
	
	while(fgets(temp,1000,stdin)!=NULL)
	{
		fprintf(in,"%s",temp);
	}
	fclose(in);
	
	return 0;
}

int splitCommand(char* commands, char** result)
{
  char delimit[] = "|><"; // split on | or < or > then whitespace will be ignored
  char* copy = copyArgument(commands);
  char* com = strtok(copy,delimit);
  int i=0;
  while(com!=NULL)
  {
    result[i] = copyArgument(com);
    com = strtok(NULL,delimit);
    i++;
  }
  return i;
  //where i is the length
}
char* getOperators(char* commands)
{
	char current,prev;
	char* result = "";
	for(int i=1;i<strlen(commands);i++)
	{
		if(commands[i]==commands[i-1] &&
		  ( commands[i]=='|' || commands[i]=='<' || commands[i]=='>' ) )
		  {	
			continue;
		  }
			
		if(commands[i]=='|' || commands[i]=='<' || commands[i]=='>')
		{
			current = commands[i];
			//printf("%c\n",current);
			result = addLetter(result,current);
		}
		
	}
	return result;
}

int parseCom(char * com, char** result)
{
	char* start = com;
	int insideQuotes=0;
	while(*com!='\0')
	{
	//printf("%c",*com);
		if(*com==' ' && insideQuotes==0)
		{
			*com='\t';
		}
		else if(insideQuotes==0 &&(*com=='\"' || *com=='\''))
		{
			insideQuotes=1;
		}
		else if(insideQuotes==1 &&(*com=='\"' || *com=='\''))
		{
			insideQuotes=0;
		}
		com++;
	}
	//printf("%s\n",start);
	com = copyArgument(start);
	char delimit[] = "\t\"'";
  	com = strtok(com,delimit);
  	int i=0;
  	while(com!=NULL)
  	{
  		//printf("%s\n",com);
    		result[i] = copyArgument(com);
    		com = strtok(NULL,delimit);
    		i++;
    		//printf("%d",i);
  	}
  	result[i]=0;
  	return i;
  	//where i is the length
}

int forkExecute(char** com, int pipeWriteEnd)
{
  int pid;

  pid=fork();
  
  if(pid<0)
  {
  	perror("Fork failed");
  }

  if(pid==0)//in child
  {
  	if(pipeWriteEnd) //if 0 it will not redirect output
  	{
  		dup2(pipeWriteEnd, STDOUT_FILENO);
  	}
  	execvp(com[0],com);
  	perror("exec");
  	abort();

 
  }
  else
  {
	wait(NULL);
  }

}

void forkExecuteCustom(int lenCurrentCom,char** currentCom, int pipeWriteEnd)
{
  int pid;
  pid=fork();
  
  if(pid<0)
  {
  	perror("Fork failed");
  }
  if(pid==0)//in child
  {
  	if(pipeWriteEnd) //if 0 it will not redirect output
  		dup2(pipeWriteEnd, STDOUT_FILENO);
  		
  	if(strcmp(currentCom[0],"lsC")==0)
	{
		lsParse(lenCurrentCom,currentCom);
		exit(0);
	}
	else if(strcmp(currentCom[0],"tacC")==0)
	{
		tacParse(lenCurrentCom,currentCom);
		exit(0);
	}
	else if(strcmp(currentCom[0],"dirnameC")==0)
	{
		dirnameImp(lenCurrentCom,currentCom);
		exit(0);
	}
	else if(strcmp(currentCom[0],"help")==0)
	{
		displayHelp();
		exit(0);
	}
	else if(strcmp(currentCom[0],"version")==0)
	{
		displayVersion();
		exit(0);
	}
	else if(strcmp(currentCom[0],"exit")==0)
	{
		exit(0);
	}
  }
  else
  {
  	wait(NULL);
  }
}

int isImplementedCommand(char **currentCom)
{
	char* implementedCom[10];	
	implementedCom[0] = copyArgument("lsC");
	implementedCom[1] = copyArgument("tacC");
	implementedCom[2] = copyArgument("dirnameC");
	implementedCom[3] = copyArgument("help");
	implementedCom[4] = copyArgument("version");
	implementedCom[5] = copyArgument("exit");
  
	for(int i=0;i<6;i++)
	{
		if(strcmp(currentCom[0],implementedCom[i])==0)
		{
			return 1;
		}
	}
	return 0;
}

void fileToStdinExecute(char* file, int lenCom, char** command, char currentOperator, int pipeWriteEnd)
{
	int fileD = open(file,O_RDONLY);
	dup2(fileD, STDIN_FILENO);
	
	if(currentOperator=='\0')
	{
		if(isImplementedCommand(command))
			forkExecuteCustom(lenCom,command,0);
		else
			forkExecute(command,0);
	}
	else
       {
		if(isImplementedCommand(command))
			forkExecuteCustom(lenCom,command,pipeWriteEnd);
		else
			forkExecute(command,pipeWriteEnd);
		close(pipeWriteEnd);
	}
}


void splitExecuteRemote(char* input)
{  
  int i, lenCurrentCom, lenPrevCom, res;

  char* commands[1000];
  char* currentCom[1000];
  char* prevCom[1000];
  char* operators = getOperators(input);
  char currentOperator, prevOperator;

  int nrComs = splitCommand(input,commands);

  if(nrComs>1)
  {  	
	  for(i=0;i<nrComs;i++)
	  {
	  	int fd[2];
		if(pipe(fd)<0)
		{
			perror("Error: pipe failed)");
			abort();
		}
		
	  	lenCurrentCom = parseCom(commands[i],currentCom);
	  	
	  	if( i < nrComs - 1 )
	  		currentOperator=operators[i];
	  		
	  	if(prevOperator=='>')
	  	{
	  		res = writeStdin(currentCom[0]);//write stdin to file
	  		if(res == -1)
	  			break;
	  		i++;
	  		continue;
	  	}
	  	else if(prevOperator=='<' && fileExists(currentCom[0]))
	  	{
	  		fileToStdinExecute(currentCom[0], lenPrevCom, prevCom, operators[i], fd[1]);
	  		//writeStdin(currentCom[0]);
	  	}
	  	else if(prevOperator=='<' && !fileExists(currentCom[0]))
	  	{
	  		fprintf(stderr,"cannot access '%s': No such file or directory\n",currentCom[0]);
	  	}
	  		  	  
		if(currentOperator=='|' || currentOperator=='>')
		{	
			if(isImplementedCommand(currentCom))
			 	forkExecuteCustom(lenCurrentCom,currentCom,fd[1]);
			else if(currentOperator == '>' && prevOperator == '<')
			{
			
			}
			else
		  	 	forkExecute(currentCom,fd[1]);
		  	
	  	  
			// Output from the previous becomes input of current
			dup2(fd[0], STDIN_FILENO);
			close(fd[1]);
		}
		else if(currentOperator=='<')
		{
			copyArr(lenCurrentCom,currentCom,prevCom);
			lenPrevCom = lenCurrentCom;
		}
		
		prevOperator = currentOperator;
		
	  }
	  //printf("%c",currentOperator);
	  if(currentOperator=='|')
	  {
		if(isImplementedCommand(currentCom))
		{
		  		displayStdin();
		}
		else
	  		displayStdin(); 
  	  }
  	  else if(currentOperator=='<' || currentOperator=='>' )
  	  {
  	  	displayStderr();
  	  }
  } 
  else
  {	
  	lenCurrentCom = parseCom(commands[i],currentCom);
  	
	if(isImplementedCommand(currentCom))
  	{
  		forkExecuteCustom(lenCurrentCom,currentCom,0);
  	}
	else
	  	forkExecute(currentCom,0);
  }

}

#include "Server.h"

void executeCustom(int lenCurrentCom, char** currentCom)
{
	if(strcmp(currentCom[0],"client")==0)
	{
		if(currentCom[1] == NULL || currentCom[2] == NULL)
		{
			printf("Error: missing arguments (ip or port) for client\n");
			return;
		}
		runClient(lenCurrentCom,currentCom);
	}
	else if(strcmp(currentCom[0],"server")==0)
	{
		if(currentCom[1] == NULL)
		{
			printf("Error: missing argument (port) for server\n");
			return;
		}
		runServer(lenCurrentCom,currentCom);
	}
	else if(isImplementedCommand(currentCom))
  	{
  		forkExecuteCustom(lenCurrentCom,currentCom,0);
  	}
	else
	  	forkExecute(currentCom,0);
}

void splitExecute(char* input)
{  
  int i, lenCurrentCom, lenPrevCom, res;

  char* commands[1000];
  char* currentCom[1000];
  char* prevCom[1000];
  char* operators = getOperators(input);
  char currentOperator, prevOperator;

  int nrComs = splitCommand(input,commands);

  if(nrComs>1)
  {  	
	  for(i=0;i<nrComs;i++)
	  {
	  	int fd[2];
		if(pipe(fd)<0)
		{
			perror("Error: pipe failed)");
			abort();
		}
		
	  	lenCurrentCom = parseCom(commands[i],currentCom);
	  	
	  	if( i < nrComs - 1 )
	  		currentOperator=operators[i];
	  		
	  	if(prevOperator=='>')
	  	{
	  		res = writeStdin(currentCom[0]);//write stdin to file
	  		if(res == -1)
	  			break;
	  		i++;
	  		continue;
	  	}
	  	else if(prevOperator=='<' && fileExists(currentCom[0]))
	  	{
	  		fileToStdinExecute(currentCom[0], lenPrevCom, prevCom, operators[i], fd[1]);
	  		//writeStdin(currentCom[0]);
	  	}
	  	else if(prevOperator=='<' && !fileExists(currentCom[0]))
	  	{
	  		fprintf(stderr,"cannot access '%s': No such file or directory\n",currentCom[0]);
	  	}
	  		  	  
		if(currentOperator=='|' || currentOperator=='>')
		{	
			if(isImplementedCommand(currentCom))
			 	forkExecuteCustom(lenCurrentCom,currentCom,fd[1]);
			else if(currentOperator == '>' && prevOperator == '<')
			{
			
			}
			else
		  	 	forkExecute(currentCom,fd[1]);
		  	
	  	  
			// Output from the previous becomes input of current
			dup2(fd[0], STDIN_FILENO);
			close(fd[1]);
		}
		else if(currentOperator=='<')
		{
			copyArr(lenCurrentCom,currentCom,prevCom);
			lenPrevCom = lenCurrentCom;
		}
		
		prevOperator = currentOperator;
		
	  }
	  //printf("%c",currentOperator);
	  if(currentOperator=='|')
	  {
		if(isImplementedCommand(currentCom))
		{
		  	displayStdin();
		}
		else
	  		displayStdin(); 
  	  }
  	  else if(currentOperator=='<' || currentOperator=='>' )
  	  {
  	  	displayStderr();
  	  }
  } 
  else
  {	
  	lenCurrentCom = parseCom(commands[i],currentCom);
  	
  	executeCustom(lenCurrentCom, currentCom);
  }

}

#endif
