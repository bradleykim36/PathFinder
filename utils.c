/*test machine: CSEL-KH4250-26
* group number: G31
* name: Brendan Lake , Bradley Kim, Jacob Jung
* x500: lake0112 , kim00544, jungx505 */

#include<stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include<string.h>
#include "utils.h"


/*
	Searches for pattern in file and writes the entire line containing the pattern to STDOUT
	Format for print: [path to file] : Line in the file
	@Params:
		path - path to the file
		pattern : pattern to be searched
*/



void searchPatternInFile(char* path, char* pattern){

	FILE *fp = fopen(path, "r");

	if (fp == NULL){
		fprintf(stderr,"Error opening file: %s \n",path);
		return;
	}
  
	char buff[MAX_WRITE_LENGTH];
  
  while(fgets(buff, sizeof(buff),fp)!= NULL) {
    if(strstr(buff, pattern) != NULL){
      printf("%s: %s",path, buff); // send each line to the pipe (or print from master)
    }
  }

	fclose(fp);
}
