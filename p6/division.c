////////////////////////////////////////////////////////////////////////////////
// Main File:        division.c
// This File:        division.c
// Semester:         CS 354 Fall 2017
//
// Author:           Chang Won Choi
// Email:            cchoi25@wisc.edu
// CS Login:         cchoi
////////////////////////////////////////////////////////////////////////////////



#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>


int op_cnt=0;
int first_int_chk = 1;
int first=0;
int second=0;

void handle_signal( int signal ) {

	switch (signal) {
		//divison by zero division detected
    case SIGFPE:
    	printf("Error: a division by 0 operation was attempted.\n");
			printf("Total number of operations completed successfully: %i\n",op_cnt);
			printf("The program will be terminated.\n");
			exit(0);
	    break; //terminate
  	case SIGINT:
    	printf("\n");
			printf("Total number of operations successfully completed: %i\n",op_cnt);
			printf("The program will be terminated.\n");
			exit(0);
			break;

    default:
      fprintf(stderr, "Caught wrong signal.\n");
      return;
	}
}



int main () {
	//ask for first integer
	printf("Enter first integer: ");
	fflush(stdout);
	struct sigaction sa;
	 // Setup the handler
	sa.sa_handler = &handle_signal;

	// block signals
	sigfillset(&sa.sa_mask);
	if (sigaction(SIGINT,&sa,NULL) != 0){
		printf("SIGINT ERROR\n");
		exit(1);
	};
	if (sigaction(SIGFPE,&sa,NULL) != 0) {
		printf("SIGFPE ERROR\n");
		exit(1);
	};

	while (1) {

		// Check for signals
		signal(SIGINT, handle_signal);
		char buffer[100];
		fgets(buffer, 100, stdin);
		int inputInt = atoi(buffer);

		//store first and second int
		if(first_int_chk){
			first = inputInt;
			printf("Enter second integer: ");
		}

		else{
			second = inputInt;
			printf("%i / %i is %i with a remainder of %i\n", first, second,
																		first/second, first-first/ second*second);
			printf("Enter first integer: ");
			op_cnt++;
		}

		// ask for first int then second int
		first_int_chk = 1 - first_int_chk;
	}
	return 0;
}