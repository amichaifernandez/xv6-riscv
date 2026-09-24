#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[]){
	if(argc < 2){
		fprintf(2, "usage: time1 <command> [args...]\n");
		exit(1);
	}

	int start  = uptime(); //before fork


	int pid = fork();
	if (pid < 0){
		fprintf(2, "time1: fork failed\n");
		exit(1);
	}

	if(pid == 0){ //child
		//child: argv[1] is the program, &argv[1] is the argument array
		exec(argv[1], &argv[1]);
		fprintf(2, "time1: exec %s failded\n", argv[1]);
		exit(1);
	}

	//parent waits for the  child then measures
	wait(0);
	int end = uptime();

	printf("elapsed time: %d ticks\n", end - start);
	exit(0);
}
