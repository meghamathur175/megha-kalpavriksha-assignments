#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_SIZE 50
#define BLOCK_SIZE 512
#define MAX_BLOCKS 1024
#define PRINT_WORKING_DIRECTORY_STACK_SIZE 50
#define SUCCESS 1
#define ERROR 0

typedef struct FreeBlock
{
    int index;
    struct FreeBlock *next;
    struct FreeBlock *previous;
} FreeBlock;

typedef struct FileNode
{
    char name[MAX_NAME_SIZE];
    struct FileNode *parent;
    struct FileNode *child;
    struct FileNode *next;
    struct FileNode *previous;
    int isDirectory;
    int sizeOfContent;
    int numberOfBlocksUsed;
    int *blockPointers;
} FileNode;

char virtualDisk[MAX_BLOCKS][BLOCK_SIZE];
FreeBlock *freeBlockHead = NULL;
FreeBlock *freeBlockTail = NULL;
FileNode *rootDirectory = NULL;
FileNode *currentWorkingDirectory = NULL;

void initializeFreeBlocks();
void initializeRootDirectory();
void initializeVirtualDisk();
void insertFileNodeIncurrentWorkingDirectory(FileNode *newNode);
void makeDirectory(char *directoryName);
void listCommand();
void changeDirectory(char *directoryName);
void printWorkingDirectory();
void createFile(char *fileName);
FileNode *searchChild(char *name);
void freeFileBlocks(FileNode *file);
FreeBlock *allocateFreeBlock();
void writeFile(char *fileName, char *data);
void readFile(char *fileName);
void deleteFile(char *fileName);
void removeDirectory(char *directoryName);
void displayDiskUsage();
void freeAllFileNodes(FileNode *directory);
void freeAllFreeBlocks();
void exitFileSystem();
void removeQuotesFromData(char *data);
void replaceNewLineCharacter(char *data);
int isValidName(char *name);
int isValidNameLength(char *name);
int validateNameArgument(char *name, char *command);

int main()
{
    char inputLine[1024];
    char command[100];
    char argument[100];
    char data[1024];

    initializeFreeBlocks();
    initializeRootDirectory();
    initializeVirtualDisk();
    currentWorkingDirectory = rootDirectory;

    printf("Compact VFS Ready. Type 'exit' to quit.\n");

    while (1)
    {
        if (currentWorkingDirectory == rootDirectory)
        {
            printf("/>");
        }
        else
        {
            printf("%s>", currentWorkingDirectory->name);
        }

        if (fgets(inputLine, sizeof(inputLine), stdin) == NULL)
        {
            continue;
        }

        inputLine[strcspn(inputLine, "\n")] = '\0';

        if (strlen(inputLine) == 0)
        {
            continue;
        }

        command[0] = argument[0] = data[0] = '\0';

        if (strncmp(inputLine, "write ", 6) == 0)
        {
            char *afterWritePointer = inputLine + 6;

            if (sscanf(afterWritePointer, "%s %[^\n]", argument, data) < 2)
            {
                printf("ERROR: write command requires filename and data. \n");
                continue;
            }

            if (!validateNameArgument(argument, "write "))
            {
                continue;
            }

            removeQuotesFromData(data);
            replaceNewLineCharacter(data);
            writeFile(argument, data);
            continue;
        }

        int scanned = sscanf(inputLine, "%s %s", command, argument);

        if (strcmp(command, "exit") == 0)
        {
            exitFileSystem();
            break;
        }
        else if (strcmp(command, "mkdir") == 0)
        {
            if (scanned < 2)
            {
                printf("ERROR: mkdir command requires directory name. \n");
                continue;
            }

            if (!validateNameArgument(argument, "mkdir"))
            {
                continue;
            }

            makeDirectory(argument);
        }
        else if (strcmp(command, "ls") == 0)
        {
            listCommand();
        }
        else if (strcmp(command, "create") == 0)
        {
            if (scanned < 2)
            {
                printf("ERROR: create command requires file name. \n");
                continue;
            }

            if (!validateNameArgument(argument, "create"))
            {
                continue;
            }

            createFile(argument);
        }
        else if (strcmp(command, "read") == 0)
        {
            if (scanned < 2)
            {
                printf("ERROR: read command requires file name. \n");
                continue;
            }

            if (!validateNameArgument(argument, "read"))
            {
                continue;
            }

            readFile(argument);
        }
        else if (strcmp(command, "delete") == 0)
        {
            if (scanned < 2)
            {
                printf("ERROR: delete command requires file name. \n");
                continue;
            }

            if (!validateNameArgument(argument, "delete"))
            {
                continue;
            }

            deleteFile(argument);
        }
        else if (strcmp(command, "rmdir") == 0)
        {
            if (scanned < 2)
            {
                printf("ERROR: rmdir command requires file name. \n");
                continue;
            }

            if (!validateNameArgument(argument, "rmdir"))
            {
                continue;
            }

            removeDirectory(argument);
        }
        else if (strcmp(command, "cd") == 0)
        {
            if (scanned < 2)
            {
                printf("ERROR: cd command requires directory name. \n");
                continue;
            }

            if (!validateNameArgument(argument, "cd"))
            {
                continue;
            }

            changeDirectory(argument);
        }
        else if (strcmp(command, "pwd") == 0)
        {
            printWorkingDirectory();
        }
        else if (strcmp(command, "df") == 0)
        {
            displayDiskUsage();
        }
        else
        {
            printf("Command is not valid. \n");
        }
    }

    return 0;
}

