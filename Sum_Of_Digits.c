#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

int main() {
	
    int n;
    scanf("%d", &n);
    
    // storing the original n, so that it wont get affected by operation
    int number = n;
    int sum = 0;
    
    while(number > 0) {
        int digit = number % 10;
        sum += digit;
        number /= 10;
    }
    
    printf("%d", sum);
    
    return 0;
}