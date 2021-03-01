#ifndef LS_IMP_H
#define LS_IMP_H
#include "HelperFunctions.h"
char* getUser(uid_t uid)
{
    struct passwd *pws;
    pws = getpwuid(uid);
        return pws->pw_name;
}

char* getGroup(gid_t gid)
{
    struct group *pws;
    pws = getgrgid(gid);
        return pws->gr_name;
}

char* convertTime(time_t rawTime)
{
	struct tm *info;
	//time(&rawTime);
	info = localtime( &rawTime );
	char* result = asctime(info);
	
	int lenRes = strlen(result);
	//result[lenRes-1] = 0;
	result = removeLastChars(result,9);
	//printf("%s\n",result);
	return result;
}

long getFileSize(char* fileName)
{
    struct stat fileStat;
    
    if(lstat(fileName,&fileStat) < 0)    
     {
     	printf("error accessing the file\n");
        return -1;
     }
     
     return fileStat.st_blocks/2;
}

int getNumberFiles(char* path, int a)
{
    struct dirent *dp;
    DIR *dir = opendir(".");
    int result=0;
    
    while ((dp = readdir(dir)) != NULL)
    {
    	if(a==0 && dp->d_name[0]=='.')
    	{
    		continue;
    	}
    	result++;   
    }
    
    return result;
}

char typeFile(char* fileName) //for each type of file associate a char
{
	struct stat fileInfo;
	if(stat(fileName,&fileInfo)<0)
		return 0;
		
	if(S_ISDIR(fileInfo.st_mode))
	{
		return '/';
	}
	else if(fileInfo.st_mode & S_IEXEC)
	{
		return '*';
	}
	else if(S_ISREG(fileInfo.st_mode))
	{
		return 0;
	}
	else if(S_ISLNK(fileInfo.st_mode))
	{
		return '@';
	}
	else if(S_ISFIFO(fileInfo.st_mode))
	{
		return '|';
	}
	else if(S_ISSOCK(fileInfo.st_mode))
	{
		return '=';
	}
	
	return 0;	
}

void displayShortLs(char** fileNames,long fileSizes[], int sumFileSizes, int numberFiles, int F)
{
	if(sumFileSizes)
	{
		printf("total %d\n",sumFileSizes);
	}
	for(int i=0;i<numberFiles;i++)
	{
		if(sumFileSizes)
		{
			printf("%ld  ",fileSizes[i]);
		}
		if(F)
		{
			printf("%s",fileNames[i]);
			printf("%c   ",typeFile(fileNames[i]));
		}
		else
		{
			printf("%s\n",fileNames[i]);	
		}
	}
	//printf("\n");
	
}

void displayInformation (char *file, long fileSize, int F)
{
    struct stat fileStat;
    
    if(stat(file,&fileStat) < 0)    
     {
     	printf("error accessing the file %s\n",file);
        //return;
     }
     
    //file type
    if(fileSize)
    {
    	printf("%ld ",fileSize);
    }
    printf( (S_ISDIR(fileStat.st_mode)) ? "d" : (S_ISLNK(fileStat.st_mode)) ? "l" : "-");
    //permissions
    printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
    //number of hard links
    printf(" %ld",fileStat.st_nlink);
    //owner
    printf(" %s",getUser(fileStat.st_uid));
    //group
    printf(" %s",getGroup(fileStat.st_gid));
  
    printf(" %ld",fileStat.st_size);
    //last modified
    printf(" %s",convertTime(fileStat.st_mtime));
    //filename
    if(!F)
    	printf(" %s\n",file);
    else
    {
    	printf(" %s%c\n",file,typeFile(file));		
    }
}

void displayLongLs(char** fileNames,long fileSizes[], int sumFileSizes, int numberFiles, int F)
{
	if(sumFileSizes)
	{
		printf("total %d\n",sumFileSizes);
	}
	for(int i=0;i<numberFiles;i++)
	{
		if(sumFileSizes)
			displayInformation(fileNames[i],fileSizes[i],F);
		else
			displayInformation(fileNames[i],0,F);
	}
	
}



