#include <stdio.h>
#include <string.h>

#define ERROR 0
#define SUCCESS 1

typedef struct Student
{
    int rollNumber;
    char name[50];
    int sub1;
    int sub2;
    int sub3;
} Student;

int calcTotalMarks(Student student)
{
    int totalMarks = 0;
    totalMarks += student.sub1 + student.sub2 + student.sub3;
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
    if (grade == 'A')
    {
        printf("*****\n");
    }
    else if (grade == 'B')
    {
        printf("****\n");
    }
    else if (grade == 'C')
    {
        printf("***\n");
    }
    else if (grade == 'D')
    {
        printf("**\n");
    }
    else
    {
        printf("\n");
        return;
    }
}

void printRollNumbers(Student *studentDetails, int totalStudents)
{
    if (totalStudents == 0)
    {
        return;
    }

    printRollNumbers(studentDetails, totalStudents - 1);
    printf("%d ", studentDetails[totalStudents-1].rollNumber);
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

int isRollNumberExist(int rollNumber, int studentIdx, Student *studentDetails, int totalStudents)
{
    for (int i = 0; i < totalStudents; i++)
    {
        if (i != studentIdx && rollNumber == studentDetails[i].rollNumber)
        {
            return 1;
        }
    }

    return 0;
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

int isValidMarks(int marksOfSub1, int marksOfSub2, int marksOfSub3)
{
    if (((marksOfSub1 < 0 || marksOfSub1 > 100) || (marksOfSub2 < 0 || marksOfSub2 > 100) || (marksOfSub3 < 0 || marksOfSub3 > 100)))
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
        int marksOfSub1;
        int marksOfSub2;
        int marksOfSub3;
        char line[100];

        fgets(line, sizeof(line), stdin);
        sscanf(line, "%d %49[^0-9] %d %d %d", &rollNumber, name, &marksOfSub1, &marksOfSub2, &marksOfSub3);
        name[strcspn(name, "\n")] = '\0';

        if (isRollNumberExist(rollNumber, i, studentDetails, totalStudents) == 1)
        {
            printf("Roll number already exist.\n");
            return ERROR;
        }

        if (!isValidName(name))
        {
            printf("Name should only contain letters.");
            return ERROR;
        }

        if (!isValidMarks(marksOfSub1, marksOfSub2, marksOfSub3))
        {
            printf("Marks must be between 0 to 100.\n");
            return ERROR;
        }

        studentDetails[i].rollNumber = rollNumber;
        strcpy(studentDetails[i].name, name);
        studentDetails[i].sub1 = marksOfSub1;
        studentDetails[i].sub2 = marksOfSub2;
        studentDetails[i].sub3 = marksOfSub3;
    }

    printf("\n");
    printStudentDetails(studentDetails, totalStudents);

    printf("List of Roll Numbers(via recursion): ");
    printRollNumbers(studentDetails, totalStudents);

    return 0;
}
