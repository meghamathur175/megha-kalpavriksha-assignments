#include <stdio.h>

#define MAXIMUM_WORD_SIZE 50

void trimNewline(char *word);
void findLongestWord(char (*wordsArray)[MAXIMUM_WORD_SIZE], int numberOfWords, char **maximumLengthWordPointer, int *maximumWordLength);
int calculateLengthOfWord(char *word);

int main()
{
    int numberOfWords;
    int maximumWordLength = 0;

    printf("Enter number of words: ");
    if (scanf("%d", &numberOfWords) != 1)
    {
        printf("Error: Enter a valid number of words. Must be a integer greater than 0.\n");
        return 0;
    }

    if (numberOfWords <= 0)
    {
        printf("Error: Number of words must be greater than 0.\n");
        return 0;
    }
    getchar();

    char wordsArray[numberOfWords][MAXIMUM_WORD_SIZE];
    char *maximumLengthWordPointer = *(wordsArray);

    printf("Enter words: \n");
    for (int currentIndex = 0; currentIndex < numberOfWords; currentIndex++)
    {
        fgets(*(wordsArray + currentIndex), MAXIMUM_WORD_SIZE, stdin);
        trimNewline(*(wordsArray + currentIndex));
    }

    findLongestWord(wordsArray, numberOfWords, &maximumLengthWordPointer, &maximumWordLength);
    printf("The longest word is: %s\n", maximumLengthWordPointer);
    printf("Length: %d", maximumWordLength);

    return 0;
}

int calculateLengthOfWord(char *word)
{
    int wordLength = 0;

    while (*word != '\0')
    {
        wordLength++;
        word++;
    }

    return wordLength;
}

void findLongestWord(char (*wordsArray)[MAXIMUM_WORD_SIZE], int numberOfWords, char **maximumLengthWordPointer, int *maximumWordLength)
{
    for (int currentIndex = 0; currentIndex < numberOfWords; currentIndex++)
    {
        char *currentWordPointer = *(wordsArray + currentIndex);
        int lengthOfCurrentWord = calculateLengthOfWord(currentWordPointer);
        int lengthOfLongestWord = calculateLengthOfWord(*maximumLengthWordPointer);

        if (lengthOfCurrentWord == 0)
        {
            continue;
        }

        if (lengthOfLongestWord < lengthOfCurrentWord)
        {
            *maximumLengthWordPointer = currentWordPointer;
            *maximumWordLength = lengthOfCurrentWord;
        }
    }
}

void trimNewline(char *word)
{
    int currentIndex = 0;

    while (*(word + currentIndex) != '\0')
    {
        if (*(word + currentIndex) == '\n')
        {
            *(word + currentIndex) = '\0';
            break;
        }

        currentIndex++;
    }
}
