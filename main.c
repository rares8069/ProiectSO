#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

char *outputMalware=NULL;


int malware(struct dirent *entry,const char *basePath){

  if(outputMalware==NULL)
    return 0;

  char *script_path = "/home/tigan/Desktop/ProiectSistemeDeOperare/malware.sh";
  char *arguments = entry->d_name;
  char command[250];
  snprintf(command, sizeof(command), "sudo %s %s/%s %s", script_path, basePath, arguments,outputMalware);
  int script_exit_status=system(command);
  //printf("%d\n\n",script_exit_status);
  return script_exit_status;
}

void create(const char *basePath, const struct stat *statbuf, const char *name,const  DIR *dir,const char *output) {
    char baseFilePath[100];
    if(output!=NULL)
      strcpy(baseFilePath, output);
    else
      strcpy(baseFilePath, basePath);
    strcat(baseFilePath, "/");
    strcat(baseFilePath, name);
    strcat(baseFilePath, ".txt");
    int f;
    if(output!=NULL){
    
      f = open(baseFilePath, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);}
    else{
      f = open(baseFilePath, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    }
    
    if (f == -1) {
        perror("Error opening file");
        exit(-1);
	}

    write(f, "New_Snapshot data si ora:", 25);
    time_t current_time;
    struct tm *timeinfo;
    char time_str[80];
    time(&current_time);
    timeinfo = localtime(&current_time);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);
    write(f, time_str, strlen(time_str));
    
    write(f,"\ndimensiune:",12);
    char fileSizeStr[20];
    sprintf(fileSizeStr, "%lld", (long long)statbuf->st_size);
    write(f,fileSizeStr,strlen(fileSizeStr));
    write(f," Bytes\n",7);
    char dataModifcare[50];
    strcpy(dataModifcare,"Data ultimei modificari:");
    write(f,dataModifcare,strlen(dataModifcare));
    struct timespec modTime = statbuf->st_mtim;
    char modTimeStr[20];
    strftime(modTimeStr, sizeof(modTimeStr), "%Y-%m-%d %H:%M:%S", localtime(&modTime.tv_sec));
    write(f,modTimeStr,strlen(modTimeStr));
    write(f,"\n",1);

    char inode_str[70]; 
    snprintf(inode_str, sizeof(inode_str)+12, "Numar Inode:%lu\n", (unsigned long)statbuf->st_ino);
    
    
    write(f, inode_str, strlen(inode_str));
    char permissions[21];
    snprintf(permissions, sizeof(permissions), "Permisiuni:%c%c%c%c%c%c%c%c%c",
    (statbuf->st_mode & S_IRUSR) ? 'r' : '-',
    (statbuf->st_mode & S_IWUSR) ? 'w' : '-',
    (statbuf->st_mode & S_IXUSR) ? 'x' : '-',
    (statbuf->st_mode & S_IRGRP) ? 'r' : '-',
    (statbuf->st_mode & S_IWGRP) ? 'w' : '-',
    (statbuf->st_mode & S_IXGRP) ? 'x' : '-',
    (statbuf->st_mode & S_IROTH) ? 'r' : '-',
    (statbuf->st_mode & S_IWOTH) ? 'w' : '-',
    (statbuf->st_mode & S_IXOTH) ? 'x' : '-');
    write(f, permissions, sizeof(permissions)-1);
    write(f,"\n",1);
    close(f);
    //closedir(dir);
}

void traverseDirectory(const char *basePath) {
    char path[1000];
    struct dirent *entry;
    struct stat statbuf;

    DIR *dir = opendir(basePath);

    if (!dir) {
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        sprintf(path, "%s/%s", basePath, entry->d_name);
        stat(path, &statbuf);

        if (S_ISDIR(statbuf.st_mode)) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                traverseDirectory(path);
            }
        } else {


	  int corupt=0;
	  if (!(statbuf.st_mode & S_IRUSR)) {
	    if (!(statbuf.st_mode & S_IWUSR))
	      if (!(statbuf.st_mode & S_IXUSR)&&outputMalware!=NULL)
		//fd[0]-read fd[1]=write
	    {
	     
	      int fd[2];
	      pipe(fd);
	      int nr=fork();
	      
	      if(nr==0)
		{
		  close(fd[0]);
		  corupt=malware(entry,basePath);
		  write(fd[1],&corupt,sizeof(int));
		  exit(EXIT_SUCCESS);
		}
	      if(nr!=0)
		{
		  close(fd[1]);
		  read(fd[0],&corupt,sizeof(int));
		  close(fd[0]);
		  if(corupt==0)
		    printf("SAFE:%s\n",entry->d_name);
		  else{
		    char string1[100],string2[100];
		    strcpy(string1,outputMalware);
		    strcat(string1,"/");
		    strcat(string1,entry->d_name);
		    strcpy(string2,basePath);
		    strcat(string2,"/");
		    strcat(string2,entry->d_name);
		    printf("Corupt:%s\n",entry->d_name);
		    rename(string2, string1);
		  }
	
		}
	    }
	  }
        
	  wait(NULL);
        
      
	  if(corupt!=0){
	  
	    continue;
	  }
	 

	  
            char name[100];
            strcpy(name, "snapshot_");
            strcat(name, entry->d_name);



            int result = strncmp(entry->d_name, "snapshot", 5);

            if (result) {
	      create(basePath, &statbuf, name,dir,NULL);
            }
        }
    }

    closedir(dir);
}


