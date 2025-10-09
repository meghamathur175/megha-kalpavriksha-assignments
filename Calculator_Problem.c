#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SIZE 1000
#define SUCCESS 0
#define ERROR 1

int pushOperand(int val);
int popOperand(int *val);
int pushOperator(char operator);
int popOperator(char *operator);
int peekOperator(char *operator);
int getPrecedence(char operator);
int calculate(int operand1, int operand2, char operator, int *result);
int evaluateTopOperations();
int evaluateExpression(char *expression, int *result);
void removeSpaces(char *expression);

int operandStack[MAX_SIZE];
int gOperandTop = -1;
char operatorStack[MAX_SIZE];
int gOperatorTop = -1;

int pushOperand(int val)
{
    if (gOperandTop >= MAX_SIZE - 1)
    {
        fprintf(stderr, "Error: Operand stack overflow.\n");
        return ERROR;
    }

    operandStack[++gOperandTop] = val;
    return SUCCESS;
}

int popOperand(int *val)
{
    if (gOperandTop < 0)
    {
        fprintf(stderr, "Error: Operand stack underflow.\n");
        return ERROR;
    }

    *val = operandStack[gOperandTop--];
    return SUCCESS;
}

int pushOperator(char operator)
{
    if (gOperatorTop >= MAX_SIZE - 1)
    {
        fprintf(stderr, "Error: Operator stack overflow.\n");
        return ERROR;
    }

    operatorStack[++gOperatorTop] = operator;
    return SUCCESS;
}

int popOperator(char *operator)
{
    if (gOperatorTop < 0)
    {
        fprintf(stderr, "Error: Operator stack underflow.\n");
        return ERROR;
    }

    *operator = operatorStack[gOperatorTop--];
    return SUCCESS;
}

int peekOperator(char *operator)
{
    if (gOperatorTop < 0)
    {
        fprintf(stderr, "Error: Operator stack is empty.\n");
        return ERROR;
    }

    *operator = operatorStack[gOperatorTop];
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

int evaluateTopOperations()
{
    int operand2;
    int operand1;
    char operator;
    int computedValue;

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

    if (calculate(operand1, operand2, operator, &computedValue))
    {
        return ERROR;
    }

    if (pushOperand(computedValue))
    {
        return ERROR;
    }

    return SUCCESS;
}

int evaluateExpression(char *expression, int *result)
{
    int exprIdx = 0;
    gOperandTop = -1;
    gOperatorTop = -1;
    int expectedChar = 1;
    int sign = 1;

    while (isspace((unsigned char)(expression[exprIdx])))
    {
        exprIdx++;
    }

    if (expression[exprIdx] == '\0')
    {
        fprintf(stderr, "Error: Empty expression.\n");
        return ERROR;
    }

    while (expression[exprIdx] != '\0')
    {
        if (isspace((unsigned char)(expression[exprIdx])))
        {
            exprIdx++;
            continue;
        }

        if (expectedChar)
        {
            if (expression[exprIdx] == '+' || expression[exprIdx] == '-')
            {
                sign = 1;
                while (expression[exprIdx] == '-' || expression[exprIdx] == '+')
                {
                    if (expression[exprIdx] == '-')
                    {
                        sign = -sign;
                    }

                    exprIdx++;
                }
            }
            else if (!isdigit((unsigned char)(expression[exprIdx])))
            {
                fprintf(stderr, "Error: Invalid expression. Expected a number but found '%c'\n", expression[exprIdx]);
                return ERROR;
            }
        }

        if (isdigit((unsigned char)(expression[exprIdx])))
        {
            int val = 0;
            while (isdigit((unsigned char)(expression[exprIdx])))
            {
                val = val * 10 + (expression[exprIdx] - '0');
                exprIdx++;
            }

            if (pushOperand(sign * val))
            {
                return ERROR;
            }

            sign = 1;
            expectedChar = 0;
            continue;
        }

        if (!expectedChar)
        {
            if (strchr("+-*/", expression[exprIdx]))
            {
                char topOp;
                while (gOperatorTop >= 0 && peekOperator(&topOp) == SUCCESS && getPrecedence(topOp) >= getPrecedence(expression[exprIdx]))
                {
                    if (evaluateTopOperations())
                    {
                        return ERROR;
                    }
                }

                if (pushOperator(expression[exprIdx]))
                {
                    return ERROR;
                }

                exprIdx++;
                expectedChar = 1;
            }
            else
            {
                fprintf(stderr, "Error: Invalid expression. Expected operator but found '%c'\n", expression[exprIdx]);
                return ERROR;
            }
        }
    }

    while (gOperatorTop >= 0)
    {
        if (evaluateTopOperations())
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

void removeSpaces(char *expression)
{
    int scanIdx = 0;
    int writeIdx = 0;
    while (expression[scanIdx] != '\0')
    {
        if (!isspace((unsigned char)(expression[scanIdx])))
        {
            expression[writeIdx++] = expression[scanIdx];
        }

        scanIdx++;
    }

    expression[writeIdx] = '\0';
}

int main()
{
    char expression[MAX_SIZE];

    if (fgets(expression, MAX_SIZE, stdin) != NULL)
    {
        size_t exprLength = strlen(expression);

        if (exprLength > 0 && expression[exprLength - 1] == '\n')
        {
            expression[exprLength - 1] = '\0';
        }
    }
    else
    {
        fprintf(stderr, "Error reading input.\n");
        return ERROR;
    }

    removeSpaces(expression);

    int result;
    if (evaluateExpression(expression, &result))
    {
        return ERROR;
    }

    printf("%d\n", result);

    return 0;
}
