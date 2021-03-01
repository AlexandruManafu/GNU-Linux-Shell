#ifndef TAC_IMP_H
#define TAC_IMP_H
#include "HelperFunctions.h"

struct stackNode
{
	char* val;
	struct stackNode *next;
};
struct stack
{
	struct stackNode *top; //NULL by default
};

void push(char*val, struct stack *s)
{
	struct stackNode *temp = malloc(sizeof(struct stackNode));
	temp->val = copyArgument(val);
	temp->next = s->top;
	s->top = temp;
}
char* pop(struct stack *s)//only print
{
	char* result;
	struct stackNode *toFree;
	if(s->top!=NULL)
	{
		result = s->top->val;
		toFree = s->top;
		s->top = s->top->next;
		free(toFree);
	}
	return result;
}
struct stack reverseStack(struct stack *s)
{
	char *temp;
	struct stack result;
	while(s->top!=NULL)
	{
		temp = pop(s);
		push(temp,&result);
	}
	return result;
}
void displayStack(struct stack *s)
{
	while(s->top!=NULL)
	{
		printf("%s",pop(s));
	}
}

char* putSepFirst(char* input, char* separator)
{
	int len = strlen(input);
	int lenSep = strlen(separator);
	
	input[len-1]='\0';
	char* result = input;
	char* target = input + (len - lenSep-1);
	
	if(strcmp("\n",separator)==0)
	{
		input[len-lenSep]='\0';
		result = concatArg(separator,input);
	}
	else if(strcmp(target,separator)==0)
	{
		input[len-lenSep-1]='\n';
		input[len-lenSep]='\0';
		result = concatArg(separator,input);
	}
	return result;
}


int displayLinesReverse(char* path,int b,char* separator)
{
	struct stack s;
	char temp[1000];
	char* toPush;
	FILE* in;

	in = fopen(path,"r");
	if(in==NULL)
	{
		fprintf(stderr,"Error: File cannot be opened\n");
		return -1;
	}
	
	while(fgets(temp,1000,in)!=NULL)
	{
		if(b)
		{
			toPush = putSepFirst(temp, separator);
			push(toPush,&s);	
		}
		else
			push(temp,&s);
	}
	fclose(in);
	
	displayStack(&s);
	return 0;
	
}

int displayStdinReverse(int b, char* separator)
{
	
	struct stack s;
	char temp[1000];
	char* toPush;

	while(fgets(temp,1000,stdin)!=NULL)
	{
		if(b)
		{
			toPush = putSepFirst(temp, separator);
			push(toPush,&s);	
		}
		else
			push(temp,&s);
	}
	displayStack(&s);
	


	return 0;
}

int tacParse(int argc, char** argv)
{
	int b=0,s=0;
	int pid;
	char option;
	char* separator = copyArgument("\n");
	while ((option = getopt (argc, argv, "bs:")) != -1)
  	{
	  	switch (option)
	  	{
	  	    case 'b':
	  	    {
	  		b=1;
  			break;
  		    }
  		    case 's':
	  	    {
	  	    	s=1;
	  		separator = copyArgument(optarg);
  			break;
  		    }
  		    case '?':
  	  	    {
   			return -1;
   	  	    }
   	  	    default:
   	  	    {
   	  		return -1;
   	  	    }
  		}
  	}
  	
  	//printf("%d %s\n",b,separator);
  	
	for(int i = optind;i<argc;i++)
	{
		if(strcmp(argv[i],"")==0 || strcmp(argv[i],"-")==0)
	  	{
	  		displayStdinReverse(b,separator);
	  	}
	  	else 
	  	{
	  		displayLinesReverse(argv[i],b,separator);	
	  	}	
	 }
	 if(argc==1 || argv[optind] == NULL)
	 {
	 	displayStdinReverse(b,separator);
	 }
	 return 0;
}
#endif