void removeQuotesFromData(char *data)
{
    int length = strlen(data);

    if (length >= 2 && data[0] == '"' && data[length - 1] == '"')
    {
        memmove(data, data + 1, length - 2);
        data[length - 2] = '\0';
    }
}

void replaceNewLineCharacter(char *data)
{
    char buffer[1024];
    int dataIndex = 0;
    int bufferIndex = 0;

    while (data[dataIndex] != '\0')
    {
        if (data[dataIndex] == '\\' && data[dataIndex + 1] == 'n')
        {
            buffer[bufferIndex++] = '\n';
            dataIndex += 2;
        }
        else
        {
            buffer[bufferIndex++] = data[dataIndex++];
        }
    }
    buffer[bufferIndex] = '\0';
    strcpy(data, buffer);
}

int isValidName(char *name)
{
    char *namePointer = name;

    while (*namePointer == ' ')
    {
        namePointer++;
    }

    if (*namePointer == '\0')
    {
        printf("Invalid name. Name cannot be empty. \n");
        return ERROR;
    }

    return SUCCESS;
}

int isValidNameLength(char *name)
{
    if (strlen(name) > 50)
    {
        return ERROR;
    }

    return SUCCESS;
}

int validateNameArgument(char *name, char *command)
{
    if (!isValidName(name))
    {
        printf("Error: %s requires a name argument. \n", command);
        return ERROR;
    }

    if (!isValidNameLength(name))
    {
        printf("Error: Name too long (max 50 chars). \n");
        return ERROR;
    }

    return SUCCESS;
}

void initializeFreeBlocks()
{
    freeBlockHead = freeBlockTail = NULL;

    for (int currentBlock = 0; currentBlock < MAX_BLOCKS; currentBlock++)
    {
        FreeBlock *newBlock = (FreeBlock *)calloc(1, sizeof(FreeBlock));
        newBlock->index = currentBlock;
        newBlock->next = NULL;
        newBlock->previous = NULL;

        if (freeBlockHead == NULL)
        {
            freeBlockHead = freeBlockTail = newBlock;
        }
        else
        {
            freeBlockTail->next = newBlock;
            newBlock->previous = freeBlockTail;
            freeBlockTail = newBlock;
        }
    }
}

void initializeRootDirectory()
{
    rootDirectory = (FileNode *)calloc(1, sizeof(FileNode));
    strcpy(rootDirectory->name, "/");
    rootDirectory->isDirectory = 1;
    rootDirectory->parent = NULL;
    rootDirectory->child = NULL;
    rootDirectory->next = NULL;
    rootDirectory->previous = NULL;
    rootDirectory->sizeOfContent = 0;
    rootDirectory->numberOfBlocksUsed = 0;
    rootDirectory->blockPointers = NULL;
}

void initializeVirtualDisk()
{
    for (int currentBlock = 0; currentBlock < MAX_BLOCKS; currentBlock++)
    {
        for (int currentBlockSize = 0; currentBlockSize < BLOCK_SIZE; currentBlockSize++)
        {
            *(*(virtualDisk + currentBlock) + currentBlockSize) = '\0';
        }
    }
}

void insertFileNodeIncurrentWorkingDirectory(FileNode *newNode)
{
    if (currentWorkingDirectory->child == NULL)
    {
        currentWorkingDirectory->child = newNode;
        newNode->next = newNode;
        newNode->previous = newNode;
    }
    else
    {
        FileNode *lastChild = currentWorkingDirectory->child->previous;

        lastChild->next = newNode;
        newNode->previous = lastChild;
        newNode->next = currentWorkingDirectory->child;
        currentWorkingDirectory->child->previous = newNode;
    }

    newNode->parent = currentWorkingDirectory;
}

