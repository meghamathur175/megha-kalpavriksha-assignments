#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MIN_SONARIMAGE_VALUE 0
#define MAX_SONARIMAGE_VALUE 255

void generateRandomMatrix(int *sonarImage, int sonarImageSize);
void printSonarImageMatrix(int *sonarImage, int sonarImageSize);
void generateRotatedMatrix(int *sonarImage, int sonarImageSize);
void generateTransposeMatrix(int *sonarImage, int sonarImageSize);
void swapValues(int *value1, int *value2);
void applySmoothingFilter(int *sonarImage, int sonarImageSize);

int main()
{
    int sonarImageSize;
    printf("Enter matrix size (2-10): ");
    scanf("%d", &sonarImageSize);

    if (sonarImageSize < 2 || sonarImageSize > 10)
    {
        printf("Invalid matrix size. Must be in range (2-10)");
    }

    int sonarImage[sonarImageSize][sonarImageSize];

    printf("Original Randomly Generated Matrix: \n");
    generateRandomMatrix((int *)sonarImage, sonarImageSize);

    printf("\nMatrix after 90 degree Clockwise Rotation: \n");
    generateRotatedMatrix((int *)sonarImage, sonarImageSize);

    printf("\nMatrix after Applying 3X3 Smoothing Filter:  \n");
    applySmoothingFilter((int *)(sonarImage), sonarImageSize);

    return 0;
}

void printSonarImageMatrix(int *sonarImage, int sonarImageSize)
{
    for (int rowIndex = 0; rowIndex < sonarImageSize; rowIndex++)
    {
        for (int columnIndex = 0; columnIndex < sonarImageSize; columnIndex++)
        {
            printf("%d ", *(sonarImage + rowIndex * sonarImageSize + columnIndex));
        }

        printf("\n");
    }
}

void swapValues(int *valueFirst, int *valueSecond)
{
    int temporaryStore = *valueFirst;
    *valueFirst = *valueSecond;
    *valueSecond = temporaryStore;
}

void generateRandomMatrix(int *sonarImage, int sonarImageSize)
{
    srand(time(0));

    for (int row = 0; row < sonarImageSize; row++)
    {
        for (int column = 0; column < sonarImageSize; column++)
        {
            *(sonarImage + row * sonarImageSize + column) = (rand() % (MAX_SONARIMAGE_VALUE - MIN_SONARIMAGE_VALUE + 1)) + MIN_SONARIMAGE_VALUE;
        }
    }

    printSonarImageMatrix((int *)(sonarImage), sonarImageSize);
}

void generateTransposeMatrix(int *sonarImage, int sonarImageSize)
{
    for (int row = 0; row < sonarImageSize; row++)
    {
        for (int column = row + 1; column < sonarImageSize; column++)
        {
            if (row != column)
            {
                swapValues((sonarImage + row * sonarImageSize + column), (sonarImage + column * sonarImageSize + row));
            }
        }
    }
}

void generateRotatedMatrix(int *sonarImage, int sonarImageSize)
{
    int *startColumn;
    int *endColumn;
    generateTransposeMatrix((int *)(sonarImage), sonarImageSize);

    for (int row = 0; row < sonarImageSize; row++)
    {
        startColumn = sonarImage + row * sonarImageSize;
        endColumn = sonarImage + row * sonarImageSize + (sonarImageSize - 1);

        while (startColumn < endColumn)
        {
            int temporaryStore = *startColumn;
            *startColumn = *endColumn;
            *endColumn = temporaryStore;

            startColumn++;
            endColumn--;
        }
    }

    printSonarImageMatrix((int *)(sonarImage), sonarImageSize);
}

void applySmoothingFilter(int *sonarImage, int sonarImageSize)
{
    int originalPreviousRow[10];
    int originalCurrentRow[10];
    int *originalPreviousRowPointer = originalPreviousRow;
    int *originalCurrentRowPointer = originalCurrentRow;

    for (int rowIndex = 0; rowIndex < sonarImageSize; rowIndex++)
    {
        for (int columnIndex = 0; columnIndex < sonarImageSize; columnIndex++)
        {
            *(originalCurrentRowPointer + columnIndex) = *(sonarImage + rowIndex * sonarImageSize + columnIndex);
        }

        for (int columnIndex = 0; columnIndex < sonarImageSize; columnIndex++)
        {
            int sumOfElements = 0;
            int totalElements = 0;

            if (rowIndex > 0)
            {
                if (columnIndex > 0)
                {
                    sumOfElements += *(originalPreviousRowPointer + columnIndex - 1);
                    totalElements++;
                }
                sumOfElements += *(originalPreviousRowPointer + columnIndex);
                totalElements++;
                if (columnIndex < sonarImageSize - 1)
                {
                    sumOfElements += *(originalPreviousRowPointer + columnIndex + 1);
                    totalElements++;
                }
            }

            if (columnIndex > 0)
            {
                sumOfElements += *(originalCurrentRowPointer + columnIndex - 1);
                totalElements++;
            }
            sumOfElements += *(originalCurrentRowPointer + columnIndex);
            totalElements++;

            if (columnIndex < sonarImageSize - 1)
            {
                sumOfElements += *(originalCurrentRowPointer + columnIndex + 1);
                totalElements++;
            }

            if (rowIndex < sonarImageSize - 1)
            {
                int *nextRowPtr = sonarImage + (rowIndex + 1) * sonarImageSize;
                if (columnIndex > 0)
                {
                    sumOfElements += *(nextRowPtr + columnIndex - 1);
                    totalElements++;
                }
                sumOfElements += *(nextRowPtr + columnIndex);
                totalElements++;
                if (columnIndex < sonarImageSize - 1)
                {
                    sumOfElements += *(nextRowPtr + columnIndex + 1);
                    totalElements++;
                }
            }

            *(sonarImage + rowIndex * sonarImageSize + columnIndex) = sumOfElements / totalElements;
        }

        for (int columnIndex = 0; columnIndex < sonarImageSize; columnIndex++)
        {
            *(originalPreviousRowPointer + columnIndex) = *(originalCurrentRowPointer + columnIndex);
        }
    }

    printSonarImageMatrix((int *)(sonarImage), sonarImageSize);
}