void sortFileNames(char** fileNames, long fileSizes[], int numberFiles) //sort file-names, at each swap also swap fileSizes
{
    int j;
    char* key;
    long temp;
    for (int i = 1; i < numberFiles; i++) 
    {  
        key = fileNames[i];
        if(fileSizes!=NULL)
        {
        	temp = fileSizes[i]; 
        }
        j = i - 1;  
  
        while (j >= 0 && strcasecmp(fileNames[j],key)>0 ) //arr[j] > key
        {  
            //printf("%s\n",fileNames[j+1]);
            fileNames[j + 1] = fileNames[j];
            if(fileSizes!=NULL)
            {
            	fileSizes[j + 1] = fileSizes[j];  
            }
            j = j - 1;  
        }  
        fileNames[j + 1] = key;  
        if(fileSizes!=NULL)
        {
        	fileSizes[j + 1] = temp;
        }
   }
}

void getFileNames(char* path, int a, char** fileNames)
{
    int i=0;
    struct dirent *dp;
    DIR *dir = opendir(path);
    
    if (!dir) 
        return;
    
    while ((dp = readdir(dir)) != NULL)
    {
    	if(a==0 && dp->d_name[0]=='.')
    	{
    		continue;
    	}
	fileNames[i] =copyArgument(dp->d_name);
	i++;
        
    }
    closedir(dir);
    
}

long getFileSizes(char** fileNames, int numberFiles, long fileSizes[])
{
	long fileSize=0;
	long total=0;
	for(int i=0;i<numberFiles;i++)
    	{
    		fileSize = getFileSize(fileNames[i]);
    		total+=fileSize;
    		fileSizes[i] = fileSize; 
    	}
    	
    	return total;
}
void lsImplementation(char* path,int l,int s, int a, int F)
{
    long fileSize=0;
    long sumFileSizes=0;
    int numberFiles,i=0;
        
    numberFiles = getNumberFiles(path,a);
    
    char* fileNames[numberFiles];
    long fileSizes[numberFiles];
    
    getFileNames(path,a,fileNames);
    
    //printf("%d %d %d %d\n",l,s,a,F);
    
    if(s)
    {
	sumFileSizes = getFileSizes(fileNames, numberFiles, fileSizes);
	sortFileNames(fileNames,fileSizes,numberFiles);
    }
    else
    {
    	sortFileNames(fileNames,NULL,numberFiles);
    }
    
    if(!l && s)	
	displayShortLs(fileNames,fileSizes,sumFileSizes,numberFiles,F);

	    	
    else if(!l && !s)
    	displayShortLs(fileNames,NULL,0,numberFiles,F);
    

    else if(l && s)
    	displayLongLs(fileNames,fileSizes,sumFileSizes,numberFiles,F);
    	
    else if(l && !s)
    {
	displayLongLs(fileNames,NULL,0,numberFiles,F);
    }
    
   	
}

int isDirectory(char* path)
{
    struct stat fileInfo;

    if (stat(path, &fileInfo) == 0 && S_ISDIR(fileInfo.st_mode))
    {
        return 1;
    } 
    else 
    {
        return 0;
    }
}

int lsParse(int argc,char** argv)
{
  int l=0,s=0,a=0,F=0;
  char option;
  //printf("in func\n");
  while ((option = getopt (argc, argv, "lsaF")) != -1)
  {
  	switch (option)
  	{
  	  case 'l':
  	  {
  		l=1;
  		break;
  	  }
  	  case 's':
  	  {
  	  	s=1;
  	  	break;
  	  }
  	  case 'a':
  	  {
  	  	a=1;
  	  	break;
  	  }
  	  case 'F':
  	  {
  	  	F=1;
  	  	break;
  	  }
  	  case '?':
  	  {
   		//printf("Unkown option\n");
   		return -1;
   	  }
   	  default:
   	  {
   	  	return -1;
   	  }

  	}
  }
  //printf("%d %d %d %d\n",l,s,a,F);

  if(argv[optind]==NULL)
  {
  	lsImplementation(".",l,s,a,F);
  }
  else
  {
  	for(int i=optind;i<argc;i++)
  	{
	  	if(isDirectory(argv[i]))
	  	{
	  		printf("%s:\n",argv[i]);
	  		lsImplementation(argv[i],l,s,a,F);
	  	}
	  	else
	  	{
	  		fprintf(stderr,"ls: cannot access '%s': No such file or directory\n",argv[optind]);
	  		return -1;
	  	}
  	}
  }
  return 0;	
}
#endif
