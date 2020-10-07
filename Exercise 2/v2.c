#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define SIZE 50
#define NUM_THREADS 3

// Link list
typedef struct node {
    int num1,num2,gcd;
    struct node* next;
} node_t;

node_t* push(node_t* head, int* nums);

// Lock and Conditional variable
pthread_mutex_t lock;
pthread_cond_t cv;

// Function that the thread run to do a task
void takeTask(node_t* myList){
	 node_t* temp;
	while(myList != NULL){
	 pthread_mutex_lock(&lock);
	 temp = myList;
	 myList = myList->next;
	 pthread_mutex_unlock(&lock);
	 temp->gcd = gcd(temp->num1,temp->num2);
	}
	pthread_cond_signal(&cv);
}



int main(int args , char* argv[]) {
	pthread_t thread[NUM_THREADS];
	node_t* myList = NULL;
	node_t* tempPtr;
	char str[SIZE];
	int success,error, count = 0,countLine=0;
	int nums[2];

	// Read from file
	if (args == 2) {
		FILE *f = fopen(argv[1], "r");
		if (!f) {
			perror("Failed to open file");
			exit(EXIT_FAILURE);
		}
		while (fgets(str, sizeof(str), f) != NULL) {
			success = getNumbers(str, ++countLine,nums);
			if (success != -1){
				count++;
				myList = push(myList,nums);
			}
		}
	} else { // Read from standard input
		printf("Input 2 numbers:\n");
		while (fgets(str, sizeof(str), stdin) != NULL) {
			success = getNumbers(str, ++countLine, nums);
			if (success != -1) {
				count++;
				myList = push(myList, nums);
			}
		}
	}
	tempPtr = myList;

	/* Launch Threads */
	for (int i = 0; i < NUM_THREADS; i++) {
		error = pthread_create(&thread[i], NULL, takeTask, tempPtr);
		if (error != 0) {
			printf("\nThread can't be created", strerror(error));
			return EXIT_FAILURE;
		}
	}

	pthread_cond_wait(&cv, &lock); // The main thread wait to all threads to finish
    pthread_mutex_destroy(&lock);
	print_list(myList);

	return EXIT_SUCCESS;
}

// Calculate the maximum shared divider between numbers
int gcd(int a, int b){
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

// Take string and split to integer
int getNumbers(char* str, int countLine,int* nums) {
	char* token;
	int counterNum = 0,num;
	token = strtok(str, " ");
	while (token != NULL) {
		num = atoi(token);
		if (num <= 0 || token == NULL) {
			printf("illegal input in line %d\n", countLine);
			return -1;
		}
		nums[counterNum] = atoi(token);
		counterNum++;
		token = strtok(NULL, " \n");
	}
	if (counterNum != 2) {
		printf("illegal input in line %d\n", countLine);
		return -1;
	}
	return 0;
}

// Print all numbers and gcd
void print_list(node_t * head) {
    node_t * current = head;

    while (current != NULL) {
        printf("%d %d gcd: %d\n", current->num1,current->num2,current->gcd);
        current = current->next;
    }
}

// Add new numbers to the list
node_t* push(node_t* head, int* nums) {
	node_t* current = NULL;
	if (head == NULL) {
		head = (node_t*) malloc(sizeof(node_t));
		current = head;
	} else {
		current = head;
		while (current->next != NULL) {
			current = current->next;
		}
	    current->next = (node_t *) malloc(sizeof(node_t));
	    current = current->next;

	}
    /* now we can add a new variable */
  	current->num1 = nums[0];
  	current->num2 = nums[1];
    current->next = NULL;
    current = head;
    return current;
}






