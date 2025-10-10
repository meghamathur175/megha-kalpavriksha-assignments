#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define ERROR 0
#define SUCCESS 1

typedef struct Student
{
    int rollNumber;
    char name[50];
    int subject1;
    int subject2;
    int subject3;
} Student;

int calcTotalMarks(Student student);
float calcAverageMarks(int totalMarks);
char calcGrade(float averageMarks);
void calcPerformance(char grade);
void printRollNumbers(Student *studentDetails, int totalStudents);
void printStudentDetails(Student *studentDetails, int totalStudents);
int isValidRollNumber(int rollNumber);
int isRollNumberExist(int rollNumber, int studentIdx, Student *studentDetails);
int isValidName(char *name);
int isValidMarks(int marksOfsubject1, int marksOfsubject2, int marksOfsubject3);

int calcTotalMarks(Student student)
{
    int totalMarks = 0;
    totalMarks += student.subject1 + student.subject2 + student.subject3;
    return totalMarks;
}

float calcAverageMarks(int totalMarks)
{
    return totalMarks / 3.0;
}

char calcGrade(float averageMarks)
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

void calcPerformance(char grade)
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
        printf("\n");
        break;
    default:
        printf("Invalid grade");
    }

    for (int i = 0; i < starsToPrint; i++)
    {
        printf("*");
    }

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
    for (int i = 0; i < totalStudents; i++)
    {
        Student student = studentDetails[i];

        printf("Roll: %d\n", student.rollNumber);
        printf("Name: %s\n", student.name);

        int totalMarks = calcTotalMarks(student);
        printf("Total: %d\n", totalMarks);

        float averageMarks = calcAverageMarks(totalMarks);
        printf("Average: %.2f\n", averageMarks);

        char grade = calcGrade(averageMarks);
        printf("Grade: %c\n", grade);

        if (grade == 'F')
        {
            printf("\n");
            continue;
        }

        printf("Performance: ");
        calcPerformance(grade);
        printf("\n");
    }
}

int isRollNumberExist(int rollNumber, int studentIdx, Student *studentDetails)
{
    for (int i = 0; i < studentIdx; i++)
    {
        if (rollNumber == studentDetails[i].rollNumber)
        {
            return 1;
        }
    }

    return 0;
}

int isValidRollNumber(int rollNumber)
{
    return rollNumber > 0 ? SUCCESS : ERROR;
}

int isValidName(char *name)
{
    for (int i = 0; i < strlen(name); i++)
    {
        if (!((name[i] >= 'A' && name[i] <= 'Z') || (name[i] >= 'a' && name[i] <= 'z') || name[i] == ' '))
        {
            return ERROR;
        }
    }

    return SUCCESS;
}

int isValidMarks(int marksOfsubject1, int marksOfsubject2, int marksOfsubject3)
{
    if (((marksOfsubject1 < 0 || marksOfsubject1 > 100) || (marksOfsubject2 < 0 || marksOfsubject2 > 100) || (marksOfsubject3 < 0 || marksOfsubject3 > 100)))
    {
        return ERROR;
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
        return 0;
    }

    Student studentDetails[totalStudents];

    for (int i = 0; i < totalStudents; i++)
    {
        char name[50];
        int rollNumber;
        int marksOfsubject1;
        int marksOfsubject2;
        int marksOfsubject3;
        char line[100];

        fgets(line, sizeof(line), stdin);
        sscanf(line, "%d %49[^0-9] %d %d %d", &rollNumber, name, &marksOfsubject1, &marksOfsubject2, &marksOfsubject3);
        name[strcspn(name, "\n")] = '\0';

        if (isValidRollNumber(rollNumber))
        {
            if (isRollNumberExist(rollNumber, i, studentDetails) == 1)
            {
                printf("Roll number already exist.\n");
                return ERROR;
            }
        }
        else
        {
            printf("Roll number must be a numeric value.\n");
            return 0;
        }

        if (!isValidName(name))
        {
            printf("Name should only contain letters.");
            return ERROR;
        }

        if (!isValidMarks(marksOfsubject1, marksOfsubject2, marksOfsubject3))
        {
            printf("Marks must be between 0 to 100.\n");
            return ERROR;
        }

        studentDetails[i].rollNumber = rollNumber;
        strcpy(studentDetails[i].name, name);
        studentDetails[i].subject1 = marksOfsubject1;
        studentDetails[i].subject2 = marksOfsubject2;
        studentDetails[i].subject3 = marksOfsubject3;
    }

    printf("\n");
    printStudentDetails(studentDetails, totalStudents);

    printf("\n");
    printf("List of Roll Numbers(via recursion): ");
    printRollNumbers(studentDetails, totalStudents);
    printf("\n");

    return 0;
}
