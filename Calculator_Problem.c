#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 1000

// Stack to store digits
int numStack[MAX];
int top_NumStack = -1;

void pushNum(int val) {
    if (top_NumStack >= MAX) {
        printf("Number Stack overflow.");
        exit(1);
    }

    numStack[++top_NumStack] = val;
}

int popNum() {
    if(top_NumStack < 0) {
        printf("NumStack is underflow");
        exit(1);
    }

    return numStack[top_NumStack--];
} 


// Stack to store operators
char opStack[MAX];
int top_opStack = -1;

void pushOp(int op) {
    if(top_opStack >= MAX) {
        printf("Operator stack overflow.");
        exit(1);
    }

    opStack[++top_opStack] = op;
}

char popOp() {
    if(top_opStack < 0) {
        printf("Operator stack underflow");
        exit(1);
    }

    return opStack[top_opStack--];
}

char peekOp() {
    if(top_opStack < 0) {
        printf("Operator stack is empty");
        return 0;
    }

    return opStack[top_opStack];    
}

int precedence(char op) {
    if(op == '+' || op == '-') {
        return 1;
    }

    if(op == '*' || op == '/') {
        return 2;
    }

    return 0;
}

int calculate(int num1, int num2, char op) {
    if(op == '+') {
        return num1 + num2;
    }
    else if(op == '-') {
        return num1 - num2;
    }
    else if(op == '*') {
        return num1 * num2;
    }
    else {
        if(num2 == 0) {
            printf("Error: Division by zero.\n");
            exit(1);
        }
        return num1 / num2;
    }

    return 0;
}

int solve(char expression[1000]) {
    int i = 0;

    while(expression[i] != '\0') {
        if(isspace(expression[i])) {
            i++;
            continue;
        }

        if(isdigit(expression[i])) {
            int val = 0;
            while(isdigit(expression[i])) {
                val = val * 10 + (expression[i] - '0');
                i++;
            }

            pushNum(val);
            continue;
        }

        if(strchr("+-*/", expression[i])) {
            while(top_opStack >= 0 && precedence(peekOp()) >= precedence(expression[i])) {
                int num2 = popNum();
                int num1 = popNum();
                char op = popOp();

                int res = calculate(num1, num2, op);
                pushNum(res);
            }

            pushOp(expression[i]);
        } else {
            printf("Error: Invalid expression.\n");
            exit(1);
        }

        i++;
    }

    while(top_opStack >= 0) {
        int num2 = popNum();
        int num1 = popNum();
        char op = popOp();
        int res = calculate(num1, num2, op);
        pushNum(res);
    }

    return popNum();
}

int main() {
    char expression[1000];

    // Take the expression input
    printf("Enter mathematical expression: ");
    scanf("%[^\n]s", expression);

    printf("%s\n", expression);

    int res = solve(expression);

    printf("Result: %d", res);

    return 0;
}