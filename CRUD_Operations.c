#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "user.txt"
#define SUCCESS 1
#define ERROR 0

typedef struct User
{
    int id;
    char name[50];
    int age;
} User;

int inputUserId();
int isUserIdExist(int userId);
int isValidName(char *name);
void inputUserName(char *userName, int userNameSize);
int inputUserAge();
void createFile();
void addUser();
void displayUsers();
void updateUser();
void deleteUser();
void clearInputBuffer();

int main()
{
    int choice;
    int loopCondition = 1;
    createFile();

    while (loopCondition)
    {
        printf("\n--- MENU ---\n");
        printf("1. Add User\n");
        printf("2. Display Users\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
            fprintf(stderr, "Invalid choice! Try again.\n");
            while (getchar() != '\n')
                ;
            continue;
        }

        switch (choice)
        {
        case 1:
            addUser();
            break;
        case 2:
            displayUsers();
            break;
        case 3:
            updateUser();
            break;
        case 4:
            deleteUser();
            break;
        case 5:
            printf("Exiting...\n");
            loopCondition = 0;
            break;
        default:
            fprintf(stderr, "Invalid choice! Try again.\n");
        }
    }

    return 0;
}

void clearInputBuffer()
{
    while (getchar() != '\n')
        ;
}

int inputUserId()
{
    int userId;

    do
    {
        printf("Enter ID: ");
        if (scanf("%d", &userId) != 1)
        {
            fprintf(stderr, "Invalid input. Enter a numeric ID.\n");
            clearInputBuffer();
            continue;
        }

        clearInputBuffer();

        if (isUserIdExist(userId) == SUCCESS)
        {
            fprintf(stderr, "Error: A user with id %d already exist, Please enter a unique ID.\n", userId);
            continue;
        }

        break;

    } while (1);

    return userId;
}

int isUserIdExist(int userId)
{
    User existingUser;
    char line[100];
    int isDuplicateId = 0;

    FILE *file = fopen(FILENAME, "r");
    if (!file)
    {
        fprintf(stderr, "Error opening file!\n");
        return ERROR;
    }

    rewind(file);
    while (fgets(line, sizeof(line), file))
    {
        if (sscanf(line, "%d,%49[^,],%d", &existingUser.id, existingUser.name, &existingUser.age) == 3)
        {
            if (userId == existingUser.id)
            {
                isDuplicateId = 1;
                break;
            }
        }
        else
        {
            printf("Warning: Skipping malformed line: %s\n", line);
        }
    }

    fclose(file);

    if (isDuplicateId)
    {
        return SUCCESS;
    }

    return ERROR;
}

void inputUserName(char *userName, int userNameSize)
{
    do
    {
        printf("Enter Name: ");
        fgets(userName, userNameSize, stdin);
        userName[strcspn(userName, "\n")] = '\0';

        if (strlen(userName) == 0 || !isValidName(userName))
        {
            fprintf(stderr, "Invalid name. Name must contain only letters and spaces.\n");
        }

    } while (strlen(userName) == 0 || !isValidName(userName));
}

int inputUserAge()
{
    int userAge;
    int isValidAge = 0;

    do
    {
        printf("Enter Age: ");

        if (scanf("%d", &userAge) != 1 || (userAge < 0 || userAge > 120))
        {
            fprintf(stderr, "Invalid input. Age must be between 0 and 120.\n");
            clearInputBuffer();
            isValidAge = 0;
        }
        else
        {
            isValidAge = 1;
        }
    } while (isValidAge != 1);

    return userAge;
}

int isValidName(char *name)
{
    int hasLetter = 0;

    for (int i = 0; name[i] != '\0'; i++)
    {
        if ((!((name[i] >= 'A' && name[i] <= 'Z') || (name[i] >= 'a' && name[i] <= 'z') || name[i] == ' ')))
        {
            return 0;
        }
        else
        {
            hasLetter = 1;
        }
    }

    return hasLetter;
}

