#include <stdio.h>

void shiftZerosToEnd(int *integerArray, int sizeOfArray);
void printArray(int *integerArray, int sizeOfArray);

int main()
{
    int sizeOfArray;

    printf("Enter number of elements: ");
    if (scanf("%d", &sizeOfArray) != 1)
    {
        printf("Error: Invalid number of elements. Must be a positive integer value greater than 0.\n");
        return 0;
    }

    if (sizeOfArray <= 0)
    {
        printf("Error: Number of elements must be greater than 0.\n");
        return 0;
    }

    int integerArray[sizeOfArray];

    printf("Enter array elements: ");
    for (int currentIndex = 0; currentIndex < sizeOfArray; currentIndex++)
    {
        if (scanf("%d", integerArray + currentIndex) != 1)
        {
            printf("Error: Enter a valid integer value.");
            return 0;
        }
    }

    shiftZerosToEnd(integerArray, sizeOfArray);
    printArray(integerArray, sizeOfArray);

    return 0;
}

void shiftZerosToEnd(int *integerArray, int sizeOfArray)
{
    int *zeroPointer = integerArray;
    int *nonZeroPointer = integerArray;

    while (nonZeroPointer < (integerArray + sizeOfArray))
    {
        if (*nonZeroPointer != 0)
        {
            if (nonZeroPointer != zeroPointer)
            {
                int temp = *zeroPointer;
                *zeroPointer = *nonZeroPointer;
                *nonZeroPointer = temp;
            }

            zeroPointer++;
        }

        nonZeroPointer++;
    }
}

void printArray(int *integerArray, int sizeOfArray)
{
    printf("Array after moving zeros: ");
    for (int *currentIndex = integerArray; currentIndex < (integerArray + sizeOfArray); currentIndex++)
    {
        printf("%d ", *currentIndex);
    }
}
