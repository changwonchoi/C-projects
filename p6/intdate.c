////////////////////////////////////////////////////////////////////////////////
// Main File:        intdate.c
// This File:        intdate.c
// Semester:         CS 354 Fall 2017
//
// Author:           Chang Won Choi
// Email:            cchoi25@wisc.edu
// CS Login:         cchoi
/////////// ////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


int alarm_time = 3;
int sigusr1cnt = 0;

//handler for alarm
void sigalrm_handler() {
    time_t currTime = time(NULL);
		//produces a warning
    printf("PID: %d | Current Time: %s", getpid(), ctime(&currTime));
    alarm(alarm_time);
}


//handler for sigint
void sigint_handler() {
	printf("\n");
	printf("SIGINT received. \n");
	printf("SIGUSR1 was received %i times. Exiting now.\n", sigusr1Count);
	exit(0);
}

//handler for sigusr1
void sigusr1_handler(){
	sigusr1cnt++;
	printf("SIGUSR1 received\n");
}

int main() {

	//sigaction handler are made
	struct sigaction act1;
	struct sigaction act2;
	struct sigaction act3;
	
	memset(&act1,0,sizeof(act1));
	memset(&act2,0,sizeof(act2));
	memset(&act3, 0, sizeof(act3));

	act1.sa_handler = sigalrm_handler;
	act2.sa_handler = sigint_handler;
	act3.sa_handler = sigusr1_handler;

	if (sigaction (SIGALRM, &act1, NULL) != 0) {
		printf("SIGALRM ERROR");
		exit(1);
	};
	
	if (sigaction (SIGINT, &act2, NULL) != 0){
		printf("SIGINT ERROR");
		exit(1);
	};

	if (sigaction (SIGUSR1, &act3, NULL) != 0) {
		printf("SIGUSR1 ERROR");
		exit(1);
	};

	printf("PID and time will be printed every 3 seconds.\n");
	printf("Enter ^C  to end the program:\n");
	alarm(alarm_time);

	while(1) {
	}
	return 0;
}