#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MAX_HASHMAP_SIZE 100
#define MAX_NAME_LENGTH 50
#define MAX_INPUT_LENGTH 100
#define MAX_INTEGER_LENGTH 5
#define MAX_PCB_NODES 100
#define COUNT_OF_KILL_EVENT_INPUT_VALUES 2
#define COUNT_OF_PROCESS_INPUT_VALUES 5

typedef enum
{
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED,
    KILLED
} State;

typedef struct PCBNode
{
    int processId;
    char processName[MAX_NAME_LENGTH];
    int arrivalTime;
    int burstTime;
    int remainingBurstTime;
    int currentBurstTime;
    int inputOutputStartTime;
    int inputOutputDurationTime;
    int currentInputOutputTime;
    int remainingInputOutputTime;
    int completionTime;
    struct PCBNode *next;
    State state;
    int executionTime;
    int inputOutputJustStartedFlag;
    bool isKilled;
} PCBNode;

typedef struct QueueNode
{
    PCBNode *pcb;
    struct QueueNode *next;
} QueueNode;

typedef struct Queue
{
    QueueNode *front;
    QueueNode *rear;
    int sizeOfQueue;
} Queue;

typedef struct KILLNode
{
    int processId;
    int KillTime;
    struct KILLNode *next;
} KILLNode;

typedef struct ProcessInput
{
    char processName[MAX_NAME_LENGTH];
    char processId[MAX_INTEGER_LENGTH];
    char burstTime[MAX_INTEGER_LENGTH];
    char inputOutputStartTime[MAX_INTEGER_LENGTH];
    char inputOutputDuration[MAX_INTEGER_LENGTH];
} ProcessInput;

typedef struct SystemQueues
{
    Queue *readyQueue;
    Queue *waitingQueue;
    Queue *terminatedQueue;
} SystemQueues;

PCBNode *hashmap[MAX_HASHMAP_SIZE];
KILLNode *KILLHead = NULL;

void initializeQueue(Queue *queue);
void initializeHashmap();
void enqueue(Queue *queue, PCBNode *pcb);
PCBNode *dequeue(Queue *queue);
int hashFunction(int processId);
void hashmapPut(PCBNode *pcb);
PCBNode *hashmapGet(int processId);
PCBNode *createPCB(char *processName, int processId, int burstTime, int inputOutputStartTime, int inputOutputDurationTime);
int removeFromQueue(Queue *queue, int processId);
void trimWhitespaces(char *inputLine);
void takeUserInput(Queue *readyQueue);
void addKillEvent(int processId, int time);
bool validateProcessInputValues(ProcessInput processInputatedValues);
bool isValidProcessName(char processName[MAX_NAME_LENGTH]);
bool isValidProcessId(char inputedProcessId[MAX_INTEGER_LENGTH]);
bool isValidInteger(char integerValue[MAX_INTEGER_LENGTH]);
bool isValidBurstTime(char inputedtBurstTime[MAX_INTEGER_LENGTH]);
bool isValidInputOutputStartTime(char *inputedInputOutputStartTime);
bool isValidInputOutputDuration(char *inputedInputOutputDuration);
bool validateKillEventInputValues(char inputedProcessId[MAX_INTEGER_LENGTH], char inputedTime[MAX_INTEGER_LENGTH]);
bool isDash(char *inputValue);
void processKillEvent(int currentTime, SystemQueues *systemQueues, PCBNode **running);
void handleInputOutputTime(Queue *waiting, Queue *ready);
void handleSchedulingOfProcesses(SystemQueues *systemQueues);
void sortPCBBasedOnProcessId(PCBNode **allTerminatedPCBs, int totalTerminatedPCBs);
bool hasKillEvent(PCBNode **allTerminatedPCBs, int totalTerminatedPCBs);
int countTotalTerminatedPCB(Queue *terminatedQueue);
void printFinalReport(Queue *terminated);
void freeQueueNodes(Queue *queue);
void freeKillNodes();
void freeHashmap();
void freeMemory(SystemQueues *systemQueues);