void traverseDirectoryOutput(const char *basePath,const char *output) {
    char path[1000];
    struct dirent *entry;
    struct stat statbuf;

    DIR *dir = opendir(basePath);

    if (!dir) {
        return;
    }
    
    while ((entry = readdir(dir)) != NULL) {
      sprintf(path, "%s/%s", basePath, entry->d_name);
      stat(path, &statbuf);
      
      if (S_ISDIR(statbuf.st_mode)) {
	if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
	 
	  traverseDirectoryOutput(path,output);
	}
      } else {


        

	int corupt=0;
	if (!(statbuf.st_mode & S_IRUSR)) {
	  if (!(statbuf.st_mode & S_IWUSR))
	    if (!(statbuf.st_mode & S_IXUSR))
	      //fd[0]-read fd[1]=write
	      {
		
		int fd[2];
		pipe(fd);
		int nr=fork();
		
		if(nr==0)
		  {
		    close(fd[0]);
		    corupt=malware(entry,basePath);
		    write(fd[1],&corupt,sizeof(int));
		    exit(EXIT_SUCCESS);
		  }
		if(nr!=0)
		  {
		    close(fd[1]);
		    read(fd[0],&corupt,sizeof(int));
		    close(fd[0]);
		  if(corupt==0)
		    printf("SAFE:%s\n",entry->d_name);
		  else{
		    char string1[100],string2[100];
		    strcpy(string1,outputMalware);
		    strcat(string1,"/");
		    strcat(string1,entry->d_name);
		    strcpy(string2,basePath);
		    strcat(string2,"/");
		    strcat(string2,entry->d_name);
		    printf("Corupt:%s\n",entry->d_name);
		    rename(string2, string1);
		  }
		  
		  }
	      }
	}
        
	wait(NULL);
	
	if(corupt!=0){

	  continue;
	}
	
	
	char name[100];
	strcpy(name, "snapshot_");
	strcat(name, entry->d_name);
	int result=strncmp(entry->d_name,"snapshot",5);
	
	
	if(result){
	  
	  
	  create(basePath, &statbuf, name,dir,output);
	}
	else{

	  //closedir(dir);
	  continue  ;}



        }
    }

    closedir(dir);
}


int main(int argc, char *argv[]) {

  pid_t wpid;
  int status = 0;
  int output=0;
  int malware=0;
  int i;
  
  #define MAX argc

  for(i=1;i<argc;i++)
    {
      if(strcmp(argv[i],"-o")==0)
	output++;
      if(strcmp(argv[i],"-s")==0){
	malware++;
	outputMalware=argv[i+1];
      }
    }
  
  if(output==0 && malware==0){
    
    for (i = 1; i < MAX; i++) {
      pid_t pid = fork();
      
      if (pid < 0) {
	
	perror("fork");
	exit(EXIT_FAILURE);
      } else if (pid == 0) {
	
	printf("Procesul %d a fost executat cu succes pentru folderul cu path-ul:%s\n",getpid(),argv[i]);
	traverseDirectory(argv[i]);
	exit(EXIT_SUCCESS); 
      }
      
      
    }
  }

  if(output==1 && malware ==0){
    for (i = 3; i < MAX; i++) {
      pid_t pid = fork();
      
      if (pid < 0) {
	
	perror("fork");
	exit(EXIT_FAILURE);
      } else if (pid == 0) {
	
	printf("Procesul %d a fost executat cu succes pentru folderul cu path-ul:%s\n",getpid(),argv[i]);
	traverseDirectoryOutput(argv[i],argv[2]);
	exit(EXIT_SUCCESS); 
      }
    }
  }

  if(output==1 && malware ==1){
    for (i = 5; i < MAX; i++) {
      pid_t pid = fork();
      
      if (pid < 0) {
	
	perror("fork");
	exit(EXIT_FAILURE);
      } else if (pid == 0) {
	
	printf("Procesul %d a fost executat cu succes pentru folderul cu path-ul:%s\n",getpid(),argv[i]);
	traverseDirectoryOutput(argv[i],argv[2]);
	exit(EXIT_SUCCESS); 
      }
    }
    
  }

  
  if(output==0 && malware==1)
    {
      
      for (i = 3; i < MAX; i++) {
	pid_t pid = fork();
	
	if (pid < 0) {
	  
	  perror("fork");
	  exit(EXIT_FAILURE);
	} else if (pid == 0) {
	  
	  printf("Procesul %d a fost executat cu succes pentru folderul cu path-ul:%s\n",getpid(),argv[i]);
	  traverseDirectory(argv[i]);
	  exit(EXIT_SUCCESS); 
	}
      }
      
    }
      
  

  while ((wpid = wait(&status)) > 0);

  printf("Programul a fost executat cu succes\n");
  return 0;
}
