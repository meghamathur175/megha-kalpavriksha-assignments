#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SIZE 1000
#define SUCCESS 0
#define ERROR 1

// Stack to store digits
int operandStack[MAX_SIZE];
int operandTop = -1;

int pushOperand(int val)
{
    if (operandTop >= MAX_SIZE - 1)
    {
        fprintf(stderr, "Error: Operand stack overflow.\n");
        return ERROR;
    }

    operandStack[++operandTop] = val;
    return SUCCESS;
}

int popOperand(int *val)
{
    if (operandTop < 0)
    {
        fprintf(stderr, "Error: Operand stack underflow.\n");
        return ERROR;
    }

    *val = operandStack[operandTop--];
    return SUCCESS;
}

// Stack to store operators
char operatorStack[MAX_SIZE];
int operatorTop = -1;

int pushOperator(char operator)
{
    if (operatorTop >= MAX_SIZE - 1)
    {
        fprintf(stderr, "Error: Operator stack overflow.\n");
        return ERROR;
    }

    operatorStack[++operatorTop] = operator;
    return SUCCESS;
}

int popOperator(char *operator)
{
    if (operatorTop < 0)
    {
        fprintf(stderr, "Error: Operator stack underflow.\n");
        return ERROR;
    }

    *operator = operatorStack[operatorTop--];
    return SUCCESS;
}

int peekOperator(char *operator)
{
    if (operatorTop < 0)
    {
        fprintf(stderr, "Error: Operator stack is empty.\n");
        return ERROR;
    }

    *operator = operatorStack[operatorTop];
    return SUCCESS;
}

int getPrecedence(char operator)
{
    if (operator == '+' || operator == '-')
    {
        return 1;
    }

    if (operator == '*' || operator == '/')
    {
        return 2;
    }

    return 0;
}

int calculate(int operand1, int operand2, char operator, int *result)
{
    if (operator == '+')
    {
        *result = operand1 + operand2;
    }
    else if (operator == '-')
    {
        *result = operand1 - operand2;
    }
    else if (operator == '*')
    {
        *result = operand1 * operand2;
    }
    else if (operator == '/')
    {
        if (operand2 == 0)
        {
            fprintf(stderr, "Error: Division by zero.\n");
            return ERROR;
        }
        *result = operand1 / operand2;
    }
    else
    {
        fprintf(stderr, "Error: Unknown operator '%c'.\n", operator);
        return ERROR;
    }

    return SUCCESS;
}

int evaluateExpression(char *expression, int *result)
{
    int idx = 0;

    while (expression[idx] == ' ')
    {
        idx++;
    }

    if (expression[idx] == '\0')
    {
        fprintf(stderr, "Error: Empty expression.\n");
        return ERROR;
    }

    // Reset stack
    operandTop = -1;
    operatorTop = -1;

    int expectedChar = 1; // 1 -> expecting operand, 0-> expecting operator
    int sign = 1;         // 1 -> positive sign(+), -1 -> negative sign(-)

    while (expression[idx] != '\0')
    {
        if (isspace((unsigned char)(expression[idx])))
        {
            idx++;
            continue;
        }

        if (expectedChar)
        {
            // process consecutive unary
            if (expression[idx] == '+' || expression[idx] == '-')
            {
                sign = 1;
                while (expression[idx] == '-' || expression[idx] == '+')
                {
                    if (expression[idx] == '-')
                    {
                        sign = -sign;
                    }

                    idx++;
                }
            }
            else if (!isdigit((unsigned char)(expression[idx])))
            {
                fprintf(stderr, "Error: Invalid expression. Expected a number but found '%c'\n", expression[idx]);
                return ERROR;
            }
        }

        if (isdigit((unsigned char)(expression[idx])))
        {
            int val = 0;
            while (isdigit((unsigned char)(expression[idx])))
            {
                val = val * 10 + (expression[idx] - '0');
                idx++;
            }

            if (pushOperand(sign * val))
            {
                return ERROR;
            }

            sign = 1;
            expectedChar = 0; // expecting an operator
            continue;
        }

        if (!expectedChar)
        { // expecting an operator
            if (strchr("+-*/", expression[idx]))
            {
                char topOp;
                while (operatorTop >= 0 && peekOperator(&topOp) == SUCCESS && getPrecedence(topOp) >= getPrecedence(expression[idx]))
                {
                    int operand2;
                    int operand1;
                    char operator;
                    int res;

                    if (popOperand(&operand2))
                    {
                        return ERROR;
                    }

                    if (popOperand(&operand1))
                    {
                        return ERROR;
                    }

                    if (popOperator(&operator))
                    {
                        return ERROR;
                    }

                    if (calculate(operand1, operand2, operator, &res))
                    {
                        return ERROR;
                    }

                    if (pushOperand(res))
                    {
                        return ERROR;
                    }
                }

                if (pushOperator(expression[idx]))
                {
                    return ERROR;
                }

                idx++;
                expectedChar = 1; // expecting an operand
            }
            else
            {
                fprintf(stderr, "Error: Invalid expression. Expected operator but found '%c'\n", expression[idx]);
                return ERROR;
            }
        }
    }

    while (operatorTop >= 0)
    {
        int operand2;
        int operand1;
        char operator;
        int res;

        if (popOperand(&operand2))
        {
            return ERROR;
        }

        if (popOperand(&operand1))
        {
            return ERROR;
        }

        if (popOperator(&operator))
        {
            return ERROR;
        }

        if (calculate(operand1, operand2, operator, &res))
        {
            return ERROR;
        }

        if (pushOperand(res))
        {
            return ERROR;
        }
    }

    if (popOperand(result))
    {
        return ERROR;
    }

    return SUCCESS;
}

int main()
{
    char expression[MAX_SIZE];

    // Take the expression input
    if (fgets(expression, MAX_SIZE, stdin) != NULL)
    {
        size_t length = strlen(expression);

        if (length > 0 && expression[length - 1] == '\n')
        {
            expression[length - 1] = '\0';
        }
    }
    else
    {
        fprintf(stderr, "Error reading input.\n");
        return ERROR;
    }

    int readIdx = 0;
    int writeIdx = 0;
    while (expression[readIdx] != '\0')
    {
        if (!isspace((unsigned char)(expression[readIdx])))
        {
            expression[writeIdx++] = expression[readIdx];
        }

        readIdx++;
    }

    expression[writeIdx] = '\0';

    int result;
    if (evaluateExpression(expression, &result))
    {
        return ERROR;
    }

    printf("%d\n", result);

    return 0;
}