int main()
{
    initializeHashmap();

    Queue readyQueue;
    Queue waitingQueue;
    Queue terminatedQueue;

    initializeQueue(&readyQueue);
    initializeQueue(&waitingQueue);
    initializeQueue(&terminatedQueue);

    takeUserInput(&readyQueue);

    SystemQueues systemQueues;
    systemQueues.readyQueue = &readyQueue;
    systemQueues.waitingQueue = &waitingQueue;
    systemQueues.terminatedQueue = &terminatedQueue;

    handleSchedulingOfProcesses(&systemQueues);
    printFinalReport(&terminatedQueue);
    freeMemory(&systemQueues);

    return 0;
}

void trimWhitespaces(char *inputLine)
{
    char *startIndex = inputLine;

    while (*startIndex != '\0' && isspace((unsigned char)(*startIndex)))
    {
        startIndex++;
    }

    if (startIndex != inputLine)
    {
        memmove(inputLine, startIndex, strlen(startIndex) + 1);
    }

    int length = strlen(inputLine) - 1;

    while (length > 0 && isspace((unsigned char)(inputLine[length])))
    {
        inputLine[length] = '\0';
        length--;
    }
}

void takeUserInput(Queue *readyQueue)
{
    char processName[MAX_NAME_LENGTH];
    char inputedProcessId[MAX_INTEGER_LENGTH];
    int processId;
    char inputedtBurstTime[MAX_INTEGER_LENGTH];
    int burstTime;
    char inputedInputOutputStartTime[MAX_INTEGER_LENGTH];
    int inputOutputStartTime;
    char inputedInputOutputDuration[MAX_INTEGER_LENGTH];
    int inputOutputDuration;
    char inputedTime[MAX_INTEGER_LENGTH];
    int time;

    while (1)
    {
        char inputLine[MAX_INPUT_LENGTH];

        if (fgets(inputLine, sizeof(inputLine), stdin) == NULL)
        {
            break;
        }

        inputLine[strcspn(inputLine, "\n")] = '\0';
        trimWhitespaces(inputLine);

        if (strlen(inputLine) == 0)
        {
            break;
        }

        if (sscanf(inputLine, "KILL %s %s", inputedProcessId, inputedTime) == COUNT_OF_KILL_EVENT_INPUT_VALUES)
        {
            if (validateKillEventInputValues(inputedProcessId, inputedTime) == false)
            {
                continue;
            }

            processId = atoi(inputedProcessId);
            time = atoi(inputedTime);

            addKillEvent(processId, time);
        }
        else if (sscanf(inputLine, "%s %s %s %s %s", processName, inputedProcessId, inputedtBurstTime, inputedInputOutputStartTime, inputedInputOutputDuration) == COUNT_OF_PROCESS_INPUT_VALUES)
        {
            ProcessInput inputedProcessValues;

            strcpy(inputedProcessValues.processId, inputedProcessId);
            strcpy(inputedProcessValues.processName, processName);
            strcpy(inputedProcessValues.burstTime, inputedtBurstTime);
            strcpy(inputedProcessValues.inputOutputStartTime, inputedInputOutputStartTime);
            strcpy(inputedProcessValues.inputOutputDuration, inputedInputOutputDuration);

            if (validateProcessInputValues(inputedProcessValues) == false)
            {
                continue;
            }

            processId = atoi(inputedProcessId);
            burstTime = atoi(inputedtBurstTime);

            if (strcmp(inputedInputOutputStartTime, "-") == 0)
            {
                inputOutputStartTime = -1;
            }
            else
            {
                inputOutputStartTime = atoi(inputedInputOutputStartTime);
            }

            if (strcmp(inputedInputOutputDuration, "-") == 0)
            {
                inputOutputDuration = 0;
            }
            else
            {
                inputOutputDuration = atoi(inputedInputOutputDuration);
            }

            PCBNode *newPcb = createPCB(processName, processId, burstTime, inputOutputStartTime, inputOutputDuration);

            if (newPcb == NULL)
            {
                return;
            }

            newPcb->state = READY;
            newPcb->remainingBurstTime = burstTime;
            hashmapPut(newPcb);
            enqueue(readyQueue, newPcb);
        }
        else
        {
            printf("Invalid input.\n");
            printf("Process input format must be: <process_name> <pid> <burst_time> <io_start_time> <io_duration> \n");
            printf("Kill event format must be: KILL <PID> <kill_time> \n");

            continue;
        }
    }
}

bool isDash(char *inputValue)
{
    if (strcmp(inputValue, "-") == 0)
    {
        return true;
    }

    return false;
}