FileNode *searchChild(char *name)
{
    FileNode *currentChild = currentWorkingDirectory->child;

    if (currentChild != NULL)
    {
        do
        {
            if (strcmp(currentChild->name, name) == 0)
            {
                return currentChild;
            }

            currentChild = currentChild->next;
        } while (currentChild != currentWorkingDirectory->child);
    }

    return NULL;
}

void makeDirectory(char *directoryName)
{
    FileNode *currentChild = currentWorkingDirectory->child;

    if (searchChild(directoryName) != NULL)
    {
        printf("Name already exists in current directory. \n");
        return;
    }

    FileNode *newDirectory = (FileNode *)calloc(1, sizeof(FileNode));
    strcpy(newDirectory->name, directoryName);
    newDirectory->isDirectory = 1;
    newDirectory->child = NULL;
    newDirectory->sizeOfContent = 0;
    newDirectory->numberOfBlocksUsed = 0;
    newDirectory->blockPointers = NULL;

    insertFileNodeIncurrentWorkingDirectory(newDirectory);

    printf("Directory '%s' created successfully.\n", directoryName);
}

void listCommand()
{
    FileNode *currentChild = currentWorkingDirectory->child;

    if (currentChild == NULL)
    {
        printf("(empty) \n");
        return;
    }

    do
    {
        if (currentChild->isDirectory)
        {
            printf("%s/ \n", currentChild->name);
        }
        else
        {
            printf("%s \n", currentChild->name);
        }
        currentChild = currentChild->next;
    } while (currentChild != currentWorkingDirectory->child);
}

void changeDirectory(char *directoryName)
{
    if (strcmp(directoryName, "..") == 0)
    {
        if (currentWorkingDirectory->parent == NULL)
        {
            printf("Already at root directory. \n");
            return;
        }
        else
        {
            currentWorkingDirectory = currentWorkingDirectory->parent;

            if (currentWorkingDirectory == rootDirectory)
            {
                printf("Moved to /\n", currentWorkingDirectory->name);
            }
            else
            {
                printf("Moved to ");
                printWorkingDirectory();
            }

            return;
        }
    }

    FileNode *currentChild = currentWorkingDirectory->child;

    if (currentChild == NULL)
    {
        printf("No subdirectories found. \n");
        return;
    }

    do
    {
        if (currentChild->isDirectory == 1 && strcmp(currentChild->name, directoryName) == 0)
        {
            currentWorkingDirectory = currentChild;
            printf("Moved to ");
            printWorkingDirectory();
            return;
        }

        currentChild = currentChild->next;
    } while (currentChild != currentWorkingDirectory->child);

    printf("Directory not found. \n");
}

void printWorkingDirectory()
{
    FileNode *currentDirectoryPointer = currentWorkingDirectory;

    if (currentDirectoryPointer->parent == NULL)
    {
        printf("/\n");
        return;
    }

    char stack[PRINT_WORKING_DIRECTORY_STACK_SIZE][MAX_NAME_SIZE];
    int top = -1;

    while (currentDirectoryPointer != NULL)
    {
        if (currentDirectoryPointer->parent != NULL)
        {
            strcpy(stack[++top], currentDirectoryPointer->name);
        }

        currentDirectoryPointer = currentDirectoryPointer->parent;
    }

    printf("/");

    for (int currentDirectoryName = top; currentDirectoryName >= 0; currentDirectoryName--)
    {
        printf("%s", stack[currentDirectoryName]);

        if (currentDirectoryName > 0)
        {
            printf("/");
        }
    }

    printf("\n");
}

void createFile(char *fileName)
{
    FileNode *currentChild = currentWorkingDirectory->child;

    if (searchChild(fileName) != NULL)
    {
        printf("Name already exists in current directory. \n");
        return;
    }

    FileNode *newFile = (FileNode *)calloc(1, sizeof(FileNode));

    if (!newFile)
    {
        printf("Memory Allocation Failed. \n");
        return;
    }

    strncpy(newFile->name, fileName, MAX_NAME_SIZE);
    newFile->name[MAX_NAME_SIZE - 1] = '\0';
    newFile->isDirectory = 0;
    newFile->child = NULL;
    newFile->sizeOfContent = 0;
    newFile->numberOfBlocksUsed = 0;
    newFile->blockPointers = NULL;

    insertFileNodeIncurrentWorkingDirectory(newFile);

    printf("File '%s' is created successfully. \n", fileName);
}

