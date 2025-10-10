#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ERROR 0
#define SUCCESS 1
#define TOTAL_SUBJECTS 3

typedef struct Student
{
    int rollNumber;
    char name[50];
    int marks[TOTAL_SUBJECTS];
} Student;

int calculateTotalMarks(Student student);
float calculateAverageMarks(int totalMarks);
char calculateGrade(float averageMarks);
void calculatePerformance(char grade);
void printRollNumbers(Student *studentDetails, int totalStudents);
void printStudentDetails(Student *studentDetails, int totalStudents);
int isValidRollNumber(char *rollNumber);
int isRollNumberExist(int rollNumber, int studentIdx, Student *studentDetails);
int isValidName(char *name);
int isValidMarks(int *marks);

int calculateTotalMarks(Student student)
{
    int totalMarks = 0;
    for (int i = 0; i < TOTAL_SUBJECTS; i++)
    {
        totalMarks += student.marks[i];
    }
    return totalMarks;
}

float calculateAverageMarks(int totalMarks)
{
    return totalMarks / (float)TOTAL_SUBJECTS;
}

char calculateGrade(float averageMarks)
{
    if (averageMarks >= 85)
    {
        return 'A';
    }
    else if (averageMarks >= 70)
    {
        return 'B';
    }
    else if (averageMarks >= 50)
    {
        return 'C';
    }
    else if (averageMarks >= 35)
    {
        return 'D';
    }
    else
    {
        return 'F';
    }
}

void calculatePerformance(char grade)
{
    int starsToPrint = 0;
    switch (grade)
    {
    case 'A':
        starsToPrint = 5;
        break;
    case 'B':
        starsToPrint = 4;
        break;
    case 'C':
        starsToPrint = 3;
        break;
    case 'D':
        starsToPrint = 2;
        break;
    case 'F':
        return;
    default:
        printf("Invalid grade");
    }

    for (int star = 0; star < starsToPrint; star++)
    {
        printf("*");
    }

    printf("\n");
    printf("\n");
}

void printRollNumbers(Student *studentDetails, int totalStudents)
{
    if (totalStudents == 0)
    {
        return;
    }

    printRollNumbers(studentDetails, totalStudents - 1);
    printf("%d ", studentDetails[totalStudents - 1].rollNumber);
}

void printStudentDetails(Student *studentDetails, int totalStudents)
{
    for (int studentIdx = 0; studentIdx < totalStudents; studentIdx++)
    {
        Student student = studentDetails[studentIdx];
        int totalMarks = calculateTotalMarks(student);
        float averageMarks = calculateAverageMarks(totalMarks);
        char grade = calculateGrade(averageMarks);

        printf("Roll: %d\n", student.rollNumber);
        printf("Name: %s\n", student.name);
        printf("Total: %d\n", totalMarks);
        printf("Average: %.2f\n", averageMarks);
        printf("Grade: %c\n", grade);

        if (grade == 'F')
        {
            printf("\n");
            continue;
        }

        printf("Performance: ");
        calculatePerformance(grade);
    }
}

int isRollNumberExist(int rollNumber, int studentIdx, Student *studentDetails)
{
    for (int currentIdx = 0; currentIdx < studentIdx; currentIdx++)
    {
        if (rollNumber == studentDetails[currentIdx].rollNumber)
        {
            return SUCCESS;
        }
    }

    return ERROR;
}

int isValidRollNumber(char *rollNumber)
{
    int numericRollNumber = atoi(rollNumber);

    for (int currentCharacter = 0; currentCharacter < strlen(rollNumber); currentCharacter++)
    {
        if (!isdigit((unsigned char)(rollNumber[currentCharacter])))
        {
            return ERROR;
        }
    }

    if (numericRollNumber < 0)
    {
        return ERROR;
    }

    return SUCCESS;
}

int isValidName(char *name)
{
    for (int currentChar = 0; currentChar < strlen(name); currentChar++)
    {
        if (!((name[currentChar] >= 'A' && name[currentChar] <= 'Z') || (name[currentChar] >= 'a' && name[currentChar] <= 'z') ||
              name[currentChar] == ' '))
        {
            return ERROR;
        }
    }

    return SUCCESS;
}

int isValidMarks(int *marks)
{
    for (int i = 0; i < TOTAL_SUBJECTS; i++)
    {
        if (marks[i] < 0 || marks[i] > 100)
        {
            return ERROR;
        }
    }

    return SUCCESS;
}

int main()
{
    int totalStudents;
    scanf("%d", &totalStudents);
    getchar();

    if (totalStudents <= 0 || totalStudents > 100)
    {
        printf("Invalid number of students. Must be between 1 and 100.\n");
        return ERROR;
    }

    Student studentDetails[totalStudents];

    for (int studentIdx = 0; studentIdx < totalStudents; studentIdx++)
    {
        char name[50];
        char rollNumberForInput[20];
        int rollNumber;
        int marks[TOTAL_SUBJECTS];
        char line[100];

        fgets(line, sizeof(line), stdin);
        sscanf(line, "%4s %49[^0-9] %d %d %d", rollNumberForInput, name, &marks[0], &marks[1], &marks[2]);
        name[strcspn(name, "\n")] = '\0';

        if (isValidRollNumber(rollNumberForInput))
        {
            rollNumber = atoi(rollNumberForInput);
            if (isRollNumberExist(rollNumber, studentIdx, studentDetails) == SUCCESS)
            {
                printf("Roll number already exist.\n");
                return ERROR;
            }
        }
        else
        {
            printf("Roll number must be a numeric value.\n");
            return ERROR;
        }

        if (!isValidName(name))
        {
            printf("Name should only contain letters.");
            return ERROR;
        }

        if (!isValidMarks(marks))
        {
            printf("Marks must be between 0 to 100.\n");
            return ERROR;
        }

        studentDetails[studentIdx].rollNumber = rollNumber;
        strcpy(studentDetails[studentIdx].name, name);
        for (int subjectIdx = 0; subjectIdx < TOTAL_SUBJECTS; subjectIdx++)
        {
            studentDetails[studentIdx].marks[subjectIdx] = marks[subjectIdx];
        }
    }

    printf("\n");
    printStudentDetails(studentDetails, totalStudents);
    printf("List of Roll Numbers(via recursion): ");
    printRollNumbers(studentDetails, totalStudents);
    printf("\n");

    return 0;
}