bool isValidProcessName(char processName[MAX_NAME_LENGTH])
{
    int currentNameIndex = 0;

    while (processName[currentNameIndex] == ' ')
    {
        currentNameIndex++;
    }

    if (processName[currentNameIndex] == '\0')
    {
        printf("Process name can't be empty. \n");
        return false;
    }

    return true;
}

bool isValidProcessId(char inputedProcessId[MAX_INTEGER_LENGTH])
{
    if (isValidInteger(inputedProcessId) == false)
    {
        printf("Invalid input. Process id must be an integer value. \n");
        return false;
    }

    return true;
}

bool isValidInteger(char integerValue[MAX_INTEGER_LENGTH])
{
    int currentIntegerIndex = 0;

    while (integerValue[currentIntegerIndex] != '\0')
    {
        if (!isdigit(integerValue[currentIntegerIndex]))
        {
            return false;
        }

        currentIntegerIndex++;
    }

    return true;
}

bool isValidInputOutputStartTime(char *inputedInputOutputStartTime)
{
    if (isValidInteger(inputedInputOutputStartTime) == false)
    {
        if (isDash(inputedInputOutputStartTime) == false)
        {
            printf("Invalid input. I/O start time must be an integer or '-'\n");
            return false;
        }
    }

    return true;
}

bool isValidInputOutputDuration(char *inputedInputOutputDuration)
{
    if (isValidInteger(inputedInputOutputDuration) == false)
    {
        if (isDash(inputedInputOutputDuration) == false)
        {
            printf("Invalid input. I/O duration time must be integer or '-'\n");
            return false;
        }
    }

    return true;
}

bool isValidBurstTime(char inputedtBurstTime[MAX_INTEGER_LENGTH])
{
    if (isValidInteger(inputedtBurstTime) == false)
    {
        printf("Invalid input. Burst must be an integer value. \n");
        return false;
    }

    return true;
}

bool validateProcessInputValues(ProcessInput inputedProcessValues)
{
    if (isValidProcessName(inputedProcessValues.processName) == false)
    {
        return false;
    }

    if (isValidProcessId(inputedProcessValues.processId) == false)
    {
        return false;
    }

    if (isValidBurstTime(inputedProcessValues.burstTime) == false)
    {
        return false;
    }

    if (isValidInputOutputStartTime(inputedProcessValues.inputOutputStartTime) == false)
    {
        return false;
    }

    if (isValidInputOutputDuration(inputedProcessValues.inputOutputDuration) == false)
    {
        return false;
    }

    return true;
}

bool validateKillEventInputValues(char inputedProcessId[MAX_INTEGER_LENGTH], char inputedTime[MAX_INTEGER_LENGTH])
{
    if (isValidProcessId(inputedProcessId) == false)
    {
        return false;
    }

    if (isValidInteger(inputedTime) == false)
    {
        return false;
    }

    return true;
}

void initializeQueue(Queue *queue)
{
    queue->front = NULL;
    queue->rear = NULL;
    queue->sizeOfQueue = 0;
}

void initializeHashmap()
{
    for (int currentHashNodeIndex = 0; currentHashNodeIndex < MAX_HASHMAP_SIZE; currentHashNodeIndex++)
    {
        hashmap[currentHashNodeIndex] = NULL;
    }
}

void enqueue(Queue *queue, PCBNode *pcb)
{
    QueueNode *newNode = malloc(sizeof(QueueNode));

    if (newNode == NULL)
    {
        printf("Memory allocation failed. \n");
        return;
    }

    newNode->pcb = pcb;
    newNode->next = NULL;

    if (queue->rear == NULL)
    {
        queue->front = queue->rear = newNode;
        queue->sizeOfQueue++;
        return;
    }

    queue->rear->next = newNode;
    queue->rear = newNode;
    queue->sizeOfQueue++;
}

PCBNode *dequeue(Queue *queue)
{
    if (queue->front == NULL)
    {
        return NULL;
    }

    QueueNode *nodeToBeDeleted = queue->front;
    PCBNode *pcb = nodeToBeDeleted->pcb;
    queue->front = nodeToBeDeleted->next;

    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }

    free(nodeToBeDeleted);
    queue->sizeOfQueue--;

    return pcb;
}

