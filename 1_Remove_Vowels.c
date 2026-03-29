#include <stdio.h>

#define STRING_SIZE 100
#define SUCCESS 1
#define ERROR 0

void trimNewline(char *string);
void removeVowels(char *string);
void printString(char *string);
int validateString(char *string);

int main()
{
    char string[STRING_SIZE];
    printf("Enter a string: ");
    fgets(string, STRING_SIZE, stdin);
    trimNewline(string);

    if (!validateString(string))
    {
        return 0;
    }

    removeVowels(string);
    printString(string);

    return 0;
}

void removeVowels(char *string)
{
    char *consonentPointer = string;
    char *vowelPointer = string;

    while (*consonentPointer != '\0')
    {
        if (*consonentPointer == 'a' || *consonentPointer == 'e' ||
            *consonentPointer == 'i' || *consonentPointer == 'o' ||
            *consonentPointer == 'u' || *consonentPointer == 'A' ||
            *consonentPointer == 'E' || *consonentPointer == 'I' ||
            *consonentPointer == 'O' || *consonentPointer == 'U')
        {
            consonentPointer++;
        }
        else
        {
            *vowelPointer = *consonentPointer;
            vowelPointer++;
            consonentPointer++;
        }
    }

    *vowelPointer = '\0';
}

void printString(char *string)
{
    if (*string == '\0')
    {
        printf("All the characters of the string were vowels. String is empty now.\n");
    }
    else
    {
        printf("String after removing vowels: %s", string);
    }
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
        printf("Error: String must contain a letter.\n");
        return ERROR;
    }

    return SUCCESS;
}
