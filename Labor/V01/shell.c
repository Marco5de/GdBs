#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define DEBUG

#ifdef DEBUG
	#include <stdio.h>
#endif

#define TRUE 1
#define STDIN 0
#define BUFF_SIZE 1024
#define PARAMETER_LIST_LEN 256

char *read_from_console();
char **extract_parameterlist(char *input);
//Todo extract parameter list

int main(int argc, char **argv){

	char *in_buf;
	
	while(TRUE){
		printf("CustomShell-$ ");
		fflush(stdout);

		in_buf = read_from_console(&in_buf);

		char **paralist = extract_parameterlist(in_buf);
		char *first_param = paralist[0];
		char **parmList = paralist;


		pid_t child_pid; 
		if((child_pid = fork()) == -1)
			exit(1);
		if(!child_pid){
			if(execv(first_param,parmList)==-1)
				printf("error exec\n");
		}
		//waiting for other process to finish
		//
	}
}


/* Split parameter list by spaces*/
char **extract_parameterlist(char *input){
	char **pch = calloc(PARAMETER_LIST_LEN, BUFF_SIZE);
	if(!pch)exit(1);

	pch[0] = strtok (input," ");

	for(int i=1; pch[i-1] != NULL ; i++){
		pch[i] = strtok (NULL, " ");
	}
	
	return pch;
}

/* Reading input from stdin in continous loop until enter is pressed*/
char *read_from_console(){
	//reading one byte at a time
	char *buffer = calloc(1,BUFF_SIZE);
	if(!buffer)exit(1);

	for(int i = 0; read(STDIN, buffer+i, 1) > 0 ; i++){
		if(buffer[i] == '\n'){
			buffer[i] = '\0';
			break;
		}
	}
	
	return buffer;
}


