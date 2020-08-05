////////////////////////////////////////////////////////////////////////////////
// Main File:        sendsig.c
// This File:        sendsig.c
// Semester:         CS 354 Fall 2017
//
// Author:           Chang Won Choi
// Email:            cchoi25@wisc.edu
// CS Login:         cchoi
/////////// ////////////////////////////////////////////////////////////////////


#include  <stdio.h>
#include  <signal.h>
#include <stdlib.h>
#include <string.h>


pid_t pid;

int main(int argc, char *argv[] ){
     
	 //Make sure that there are 3 args
     if (argc != 3) {
          printf("Usage: <signal type> <pid>\n");
          exit(1);
     }
	 
     //gets pid provided
     pid = atoi(argv[2]);
	 
  
     if (strcmp(argv[1] ,"-i") == 0) {
	      kill(pid, 2);
     }//-i for SIGINT
     else if (strcmp(argv[1] ,"-u") == 0) {
         kill(pid, 10);
     }//-u for SIGUSR1
	 
     return 0;
}
