#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "users.txt"

typedef struct {
    int id;
    char name[50];
    int age;
} User;

// Function prototypes
void checkFile();
void addUser();
void displayUsers();
void updateUser();
void deleteUser();

int main() {
    int choice;

    checkFile(); // ensure file exists

    while (1) {
        printf("\n--- MENU ---\n");
        printf("1. Add User\n");
        printf("2. Display Users\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            // If non-numeric input, clear buffer
            printf("Invalid choice! Try again.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
            case 1: addUser(); break;
            case 2: displayUsers(); break;
            case 3: updateUser(); break;
            case 4: deleteUser(); break;
            case 5: exit(0);
            default: printf("Invalid choice! Try again.\n");
        }
    }
    return 0;
}

// Ensure file exists
void checkFile() {
    FILE *file = fopen(FILENAME, "a+");
    if (!file) {
        printf("Error creating file!\n");
        exit(1);
    }
    fclose(file);
}

// Add a new user
void addUser() {
    FILE *file = fopen(FILENAME, "a");
    if (!file) {
        printf("Error opening file!\n");
        return;
    }

    User user;
    printf("Enter ID: ");
    scanf("%d", &user.id);

    printf("Enter Name: ");
    getchar(); // clear newline left by scanf
    fgets(user.name, sizeof(user.name), stdin);
    user.name[strcspn(user.name, "\n")] = '\0'; // remove newline

    printf("Enter Age: ");
    scanf("%d", &user.age);

    fprintf(file, "%d,%s,%d\n", user.id, user.name, user.age);
    fclose(file);
    printf("User added successfully!\n");
}

// Display all users
void displayUsers() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        printf("No users found!\n");
        return;
    }

    User user;
    char line[100];

    printf("\nID\tName\t\tAge\n");
    printf("----------------------------\n");
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d,%49[^,],%d", &user.id, user.name, &user.age) == 3) {
            printf("%d\t%-15s%d\n", user.id, user.name, user.age);
        }
    }
    fclose(file);
}

// Update a user
void updateUser() {
    FILE *file = fopen(FILENAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!file || !temp) {
        printf("Error opening file!\n");
        return;
    }

    int id, found = 0;
    printf("Enter ID to update: ");
    scanf("%d", &id);

    User user;
    char line[100];

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d,%49[^,],%d", &user.id, user.name, &user.age) == 3) {
            if (user.id == id) {
                printf("Enter new Name: ");
                getchar(); // clear leftover newline
                fgets(user.name, sizeof(user.name), stdin);
                user.name[strcspn(user.name, "\n")] = '\0';

                printf("Enter new Age: ");
                scanf("%d", &user.age);
                found = 1;
            }
            fprintf(temp, "%d,%s,%d\n", user.id, user.name, user.age);
        }
    }

    fclose(file);
    fclose(temp);
    remove(FILENAME);
    rename("temp.txt", FILENAME);

    if (found)
        printf("User updated successfully!\n");
    else
        printf("User with ID %d not found!\n", id);
}

// Delete a user
void deleteUser() {
    FILE *file = fopen(FILENAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!file || !temp) {
        printf("Error opening file!\n");
        return;
    }

    int id, found = 0;
    printf("Enter ID to delete: ");
    scanf("%d", &id);

    User user;
    char line[100];

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d,%49[^,],%d", &user.id, user.name, &user.age) == 3) {
            if (user.id == id) {
                found = 1;
                continue; // skip writing this user
            }
            fprintf(temp, "%d,%s,%d\n", user.id, user.name, user.age);
        }
    }

    fclose(file);
    fclose(temp);
    remove(FILENAME);
    rename("temp.txt", FILENAME);

    if (found)
        printf("User deleted successfully!\n");
    else
        printf("User with ID %d not found!\n", id);
}
