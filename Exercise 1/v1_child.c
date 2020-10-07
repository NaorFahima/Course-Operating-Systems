#include<stdio.h>
#include<unistd.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
	int num1 = atoi(argv[1]) ,num2 = atoi(argv[2]);
	int num;

	//gcd calculate
	while(1){
		if(num1 == 0) {
			break;
		}
		num = num1;
		num1  = num2 % num1;
		num2 = num;
	}
	printf("%d %d ",atoi(argv[1]),atoi(argv[2]));
	return num2;
}