void freeFileBlocks(FileNode *file)
{
    if (file->blockPointers == NULL)
    {
        return;
    }

    for (int currentBlock = 0; currentBlock < file->numberOfBlocksUsed; currentBlock++)
    {
        int currentBlockIndex = file->blockPointers[currentBlock];
        memset(virtualDisk[currentBlockIndex], 0, BLOCK_SIZE);

        FreeBlock *newBlock = (FreeBlock *)calloc(1, sizeof(FreeBlock));
        newBlock->index = currentBlockIndex;
        newBlock->next = NULL;
        newBlock->previous = freeBlockTail;

        if (freeBlockHead == NULL)
        {
            freeBlockHead = freeBlockTail = newBlock;
        }
        else
        {
            freeBlockTail->next = newBlock;
            newBlock->previous = freeBlockTail;
            freeBlockTail = newBlock;
        }
    }

    free(file->blockPointers);
    file->blockPointers = NULL;
    file->numberOfBlocksUsed = 0;
    file->sizeOfContent = 0;
}

FreeBlock *allocateFreeBlock()
{
    FreeBlock *allocatedBlock = freeBlockHead;

    if (freeBlockHead == NULL)
    {
        printf("No free blocks available. \n");
        return NULL;
    }

    if (freeBlockHead->next == NULL)
    {
        freeBlockHead = freeBlockTail = NULL;
    }
    else
    {
        freeBlockHead = freeBlockHead->next;
        freeBlockHead->previous = NULL;
    }

    allocatedBlock->next = NULL;
    allocatedBlock->previous = NULL;

    return allocatedBlock;
}

void writeFile(char *fileName, char *data)
{
    FileNode *file = searchChild(fileName);
    int dataSize = strlen(data);
    int requiredBlocks = (dataSize + BLOCK_SIZE - 1) / BLOCK_SIZE;
    int allocatedBlockCount = 0;
    int currentDataPosition = 0;

    if (file == NULL)
    {
        printf("File not found. \n", fileName);
        return;
    }

    if (file->isDirectory)
    {
        printf("'%s' is not a file. It's a directory \n", fileName);
        return;
    }

    freeFileBlocks(file);

    if (requiredBlocks == 0)
    {
        freeFileBlocks(file);
        file->sizeOfContent = 0;
        printf("File '%s' have 0 bytes. \n", fileName);
        return;
    }

    int *newBlockIndexes = calloc(requiredBlocks, sizeof(int));

    if (!newBlockIndexes)
    {
        printf("Memory allocation failed.\n");
        return;
    }

    FreeBlock **allocatedBlocks = calloc(requiredBlocks, sizeof(FreeBlock *));

    for (int currentBlock = 0; currentBlock < requiredBlocks; currentBlock++)
    {
        FreeBlock *newBlock = allocateFreeBlock();

        if (newBlock == NULL)
        {
            break;
        }

        allocatedBlocks[currentBlock] = newBlock;
        newBlockIndexes[currentBlock] = newBlock->index;
        allocatedBlockCount++;
    }

    if (allocatedBlockCount < requiredBlocks)
    {
        for (int currentBlock = 0; currentBlock < allocatedBlockCount; currentBlock++)
        {
            FreeBlock *block = allocatedBlocks[currentBlock];
            block->next = NULL;
            block->previous = freeBlockTail;

            if (freeBlockTail)
            {
                freeBlockTail->next = block;
            }
            else
            {
                freeBlockHead = block;
            }

            freeBlockTail = block;
        }

        free(newBlockIndexes);
        free(allocatedBlocks);
        printf("Not enough space available. Write failed. \n");
        return;
    }

    for (int currentBlock = 0; currentBlock < requiredBlocks; currentBlock++)
    {
        int blockIndex = newBlockIndexes[currentBlock];
        int dataBytesToWrite = 0;

        if ((dataSize - currentDataPosition) < BLOCK_SIZE)
        {
            dataBytesToWrite = dataSize - currentDataPosition;
        }
        else
        {
            dataBytesToWrite = BLOCK_SIZE;
        }

        memset(virtualDisk[blockIndex], 0, BLOCK_SIZE);
        memcpy(virtualDisk[blockIndex], data + currentDataPosition, dataBytesToWrite);
        currentDataPosition += dataBytesToWrite;
    }

    file->blockPointers = newBlockIndexes;
    file->numberOfBlocksUsed = requiredBlocks;
    file->sizeOfContent = dataSize;
    free(allocatedBlocks);

    printf("Data written successfully (Size=%d bytes)\n", dataSize);
}

