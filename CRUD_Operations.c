#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "users.txt"

typedef struct
{
    int id;
    char name[50];
    int age;
} User;

// Function prototypes
void createFile();
void addUser();
void displayUsers();
void updateUser();
void deleteUser();
void clearInputBuffer();
int isValidName(char *name);

int main()
{
    int choice;
    int loopCondition = 1;
    createFile(); // ensure file exists

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
            // If non-numeric input, clear buffer
            printf("Invalid choice! Try again.\n");
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
            printf("Invalid choice! Try again.\n");
        }
    }

    return 0;
}

void clearInputBuffer()
{
    while (getchar() != '\n')
        ;
}

int isValidName(char *name)
{
    int hasLetter = 0;

    for (int i = 0; name[i] != '\0'; i++)
    {
        if ((!((name[i] >= 'A' && name[i] <= 'Z') ||
               (name[i] >= 'a' && name[i] <= 'z') ||
               name[i] == ' ')))
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
        printf("Error creating file!\n");
        return;
    }

    fclose(file);
}

void addUser()
{
    FILE *file = fopen(FILENAME, "a+");
    if (!file)
    {
        printf("Error opening file!\n");
        return;
    }

    User user;
    int idExist = 0;
    int validId = 0;
    User existingUser;
    char line[100];

    do
    {
        idExist = 0;
        validId = 0;

        printf("Enter ID: ");
        if (scanf("%d", &user.id) != 1)
        {
            printf("Invalid input. Enter a numeric ID.\n");
            clearInputBuffer();
            validId = 0;
            continue;
        }

        validId = 1;

        rewind(file);
        while (fgets(line, sizeof(line), file))
        {
            if (sscanf(line, "%d,%49[^,],%d", &existingUser.id, existingUser.name, &existingUser.age) == 3)
            {
                if (user.id == existingUser.id)
                {
                    idExist = 1;
                    break;
                }
            }
            else
            {
                printf("Warning: Skipping malformed line: %s\n", line);
            }
        }

        if (idExist)
        {
            printf("Error: A user with id %d already exist, Please use a unique ID.\n", user.id);
            clearInputBuffer();
        }
    } while (idExist == 1 || validId == 0);

    clearInputBuffer();

    do
    {
        printf("Enter Name: ");
        fgets(user.name, sizeof(user.name), stdin);
        user.name[strcspn(user.name, "\n")] = '\0';

        if (strlen(user.name) == 0 || !isValidName(user.name))
        {
            printf("Invalid name. Name must contain only letters and spaces.\n");
        }

    } while (strlen(user.name) == 0 || !isValidName(user.name));

    int validAge = 0;
    do
    {
        printf("Enter Age: ");

        if (scanf("%d", &user.age) != 1 || (user.age < 0 || user.age > 120))
        {
            printf("Invalid input. Age must be between 0 and 120.\n");
            clearInputBuffer();
            validAge = 0;
        }
        else
        {
            validAge = 1;
        }
    } while (validAge != 1);

    fprintf(file, "%d,%s,%d\n", user.id, user.name, user.age);
    fclose(file);
    printf("User added successfully!\n");
}

void displayUsers()
{
    FILE *file = fopen(FILENAME, "r");
    if (!file)
    {
        printf("No users found!\n");
        return;
    }

    User user;
    char line[100];

    printf("\nID\tName\t\tAge\n");
    printf("----------------------------\n");
    int hasData = 0;

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
    FILE *file = fopen(FILENAME, "r");
    FILE *temp = fopen("temp.txt", "w");

    if (!file || !temp)
    {
        printf("Error opening file!\n");
        return;
    }

    int id, found = 0;
    printf("Enter ID to update: ");
    scanf("%d", &id);

    User user;
    char line[100];

    while (fgets(line, sizeof(line), file))
    {
        if (sscanf(line, "%d,%49[^,],%d", &user.id, user.name, &user.age) == 3)
        {
            if (user.id == id)
            {
                do
                {
                    printf("Enter new Name: ");
                    clearInputBuffer();
                    fgets(user.name, sizeof(user.name), stdin);
                    user.name[strcspn(user.name, "\n")] = '\0';

                    if (strlen(user.name) == 0)
                    {
                        printf("Invalid name. Name cannot be empty.\n");
                    }

                } while (strlen(user.name) == 0);

                int validAge = 0;
                do
                {
                    printf("Enter new Age: ");

                    if (scanf("%d", &user.age) != 1 || (user.age < 0 || user.age > 120))
                    {
                        printf("Invalid input. Age must be between 0 and 120.\n");
                        clearInputBuffer();
                        validAge = 0;
                    }
                    else
                    {
                        validAge = 1;
                    }
                } while (validAge != 1);

                found = 1;
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
        printf("Error renaming file.\n");
        return;
    }

    if (found)
    {
        printf("User updated successfully!\n");
    }
    else
    {
        printf("User with ID %d not found!\n", id);
    }
}

void deleteUser()
{
    FILE *file = fopen(FILENAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!file || !temp)
    {
        printf("Error opening file!\n");
        return;
    }

    int id, found = 0;
    printf("Enter ID to delete: ");
    scanf("%d", &id);

    User user;
    char line[100];

    while (fgets(line, sizeof(line), file))
    {
        if (sscanf(line, "%d,%49[^,],%d", &user.id, user.name, &user.age) == 3)
        {
            if (user.id == id)
            {
                found = 1;
                continue; // skip writing this user
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
        printf("Error renaming file.\n");
        return;
    }

    if (found)
    {
        printf("User deleted successfully!\n");
    }
    else
    {
        printf("User with ID %d not found!\n", id);
    }
}