int removeFromQueue(Queue *queue, int processId)
{
    QueueNode *currentNode = queue->front;
    QueueNode *previousNode = NULL;

    while (currentNode != NULL)
    {
        if (currentNode->pcb->processId == processId)
        {
            if (previousNode == NULL)
            {
                queue->front = currentNode->next;
            }
            else
            {
                previousNode->next = currentNode->next;
            }

            if (currentNode == queue->rear)
            {
                queue->rear = previousNode;
            }

            free(currentNode);
            queue->sizeOfQueue--;

            return 1;
        }

        previousNode = currentNode;
        currentNode = currentNode->next;
    }

    return 0;
}

int hashFunction(int processId)
{
    int hashedIndex = processId % MAX_HASHMAP_SIZE;

    if (hashedIndex < 0)
    {
        hashedIndex += MAX_HASHMAP_SIZE;
    }

    return hashedIndex;
}

void hashmapPut(PCBNode *pcb)
{
    int bucketIndex = hashFunction(pcb->processId);
    pcb->next = hashmap[bucketIndex];
    hashmap[bucketIndex] = pcb;
}

PCBNode *hashmapGet(int processId)
{
    int bucketindex = hashFunction(processId);
    PCBNode *currentPCBNode = hashmap[bucketindex];

    while (currentPCBNode != NULL)
    {
        if (currentPCBNode->processId == processId)
        {
            return currentPCBNode;
        }

        currentPCBNode = currentPCBNode->next;
    }

    return NULL;
}