void createFile()
{
    FILE *file = fopen(FILENAME, "a+");
    if (!file)
    {
        fprintf(stderr, "Error creating file!\n");
        return;
    }

    fclose(file);
}

void addUser()
{
    User user;
    FILE *file = fopen(FILENAME, "a+");
    if (!file)
    {
        fprintf(stderr, "Error opening file!\n");
        return;
    }

    // Id input
    user.id = inputUserId();
    inputUserName(user.name, sizeof(user.name));
    user.age = inputUserAge();
    clearInputBuffer();

    fprintf(file, "%d,%s,%d\n", user.id, user.name, user.age);
    fclose(file);
    printf("User added successfully!\n");
}

void displayUsers()
{
    User user;
    char line[100];
    int hasData = 0;

    FILE *file = fopen(FILENAME, "r");
    if (!file)
    {
        printf("No users found!\n");
        return;
    }

    printf("\nID\tName\t\tAge\n");
    printf("----------------------------\n");

    while (fgets(line, sizeof(line), file))
    {
        if (sscanf(line, "%d,%49[^,],%d", &user.id, user.name, &user.age) == 3)
        {
            printf("%d\t%-15s%d\n", user.id, user.name, user.age);
            hasData = 1;
        }
        else
        {
            printf("Warning: Skipping malformed line: %s\n", line);
        }
    }

    if (hasData == 0)
    {
        printf("No user found.\n");
    }

    fclose(file);
}

void updateUser()
{
    int id;
    User user;
    char line[100];

    printf("Enter ID to update: ");
    scanf("%d", &id);
    clearInputBuffer();

    if (!isUserIdExist(id))
    {
        printf("User not found.\n");
        return;
    }

    FILE *file = fopen(FILENAME, "r");
    FILE *temp = fopen("temp.txt", "w");

    if (!file || !temp)
    {
        fprintf(stderr, "Error opening file!\n");
        return;
    }

    while (fgets(line, sizeof(line), file))
    {
        if (sscanf(line, "%d,%49[^,],%d", &user.id, user.name, &user.age) == 3)
        {
            if (user.id == id)
            {
                inputUserName(user.name, sizeof(user.name));
                user.age = inputUserAge();
            }

            fprintf(temp, "%d,%s,%d\n", user.id, user.name, user.age);
        }
        else
        {
            printf("Warning: Skipping malformed line: %s\n", line);
        }
    }

    fclose(file);
    fclose(temp);

    if (remove(FILENAME) != 0)
    {
        fprintf(stderr, "Error deleting file.\n");
        return;
    }

    if (rename("temp.txt", FILENAME) != 0)
    {
        fprintf(stderr, "Error renaming file.\n");
        return;
    }

    printf("User updated successfully.\n");
}

void deleteUser()
{
    int id;
    int found = 0;
    User user;
    char line[100];

    FILE *file = fopen(FILENAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!file || !temp)
    {
        fprintf(stderr, "Error opening file!\n");
        return;
    }

    printf("Enter ID to delete: ");
    scanf("%d", &id);
    clearInputBuffer();

    if (!isUserIdExist(id))
    {
        printf("User not found.\n");
        return;
    }

    while (fgets(line, sizeof(line), file))
    {
        if (sscanf(line, "%d,%49[^,],%d", &user.id, user.name, &user.age) == 3)
        {
            if (user.id == id)
            {
                found = 1;
                continue;
            }
            fprintf(temp, "%d,%s,%d\n", user.id, user.name, user.age);
        }
        else
        {
            printf("Warning: Skipping malformed line: %s\n", line);
        }
    }

    fclose(file);
    fclose(temp);

    if (remove(FILENAME) != 0)
    {
        printf("Error deleting file.\n");
        return;
    }

    if (rename("temp.txt", FILENAME) != 0)
    {
        fprintf(stderr, "Error renaming file.\n");
        return;
    }

    if (found)
    {
        printf("User deleted successfully!\n");
    }
    else
    {
        printf("User with ID %d not found! \n", id);
    }
}
