#include <stdio.h>

#define MAXIMUM_SIZE 100
#define SUCCESS 1
#define ERROR 0

int validateString(char *string);
void trimNewline(char *string);
void stringCopy(char *inputString, char *copiedString);

int main()
{
    char inputString[MAXIMUM_SIZE];
    char copiedString[MAXIMUM_SIZE];

    printf("Source: ");
    fgets(inputString, MAXIMUM_SIZE, stdin);
    trimNewline(inputString);

    if (!validateString(inputString))
    {
        return 0;
    }

    stringCopy(inputString, copiedString);
    printf("Copied string: %s", copiedString);

    return 0;
}

void stringCopy(char *inputString, char *copiedString)
{
    char *inputStringPointer = inputString;
    char *copiedStringPointer = copiedString;

    while (*(inputStringPointer) != '\0')
    {
        *copiedStringPointer = *inputStringPointer;
        inputStringPointer++;
        copiedStringPointer++;
    }

    *copiedStringPointer = '\0';
}

int validateString(char *string)
{
    int currentIndex = 0;
    int isEmptyString = 1;

    if (*string == '\0')
    {
        printf("Error: Empty string. Enter a valid string.\n");
        return ERROR;
    }

    while (*(string + currentIndex) != '\0')
    {
        if (*(string + currentIndex) == ' ' || *(string + currentIndex) == '\n' || *(string + currentIndex) == '\t')
        {
            currentIndex++;
        }
        else
        {
            isEmptyString = 0;
            break;
        }
    }

    if (isEmptyString)
    {
        printf("Error: Empty string. String must contain a letter.\n");
        return ERROR;
    }

    return SUCCESS;
}

void trimNewline(char *string)
{
    int currentIndex = 0;

    while (*(string + currentIndex) != '\0')
    {
        if (*(string + currentIndex) == '\n')
        {
            *(string + currentIndex) = '\0';
            break;
        }

        currentIndex++;
    }
}
