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

// Thread Data
typedef struct ThreadData {
	int size;
	struct node* start;
} ThreadData;

node_t* push(node_t* head, int* nums);

// Function that the thread run
void* funcThread(ThreadData* td) {
	node_t* head = td->start;
	for (int i = 0; i < td->size; i++) {
		if (head != NULL) {
			head->gcd = gcd(head->num1, head->num2);
			head = head->next;
    	}
    }
    return NULL;
}


int main(int args , char* argv[]) {
	pthread_t thread[NUM_THREADS];
	ThreadData data[NUM_THREADS];
	node_t* myList = NULL;
	node_t* tempPtr;
	char str[SIZE];
	int tasksPerThread,success,error, count = 0,countLine=0;
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


	// Divide the numbers between the threads
    tasksPerThread = count/NUM_THREADS;
	tempPtr = myList;
	for (int i = 0; i < NUM_THREADS; i++) {
		data[i].start = tempPtr;
		data[i].size = tasksPerThread;
		for (int j = 0; j < data[i].size; j++) {
			tempPtr = tempPtr->next;
		}
	}

	/* Launch Threads */
	for (int i = 0; i < NUM_THREADS; i++) {
		error = pthread_create(&thread[i], NULL, funcThread, &data[i]);
		if (error != 0) {
			printf("\nThread can't be created", strerror(error));
			return EXIT_FAILURE;
		}
	}

	// The main thread calculate some of the numbers
	while (tempPtr != NULL) {
		tempPtr->gcd = gcd(tempPtr->num1, tempPtr->num2);
		tempPtr = tempPtr->next;
	}

	/* Wait for Threads to Finish */
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join(thread[i], NULL);
	}

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






