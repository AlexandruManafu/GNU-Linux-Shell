#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <fcntl.h>

#include <readline/readline.h>
#include <readline/history.h>

#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <pthread.h>

char* copyArgument(char* source)
{
	if(source==NULL)
		return NULL;
	int len = strlen(source)+1;
	char* result = malloc(sizeof(char)*len);
	strcpy(result,source);
	result[len]='\0';
	
	return result;
}

char* concatArg(char* string1, char* string2)
{
	char* result = NULL;
	int len = 0;
	
	if(string1)
		len+=strlen(string1);
	if(string2)
		len+=strlen(string2);
	if((string1 || string2) && ((result = malloc(len+1)) !=NULL))
	{
		*result = '\0';
		if(string1)
			strcpy(result,string1);
			
		if(string2)
		{
			strcat(result,string2);
		}
	}
	//printf("%s",result);
	return result;
}

char* addLetter(char* string, char letter)
{
	if(string==NULL)
	{
		char* result=malloc(2);
		result[0] = letter;
		result[1] = '\0';
	}
	int len = strlen(string);
	len++;
	
	char* result = copyArgument(string);
	result[len-1]=letter;
	result[len]='\0';
	
	return result;
}

char* removeLastChars(char *string, int howMany)
{
	int i=0,len = strlen(string);
	if(len <= howMany || string==NULL)
	{
		return string;
	}
	char* result = malloc(len + 1 - howMany);
	for(i=0;i<len-howMany;i++)
	{
		result[i] = string[i];
	}
	result[i] = '\0';
	return result;
}

char* removeTrailingComponent(char* str, char letter)
{
	int i,len = strlen(str);
	char* result;
	int containsLetter=0;
	for(i=len-1;i>=0;i--)
	{
		if(str[i]==letter)
		{
			containsLetter=1;
			str[i]='\0';
			break;
		}
	}
	if(containsLetter)
	{
		result = copyArgument(str);
		return result;
	}
	else
		return ".";
	
}

int dirnameImp(int argc,char** argv)
{
	char* temp;
	for(int i=1;i<argc;i++)
	{
		temp = removeTrailingComponent(argv[i],'/');
		printf("%s\n",temp);
	}
}

void copyArr(int len,char ** in, char** result)
{
	for(int i=0;i<len;i++)
	{
		result[i] = copyArgument(in[i]);
	}
}

int fileExists(char* path)
{
    struct stat fileInfo;

    if (stat(path, &fileInfo) == 0)
    {
        return 1;
    } 
    else 
    {
        return 0;
    }
}

void emptyArray(char* arr, int len)
{
	for(int i=0;i<len;i++)
	{
		arr[i]='\0';
	}
}

void displayHelp()
{
	printf(
	"\nAside from the standard commands, the following commands and options were implemented from scratch:\n\n lsC [Options] [Directories] \n Display information about the Files, if there is no argument use the current directory instead.\n The output is sorted by file-name (ignoring cases) by default.\n\n The options for lsC are:\n -l display in a long listing format, in this format the following information is shown:\n  permissions (USR,GRP,OTH), number of hard links, owner, group, file-size, last modified, file-name \n\n -s display a total allocated size (in blocks) and an allocated size for each file \n -a also display information about the current directory and the parrent directory \n -F display the file type right after the name ( / directory, * executable, @ symbolic link, | FIFO or pipe, = socket ) \n\n tacC [Options][Files] \n Display the files in reverse, with no files as arguments or with \"-\" as argument read from standard input.\n The options for tac are:\n -b display the separator (default is newline) before the lines\n -s set the separator\n\n dirnameC [Arguments]\n Given some strings as arguments strip all contents after the last \"/\" and display the modified strings.\n If there is no such character print \".\"\n\n server [Port][maxCon] starts a server that will send back the output of commands sent by clients.\n The argument maxCon (default 0) is the maximum number of concurrent connections. \n\n client[IP][Port] opens another shell in which commands will be sent to the server at IP:Port. \n\n exit exists the current shell.\n\nhelp\n Display this page.\n\n version \n Display release date and author info.   \n\n"
	);
}

void displayVersion()
{
	printf(
	"C shell interpreter 11.01.21 Release \nIn this version the piping \"|\" and redirection \">\" \"<\" operators should work with both custom commands and standard commands.\n+Added client server functionality \nFeel free to change and redistribute.\n\nWritten by Alexandru Manafu\nUVT Computer Science in English. \n"
	);
}

float absolute(float val)
{
	if(val<0)
		return val*-1;
	return val;
}

#endif
