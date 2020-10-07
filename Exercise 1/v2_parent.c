#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#define IN 0
#define OUT 1
#define SIZE 50


int main(int args , char* argv[]) {

	if (args != 2 || strcmp(argv[1], "--help") == 0){
		fprintf(stdout, "‫‪usage: %s <FILE NAME>\n", argv[0]);
		fflush(stdout);
		return EXIT_FAILURE;
	}

	/* c1_in  - parent write to child 1
	   c1_out - child 1 write to parent
	   c2_in  - parent write to child 2
	   c2_out - child 2 write to parent
	 */
    int c1_in[2],c1_out[2],c2_out[2] ,c2_in[2]; // Pipe
    int gcd,readSuccess,readNum1 ,readNum2,countLines = 0;
    int numbers[4];
    char str[SIZE];

    //Create pipe
    if (pipe(c1_in) == -1 || pipe(c1_out) == -1 || pipe(c2_in) == -1 || pipe(c2_out) == -1) {
    	perror("pipe() failed");
    	return EXIT_FAILURE;
    }

    //Open file
    FILE* f = fopen(argv[1], "r");
    if (!f) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    //Create child 1
    switch (fork()) {
    case -1:
    	perror("First fork() failed");
    	return EXIT_FAILURE;

    case 0: //Child 1
    	change_fd(c1_in,c1_out);
		execlp("./v2_child", "v2_child", NULL);
		fprintf(stderr, "execlp() failed");
		return EXIT_FAILURE;

    default: //Parent
        switch (fork()) { //Create child 2
        case -1:
        	perror("Second fork() failed");
        	return EXIT_FAILURE;

        case 0: //Child 2
        	change_fd(c2_in,c2_out);
            execlp("./v2_child", "v2_child", NULL);
            fprintf(stderr, "execlp() failed");
            return EXIT_FAILURE;

        default:
            break;
        }
    }


    close_fd(c1_in[IN]);    //Close read:  parent to child 1
    close_fd(c1_out[OUT]);  //Close write: child 1 to parent
    close_fd(c2_in[IN]);	//Close read:  parent to child 2
    close_fd(c2_out[OUT]);  //Close write: child 2 to parent

    //Start read from file
    while (fgets(str,sizeof(str),f)!= NULL) {
    	countLines++;
    	readSuccess = getNumbers(str,countLines,numbers);
    	if(readSuccess == 0){
    	write(c1_in[OUT], &numbers[0], sizeof(int)); //write to child 1
    	write(c1_in[OUT], &numbers[1], sizeof(int));
    	readNum1 = 1;
        if (fgets(str,sizeof(str),f)!= NULL){
        	countLines++;
        	readSuccess = getNumbers(str,countLines,numbers+2);
        	if(readSuccess == 0){
        	write(c2_in[OUT], &numbers[2], sizeof(int)); //write to child 2
        	write(c2_in[OUT], &numbers[3], sizeof(int));
        	readNum2 = 1;
        	}
        }
        if(readNum1 == 1){
        	if (read(c1_out[IN], &gcd, sizeof(int)) == EOF) //read from child 1
        		break;
        	printf("%d %d gcd: %d\n", numbers[0] , numbers[1], gcd);
        	readNum1 = 0;
        }
        if(readNum2 == 1){
            if (read(c2_out[IN], &gcd, sizeof(int)) == EOF) //read from child 2
                break;
            printf("%d %d gcd: %d\n", numbers[2] , numbers[3], gcd);
            readNum2 = 0;
        }
    	}
    }
    int finishRead = -1;
    write(c1_in[OUT], &finishRead, sizeof(int)); //write to child 1 to finish
    write(c2_in[OUT], &finishRead, sizeof(int)); //write to child 2 to finish
    close_fd(c1_in[OUT]); //Close write: parent to child 1
    close_fd(c2_in[OUT]); //Close write: parent to child 2

    return EXIT_SUCCESS;
}

void change_fd(int* pipe_in , int* pipe_out){
	close_fd(pipe_out[IN]);   //Close read: child to parent
	close_fd(pipe_in[OUT]);   //Close write: parent to child
	close_fd(STDIN_FILENO);	  //Close STDIN
	dup(pipe_in[IN]);		  //Duplicate STDIN on pipe - (read) parent to child
	close_fd(STDOUT_FILENO);  //Close STDOUT
	dup(pipe_out[OUT]);		  //Duplicate STDOUT on pipe - (write) child to parent
}

int getNumbers(char* str, int countLine, int* nums) {
	char* token;
	int counterNum = 0,num;
	token = strtok(str, " ");
	while (token != NULL || counterNum == 1) {
		if(token != NULL)
		num = atoi(token);
		if (num < 0 || counterNum == 2 || token == NULL ) {
			printf("illegal input in line %d\n", countLine);
			return -1;
		}
		nums[counterNum] = num;
		counterNum++;
		token = strtok(NULL, " \r\n");
	}

	return 0;
}

void close_fd(int fd) {
	if (close(fd) == -1) {
		perror("close() failed");
		exit(EXIT_FAILURE);
	}
}