void readFile(char *fileName)
{
    FileNode *file = searchChild(fileName);

    if (file == NULL)
    {
        printf("File not found. \n", fileName);
        return;
    }

    if (file->isDirectory)
    {
        printf("'%s' is a directory. Cannot read. \n", fileName);
        return;
    }

    if (file->numberOfBlocksUsed == 0 || file->sizeOfContent == 0)
    {
        printf("(empty) \n");
        return;
    }

    char *fileContent = calloc(file->sizeOfContent + 1, sizeof(char));
    int currentDataPosition = 0;

    if (!fileContent)
    {
        printf("Memory allocation failed. \n");
        return;
    }

    for (int currentBlock = 0; currentBlock < file->numberOfBlocksUsed; currentBlock++)
    {
        int blockIndex = file->blockPointers[currentBlock];
        int dataBytesToRead = 0;

        if ((file->sizeOfContent - currentDataPosition) < BLOCK_SIZE)
        {
            dataBytesToRead = file->sizeOfContent - currentDataPosition;
        }
        else
        {
            dataBytesToRead = BLOCK_SIZE;
        }

        memcpy(fileContent + currentDataPosition, virtualDisk[blockIndex], dataBytesToRead);
        currentDataPosition += dataBytesToRead;
    }

    fileContent[file->sizeOfContent] = '\0';
    printf("%s \n", fileContent);
    free(fileContent);
}

void deleteFile(char *fileName)
{
    FileNode *file = searchChild(fileName);

    if (file == NULL)
    {
        printf("File '%s' not found.\n", fileName);
        return;
    }

    if (file->isDirectory)
    {
        printf("'%s' is a directory. Use rmdir to delete directories.\n", fileName);
        return;
    }

    freeFileBlocks(file);

    if (file->next == file)
    {
        currentWorkingDirectory->child = NULL;
    }
    else
    {
        if (currentWorkingDirectory->child == file)
        {
            currentWorkingDirectory->child = file->next;
        }

        file->previous->next = file->next;
        file->next->previous = file->previous;
    }

    free(file);
    printf("File deleted successfully.\n");
}

void removeDirectory(char *directoryName)
{
    FileNode *directory = searchChild(directoryName);

    if (directory == NULL)
    {
        printf("Directory not found.\n", directoryName);
        return;
    }

    if (!directory->isDirectory)
    {
        printf("%s is not a directory. \n", directoryName);
        return;
    }

    if (directory->child != NULL)
    {
        printf("Directory not empty. Remove files first. \n");
        return;
    }

    if (directory->next == directory)
    {
        currentWorkingDirectory->child = NULL;
    }
    else
    {
        if (currentWorkingDirectory->child == directory)
        {
            currentWorkingDirectory->child = directory->next;
        }

        directory->previous->next = directory->next;
        directory->next->previous = directory->previous;
    }

    free(directory);
    printf("Directory removed successfully.\n");
}

void displayDiskUsage()
{
    int totalBlocks = MAX_BLOCKS;
    int totalFreeBlocks = 0;
    FreeBlock *currentBlock = freeBlockHead;

    while (currentBlock != NULL)
    {
        totalFreeBlocks++;
        currentBlock = currentBlock->next;
    }

    int usedBlocks = totalBlocks - totalFreeBlocks;
    double diskUsages = (double)usedBlocks / totalBlocks * 100;

    printf("Total Blocks: %d\n", totalBlocks);
    printf("Used Blocks: %d\n", usedBlocks);
    printf("Free Blocks: %d\n", totalFreeBlocks);
    printf("Disk Usage: %.2f%%\n", diskUsages);
}

void freeAllFileNodes(FileNode *directory)
{
    if (!directory)
    {
        return;
    }

    FileNode *child = directory->child;

    if (child == NULL)
    {
        return;
    }

    FileNode *currentChild = child;

    do
    {
        FileNode *nextNode = currentChild->next;

        if (currentChild->isDirectory)
        {
            freeAllFileNodes(currentChild);
        }
        else
        {
            freeFileBlocks(currentChild);
        }

        free(currentChild);
        currentChild = nextNode;
    } while (currentChild != child);
}

void freeAllFreeBlocks()
{
    FreeBlock *currentBlock = freeBlockHead;

    while (currentBlock != NULL)
    {
        FreeBlock *nextBlock = currentBlock->next;
        free(currentBlock);
        currentBlock = nextBlock;
    }

    freeBlockHead = freeBlockTail = NULL;
}

void exitFileSystem()
{
    freeAllFileNodes(rootDirectory);
    freeAllFreeBlocks();

    printf("Memory released. Exiting program...");
}
