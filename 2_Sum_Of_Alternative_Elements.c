#include <stdio.h>

#define ELEMENT_INCREMENT_BY 2

void sumOfAlternativeElements(int *array, int arraySize, int *sumOfElements);

int main()
{
    int numberOfElements;
    int sumOfElements = 0;
    char characterInBuffer;

    printf("Enter number of elements: ");
    if (scanf("%d", &numberOfElements) != 1)
    {
        printf("Error: Invalid number of elements. Must be a integer value.\n");
        return 0;
    }

    if (numberOfElements <= 0)
    {
        printf("Error: Invalid number of elements. Please enter an integer greater than 0.\n");
        return 0;
    }

    int array[numberOfElements];

    printf("Enter elements: ");
    for (int currentElement = 0; currentElement < numberOfElements; currentElement++)
    {
        if (scanf("%d", (array + currentElement)) != 1)
        {
            printf("Error: Invalid input. Please enter a valid integer.\n", numberOfElements);
            while ((characterInBuffer = getchar()) != '\n')
                ;
            return 0;
        }
    }

    sumOfAlternativeElements(array, numberOfElements, &sumOfElements);
    printf("Sum of alternate elements = %d", sumOfElements);

    return 0;
}

void sumOfAlternativeElements(int *array, int arraySize, int *sumOfElements)
{
    for (int *currentIndex = array; currentIndex < (array + arraySize); currentIndex += ELEMENT_INCREMENT_BY)
    {
        *sumOfElements += *(currentIndex);
    }
}
