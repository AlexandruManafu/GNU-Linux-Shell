#include "PipingRedirection.h"


int main(int argc, char *argv[]) 
{
  char path[FILENAME_MAX];
  char *prompt;
  char *command;
  int p;
  
  using_history();
  while(1)
  {
  	getcwd(path,FILENAME_MAX); //get current working  directory
  	prompt = concatArg(getenv("USERNAME"),":");
  	prompt = concatArg(prompt,path);
  	prompt = concatArg(prompt,">_\b");
  	
  	command = readline(prompt);
  	
  	if(strcmp(command,"")!=0)
		add_history(command);
	
	//printf("%s\n",command);
	if(strcmp(command,"exit")==0)
		break;
		  
	p=fork();
	if(p<0)
	{
		perror("Fork failed");
		abort();
	}
	else if(p==0)
	{
		splitExecute(command);
		exit(0);
		
	}
	wait(NULL);
	
  }
  return 0;
}
