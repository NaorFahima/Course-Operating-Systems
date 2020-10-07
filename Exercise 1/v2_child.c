#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>



int main(int args , char* argv[]) {

    int num1,num2,num,endFile = -1;

    while (1) {
    	if (read(STDIN_FILENO, &num1, sizeof(int))== EOF){
    	write(STDOUT_FILENO, &endFile , sizeof(int));
    	break;
    	}

        read(STDIN_FILENO, &num2, sizeof(int));
        num = gcd(num1, num2);
        write(STDOUT_FILENO, &num, sizeof(int));

        if(num1 == -1){ // End of file
        write(STDOUT_FILENO, &endFile , sizeof(int));
        break;
        }
    }
    return EXIT_SUCCESS;
}

int gcd(int a, int b){
    if (b == 0)
        return a;
    return gcd(b, a % b);
}




