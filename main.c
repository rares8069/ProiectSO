#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>


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
	  //printf("Directory: %s\n", path);
	  traverseDirectory(path);
	}
      } else {

	char name[100];
	strcpy(name, "snapshot_");
	strcat(name, entry->d_name);
	int result=strncmp(entry->d_name,"snapshot",5);
	
	
	if(result){
	  char baseFilePath[100];
	  strcpy(baseFilePath,basePath);
	  strcat(baseFilePath,"/");
	  strcat(baseFilePath,name);
	  strcat(baseFilePath,".txt");
	  
	  int f=open(baseFilePath, O_WRONLY | O_APPEND | O_CREAT , S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

	   if (f == -1) {
	     perror("Error opening file");
	     exit(-1);}

	   write(f, "New_Snapshot", 12);
	   write(f,"\ndimensiune ",12);
	   char fileSizeStr[20];
	   sprintf(fileSizeStr, "%lld", (long long)statbuf.st_size);
	   write(f,fileSizeStr,strlen(fileSizeStr));
	   write(f," Bytes\n",7);
	   char dataModifcare[50];
	   strcpy(dataModifcare,"Data ultimei modificari:");
	   write(f,dataModifcare,strlen(dataModifcare));
	   struct timespec modTime = statbuf.st_mtim;
	   char modTimeStr[20];
	   strftime(modTimeStr, sizeof(modTimeStr), "%Y-%m-%d %H:%M:%S", localtime(&modTime.tv_sec));
	   write(f,modTimeStr,strlen(modTimeStr));
	   write(f,"\n",1);
	   close(f);

	  }
	//printf("File: %s\n", entry->d_name);
	//printf("cale: %s\n",basePath);

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
	  //printf("Directory: %s\n", path);
	  traverseDirectoryOutput(path,output);
	}
      } else {

	char name[100];
	strcpy(name, "snapshot_");
	strcat(name, entry->d_name);
	int result=strncmp(entry->d_name,"snapshot",5);
	
	
	if(result){
	  char baseFilePath[100];
	  strcpy(baseFilePath,output);
	  strcat(baseFilePath,"/");
	  strcat(baseFilePath,name);
	  strcat(baseFilePath,".txt");
	  chdir(output);
	  
	  int f=open(baseFilePath, O_WRONLY | O_APPEND | O_CREAT , S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

	   if (f == -1) {
	     perror("Error opening file");
	     exit(-1);}

	   write(f, "New_Snapshot", 12);
	   write(f,"\ndimensiune ",12);
	   char fileSizeStr[20];
	   sprintf(fileSizeStr, "%lld", (long long)statbuf.st_size);
	   write(f,fileSizeStr,strlen(fileSizeStr));
	   write(f," Bytes\n",7);
	   char dataModifcare[50];
	   strcpy(dataModifcare,"Data ultimei modificari:");
	   write(f,dataModifcare,strlen(dataModifcare));
	   struct timespec modTime = statbuf.st_mtim;
	   char modTimeStr[20];
	   strftime(modTimeStr, sizeof(modTimeStr), "%Y-%m-%d %H:%M:%S", localtime(&modTime.tv_sec));
	   write(f,modTimeStr,strlen(modTimeStr));
	   write(f,"\n",1);
	   close(f);

	  }
	//printf("File: %s\n", entry->d_name);
	//printf("cale: %s\n",basePath);

        }
    }

    closedir(dir);
}


int main(int argc, char *argv[]) {
  
  
  #define MAX argc

  if(argc>1)
    {
      if(strcmp(argv[1],"-o")!=0)
	{
       
	
	  int i;
	  
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
      else{
	 int i;
	  
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
    
    }
  
  
  
  
    return 0;
}
