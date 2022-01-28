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

/*
	Traverse the directory recursively and search for pattern in files.
	@params:
		name - path to the directory
		pattern - pattern to be recusrively searched in the directory
		
	Note: Feel free to modify the function header if neccessary
	
*/

int sizeOfHardLinkArray = MAX_DIRECTORY_NUMBER;
long unsigned int* hardLinkInodes; //stores all inodes of regular files for this process
int numHardLinks = 0;

void dirTraverse(char *name, char * pattern)
{
  DIR * subDirectory;
  struct dirent * entry;
  struct stat* st = (struct stat*) malloc(sizeof(struct stat));

  if((subDirectory = opendir(name)) == NULL){
    printf("%s is empty.\n", name);
  }
  
  while ((entry = readdir(subDirectory)) != NULL) { 
    if ((strcmp(".", entry->d_name) == 0) || (strcmp("..", entry->d_name) == 0)) { // skip current and parent directories
      continue;
    }

    char entry_name[MAX_ENTRY_NAME_LENGTH] = {'\0'};
    snprintf(entry_name, sizeof(entry_name), "%s/%s", name, entry->d_name);

    lstat(entry_name, st);
    mode_t entryType = st->st_mode;

    if(S_ISREG(entryType)){ //found a file. search for pattern.

      long unsigned int inode = st->st_ino; //assign inode

      int i;
      int setContinue = 0;
      for (i = 0; i < numHardLinks; i++) { // check if inode already exists in inode array
        if (inode == hardLinkInodes[i]) {
          // printf("Duplicate Hard Link --- Inode: %lu\n", inode);
          setContinue = 1;
          break;
        }
      }

      if (setContinue) {
        continue;
      }

      if (numHardLinks == sizeOfHardLinkArray) { // resize inode array if neccessary
        sizeOfHardLinkArray *= 2;
        hardLinkInodes = (long unsigned int*) realloc(hardLinkInodes, sizeof(long unsigned int)*sizeOfHardLinkArray);
      }

      hardLinkInodes[numHardLinks] = inode;
      numHardLinks++;

      searchPatternInFile(entry_name, pattern);
    }
    else if(S_ISDIR(entryType)){//directory. look for everything in it
      
      char new_path[MAX_ENTRY_NAME_LENGTH] = {'\0'};
      snprintf(new_path, sizeof(new_path), "%s/%s", name, entry->d_name);

      dirTraverse(new_path, pattern);
    }
    else if(S_ISLNK(entryType)){
   //   printf("Symbolic Link --- File: %s, Inode: %lu\n", entry->d_name, st->st_ino);
    }
    else{
      printf("ERROR: Assumption Violation: file type not supported");
      exit(EXIT_FAILURE);
    }
  }
  closedir(subDirectory);
  free(st);
	//Recursively traverse the directory and call SearchForPattern when neccessary
}

int main(int argc, char** argv){
	if(argc != 3){
	
		fprintf(stderr,"Child process : %d received %d arguments, expected 3 \n",getpid(), argc);
		fprintf(stderr,"Usage child.o [Directory Path] [Pattern] \n");
		exit(EXIT_FAILURE);
	}
	
	char* path = argv[1];
	char* pattern = argv[2];

  hardLinkInodes = (long unsigned int*) malloc(sizeof(long unsigned int)*sizeOfHardLinkArray);

  dirTraverse(path, pattern);

  free(hardLinkInodes);

	close(STDIN_FILENO);
  fflush(stdout);
	close(STDOUT_FILENO);
	exit(EXIT_SUCCESS);
  }

