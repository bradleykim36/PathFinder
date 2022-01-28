/*test machine: CSEL-KH4250-26
* group number: G31
* name: Brendan Lake , Bradley Kim, Jacob Jung
* x500: lake0112 , kim00544, jungx505 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"

int main(int argc, char** argv){

	if(argc != 3){ //input check
		 
		fprintf(stderr,"Usage ./a.out [Path to Directory] [Pattern to search] \n");
		exit(EXIT_FAILURE);
	}
	char* path = argv[1]; //file path
	char* pattern = argv[2]; //string we are looking for
	
	//Declare any other neccessary variables
	DIR * rootFolder;  //directory
  struct dirent* entry; //directory entry (each file/folder)
  struct stat* st = (struct stat*) malloc(sizeof(struct stat));

  int readFds[MAX_DIRECTORY_NUMBER];
  int numPipes = 0;
	
  int sizeOfHardLinkArray = MAX_DIRECTORY_NUMBER;
  long unsigned int* hardLinkInodes = (long unsigned int*) malloc(sizeof(long unsigned int)*sizeOfHardLinkArray);
  int numHardLinks = 0;

	//Open root directory

  if((rootFolder = opendir(path)) == NULL){
    printf("%s is empty.\n", path);
  }
	// Iterate through root dir and spawn children as neccessary
	
  while ((entry = readdir(rootFolder)) != NULL) {

    if ((strcmp(".", entry->d_name) == 0) || (strcmp("..", entry->d_name) == 0)) { // skip current and parent directories
      continue;
    }

    char entry_name[MAX_ENTRY_NAME_LENGTH] = {'\0'};
    snprintf(entry_name, sizeof(entry_name), "%s/%s", path, entry->d_name);
    lstat(entry_name, st);

    mode_t entryType = st->st_mode;

    if (S_ISREG(entryType)) { //regular file

      long unsigned int inode = st->st_ino; //assign inode

      int i;
      int setContinue = 0;
      for (i = 0; i < numHardLinks; i++) { // don't search the same inode more than once
        if (inode == hardLinkInodes[i]) {
          setContinue = 1;
          break;
        }
      }

      if (setContinue) { // go to the next entry
        continue;
      }

      if (numHardLinks == sizeOfHardLinkArray) { // resize array of inodes if necessary
        sizeOfHardLinkArray *= 2;
        hardLinkInodes = (long unsigned int*) realloc(hardLinkInodes, sizeof(long unsigned int)*sizeOfHardLinkArray);
      }

      hardLinkInodes[numHardLinks] = inode; // add inode to array of inodes
      numHardLinks++;

      searchPatternInFile(entry_name, pattern); // search regular file
    }
    else if (S_ISDIR(entryType)) { //its a directory

      int fds[2];

      int pipe_result = pipe(fds);
      if (pipe_result < 0) {
        printf("ERROR: Failed to open pipe\n");
      }

      readFds[numPipes] = fds[0];
      numPipes++;

      pid_t pid = fork();

      if (pid < 0) {
        printf("ERROR: Failed to fork\n");
      }
      else if (pid > 0) { // parent
        close(fds[1]);
      }
      else { // child

        close(fds[0]);
        dup2(fds[1], STDOUT_FILENO);
        close(fds[1]);

        // At this point, stdout is redirected to the pipe
        // Any calls to printf will be sent through the pipe to master
        // Master will print everything out at the end

        execl("child", "child", entry_name, pattern, NULL);
        printf("Error executing execl()\n");   
      }
    }
    else if (S_ISLNK(entryType)) {
  //    printf("Symbolic Link --- File: %s, Inode: %lu\n", entry->d_name, st->st_ino); 
    }
    else {
      printf("ERROR: Assumption Violation: file type not supported");
      exit(EXIT_FAILURE);
    }

  }
  closedir(rootFolder);
  free(st);
  free(hardLinkInodes);

  pid_t terminated_pid;
  while((terminated_pid = wait(NULL)) > 0); // wait for all child processes
  
  char buffer[MAX_ENTRY_NAME_LENGTH];
  FILE *file;

  // Print out all data from the pipes

  int i;
  for (i = 0; i < numPipes; i++) {
    file = fdopen(readFds[i], "r");
    while (fgets(buffer, MAX_ENTRY_NAME_LENGTH, file) != NULL) {
      printf("%s", buffer);
    }
    close(readFds[i]);
  }
	return 0;
}
