#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define SIZE 50

int main(int arcg, char* argv[]) {
	char str[SIZE];
	int success ,count = 0;
	char *args[] = {"v1_child","","",NULL};

	printf("Input 2 numbers:\n");
	while (fgets(str, sizeof(str), stdin) != NULL) {
		count++;
		success = getNumbers(str, count, args);

		if(success != -1){
		pid_t pID = fork();

		//Failed create child
		if(pID < 0) {
			perror("Error Creating Child!");
			return EXIT_FAILURE;
		}

		//Child
		if(pID == 0) {
				if(execv("./v1_child", args) == -1) {
					perror("Failed to execute v1_child");
					return EXIT_FAILURE;
				}
		}

		//Parent
		int son_id = wait(&pID);
		if(son_id == -1){
			perror("Child Failed!\n");
		} else {
			printf("gcd: %d \n", WEXITSTATUS(pID));
		}
		}
	}
	return EXIT_SUCCESS;
}

//Take string and split to integer
int getNumbers(char* str, int countLine, char* argv[]) {
	char* token;
	int counterNum = 0,num;
	token = strtok(str, " ");
	while (token != NULL) {
		if(token != NULL)
		num = atoi(token);
		if (num < 0 || counterNum == 2 || token == NULL) {
			printf("illegal input in line %d\n", countLine);
			return -1;
		}
		counterNum++;
		argv[counterNum] = token;
		token = strtok(NULL, " ");
	}
	return 0;
}