PCBNode *createPCB(char *processName, int processId, int burstTime, int inputOutputStartTime, int inputOutputDurationTime)
{
    PCBNode *newPCB = malloc(sizeof(PCBNode));

    if (newPCB == NULL)
    {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    newPCB->processId = processId;
    strcpy(newPCB->processName, processName);
    newPCB->arrivalTime = 0;
    newPCB->burstTime = burstTime;
    newPCB->currentBurstTime = 0;
    newPCB->remainingBurstTime = burstTime;
    newPCB->inputOutputStartTime = inputOutputStartTime;
    newPCB->inputOutputDurationTime = inputOutputDurationTime;
    newPCB->remainingInputOutputTime = 0;
    newPCB->currentInputOutputTime = 0;
    newPCB->completionTime = 0;
    newPCB->next = NULL;
    newPCB->executionTime = 0;
    newPCB->inputOutputJustStartedFlag = 0;
    newPCB->isKilled = false;
    return newPCB;
}

void addKillEvent(int processId, int time)
{
    KILLNode *newNode = malloc(sizeof(KILLNode));

    if (newNode == NULL)
    {
        printf("Memory allocation failed. \n");
        return;
    }

    newNode->processId = processId;
    newNode->KillTime = time;
    newNode->next = NULL;

    if (KILLHead == NULL || KILLHead->KillTime > time)
    {
        newNode->next = KILLHead;
        KILLHead = newNode;
        return;
    }
    else
    {
        KILLNode *currentNode = KILLHead;

        while (currentNode->next != NULL && currentNode->next->KillTime <= time)
        {
            currentNode = currentNode->next;
        }

        newNode->next = currentNode->next;
        currentNode->next = newNode;
    }
}

void processKillEvent(int currentTime, SystemQueues *systemQueues, PCBNode **running)
{
    KILLNode *currentKillNode = KILLHead;
    KILLNode *previousKillNode = NULL;

    while (currentKillNode != NULL)
    {
        if (currentKillNode->KillTime == currentTime)
        {
            PCBNode *pcb = hashmapGet(currentKillNode->processId);

            if (pcb != NULL && pcb->isKilled == false && pcb->state != TERMINATED)
            {
                pcb->state = KILLED;
                pcb->isKilled = true;
                pcb->completionTime = currentTime;

                removeFromQueue(systemQueues->readyQueue, pcb->processId);
                removeFromQueue(systemQueues->waitingQueue, pcb->processId);

                if (*running != NULL && (*running)->processId == pcb->processId)
                {
                    *running = NULL;
                }

                enqueue(systemQueues->terminatedQueue, pcb);
            }

            KILLNode *nodeToBeDeleted = currentKillNode;

            if (previousKillNode == NULL)
            {
                KILLHead = KILLHead->next;
                currentKillNode = KILLHead;
            }
            else
            {
                previousKillNode->next = currentKillNode->next;
                currentKillNode = previousKillNode->next;
            }

            free(nodeToBeDeleted);
            continue;
        }

        previousKillNode = currentKillNode;
        currentKillNode = currentKillNode->next;
    }
}

void handleInputOutputTime(Queue *waitingQueue, Queue *readyQueue)
{
    QueueNode *currentNode = waitingQueue->front;
    QueueNode *previousNode = NULL;

    while (currentNode != NULL)
    {
        PCBNode *pcb = currentNode->pcb;
        QueueNode *nextNode = currentNode->next;

        if (pcb->inputOutputJustStartedFlag)
        {
            pcb->inputOutputJustStartedFlag = 0;
        }
        else
        {
            if (pcb->remainingInputOutputTime > 0)
            {
                pcb->remainingInputOutputTime--;
            }
        }

        if (pcb->remainingInputOutputTime == 0)
        {
            if (pcb->isKilled == false)
            {
                pcb->state = READY;
                enqueue(readyQueue, pcb);
            }

            if (previousNode == NULL)
            {
                waitingQueue->front = currentNode->next;
            }
            else
            {
                previousNode->next = currentNode->next;
            }

            if (currentNode == waitingQueue->rear)
            {
                waitingQueue->rear = previousNode;
            }

            free(currentNode);
            waitingQueue->sizeOfQueue--;
            currentNode = nextNode;
            continue;
        }

        previousNode = currentNode;
        currentNode = currentNode->next;
    }
}

void handleSchedulingOfProcesses(SystemQueues *systemQueues)
{
    int currentTime = 0;

    PCBNode *runningProcess = NULL;

    while (systemQueues->readyQueue->sizeOfQueue > 0 || systemQueues->waitingQueue->sizeOfQueue > 0 || runningProcess != NULL)
    {
        processKillEvent(currentTime, systemQueues, &runningProcess);

        if (runningProcess == NULL && systemQueues->readyQueue->sizeOfQueue > 0)
        {
            PCBNode *currentNode = dequeue(systemQueues->readyQueue);

            if (currentNode != NULL && !currentNode->isKilled)
            {
                runningProcess = currentNode;
            }
            else if (currentNode != NULL)
            {
                enqueue(systemQueues->terminatedQueue, currentNode);
            }
        }

        if (runningProcess != NULL)
        {
            runningProcess->currentBurstTime++;
            runningProcess->executionTime++;
            runningProcess->remainingBurstTime--;

            if (runningProcess->currentBurstTime == runningProcess->inputOutputStartTime && runningProcess->inputOutputDurationTime > 0)
            {
                runningProcess->state = WAITING;
                runningProcess->remainingInputOutputTime = runningProcess->inputOutputDurationTime;
                runningProcess->currentInputOutputTime = 0;
                runningProcess->inputOutputJustStartedFlag = 1;

                enqueue(systemQueues->waitingQueue, runningProcess);
                runningProcess = NULL;
            }
            else if (runningProcess->remainingBurstTime <= 0)
            {
                runningProcess->completionTime = currentTime + 1;
                runningProcess->state = TERMINATED;

                enqueue(systemQueues->terminatedQueue, runningProcess);
                runningProcess = NULL;
            }
        }

        handleInputOutputTime(systemQueues->waitingQueue, systemQueues->readyQueue);
        currentTime++;
    }
}

void sortPCBBasedOnProcessId(PCBNode **allTerminatedPCBs, int totalTerminatedPCBs)
{
    for (int turn = 0; turn < totalTerminatedPCBs - 1; turn++)
    {
        for (int index = 0; index < totalTerminatedPCBs - turn - 1; index++)
        {
            if (allTerminatedPCBs[index]->processId > allTerminatedPCBs[index + 1]->processId)
            {
                PCBNode *temporaryStore = allTerminatedPCBs[index];
                allTerminatedPCBs[index] = allTerminatedPCBs[index + 1];
                allTerminatedPCBs[index + 1] = temporaryStore;
            }
        }
    }
}

int countTotalTerminatedPCB(Queue *terminatedQueue)
{
    QueueNode *currentNode = terminatedQueue->front;
    int totalTerminatedPCBs = 0;

    while (currentNode != NULL)
    {
        totalTerminatedPCBs++;
        currentNode = currentNode->next;
    }

    return totalTerminatedPCBs;
}

bool hasKillEvent(PCBNode **allTerminatedPCBs, int totalTerminatedPCBs)
{
    for (int index = 0; index < totalTerminatedPCBs; index++)
    {
        if (allTerminatedPCBs[index]->state == KILLED)
        {
            return true;
        }
    }

    return false;
}

void printFinalReport(Queue *terminatedQueue)
{
    int totalTerminatedPCBs = countTotalTerminatedPCB(terminatedQueue);

    if (totalTerminatedPCBs == 0)
    {
        return;
    }

    PCBNode *allTerminatedPCBs[totalTerminatedPCBs];
    QueueNode *currentNode = terminatedQueue->front;
    int index = 0;

    while (currentNode != NULL)
    {
        allTerminatedPCBs[index++] = currentNode->pcb;
        currentNode = currentNode->next;
    }

    sortPCBBasedOnProcessId(allTerminatedPCBs, totalTerminatedPCBs);
    bool isKillEventExist = hasKillEvent(allTerminatedPCBs, totalTerminatedPCBs);

    if (isKillEventExist)
    {
        printf("\n%-5s %-10s %-5s %-5s %-15s %-12s %-8s\n",
               "PID", "Name", "CPU", "IO", "Status", "Turnaround", "Waiting");
    }
    else
    {
        printf("\n%-5s %-10s %-5s %-5s %-12s %-8s\n",
               "PID", "Name", "CPU", "IO", "Turnaround", "Waiting");
    }

    int currentTerminatedPCBIndex = 0;

    while (currentTerminatedPCBIndex < totalTerminatedPCBs)
    {
        PCBNode *pcb = allTerminatedPCBs[currentTerminatedPCBIndex];
        int cpuTime = pcb->burstTime;
        int inputOutputTime = pcb->inputOutputDurationTime;

        if (pcb->state == KILLED)
        {
            printf("%-5d %-10s %-5d %-5d KILLED at %-4d %-12s %-8s\n",
                   pcb->processId,
                   pcb->processName,
                   cpuTime,
                   inputOutputTime,
                   pcb->completionTime,
                   "-",
                   "-");
        }
        else
        {
            int turnaroundTime = pcb->completionTime - pcb->arrivalTime;
            int waitingTime = turnaroundTime - cpuTime;

            if (isKillEventExist)
            {
                printf("%-5d %-10s %-5d %-5d OK%-12s %-12d %-8d\n",
                       pcb->processId,
                       pcb->processName,
                       cpuTime,
                       inputOutputTime,
                       "",
                       turnaroundTime,
                       waitingTime);
            }
            else
            {
                printf("%-5d %-10s %-5d %-5d %-12d %-8d\n",
                       pcb->processId,
                       pcb->processName,
                       cpuTime,
                       inputOutputTime,
                       turnaroundTime,
                       waitingTime);
            }
        }

        currentTerminatedPCBIndex++;
    }
}

void freeQueueNodes(Queue *queue)
{
    QueueNode *currentNode = queue->front;

    while (currentNode != NULL)
    {
        QueueNode *nextNode = currentNode->next;
        free(currentNode);
        currentNode = nextNode;
    }

    queue->front = NULL;
    queue->rear = NULL;
    queue->sizeOfQueue = 0;
}

void freeHashmap()
{
    for (int currentIndex = 0; currentIndex < MAX_HASHMAP_SIZE; currentIndex++)
    {
        PCBNode *currentPCB = hashmap[currentIndex];

        while (currentPCB != NULL)
        {
            PCBNode *nextPCB = currentPCB->next;
            free(currentPCB);
            currentPCB = nextPCB;
        }

        hashmap[currentIndex] = NULL;
    }
}

void freeKillNodes()
{
    KILLNode *currentKillNode = KILLHead;

    while (currentKillNode != NULL)
    {
        KILLNode *nextKillNode = currentKillNode->next;
        free(currentKillNode);
        currentKillNode = nextKillNode;
    }

    KILLHead = NULL;
}

void freeMemory(SystemQueues *systemQueues)
{
    freeHashmap();
    freeKillNodes();

    freeQueueNodes(systemQueues->readyQueue);
    freeQueueNodes(systemQueues->waitingQueue);
    freeQueueNodes(systemQueues->terminatedQueue);
}
