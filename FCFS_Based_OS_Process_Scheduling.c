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
void addKillEvent(int processId, int time);
void takeTotalProcessesInput(int totalProcesses, Queue *readyQueue);
void takeTotalKillEventsInput(int totalKillEvents);
bool validateProcessInputValues(char processName[MAX_NAME_LENGTH], char inputedProcessId[MAX_INTEGER_LENGTH], char inputedtBurstTime[MAX_INTEGER_LENGTH], char inputedInputOutputStartTime[MAX_INTEGER_LENGTH], char inputedInputOutputDuration[MAX_INTEGER_LENGTH]);
bool validateKillEventInputValues(char inputedProcessId[MAX_INTEGER_LENGTH], char inputedTime[MAX_INTEGER_LENGTH]);
bool validateInputLine(char inputLine[MAX_INPUT_LENGTH]);
bool isValidProcessName(char processName[MAX_NAME_LENGTH]);
bool isValidInteger(char integerValue[MAX_INTEGER_LENGTH]);
bool isDashOrInteger(char *inputValue);
void processKillEvent(int currentTime, Queue *ready, Queue *waiting, Queue *terminated, PCBNode **running);
void handleInputOutputTime(Queue *waiting, Queue *ready);
void handleSchedulingOfProcesses(Queue *readyQueue, Queue *waitingQueue, Queue *terminatedQueue);
void printFinalReport(Queue *terminated);

int main()
{
    int totalProcesses = 0;
    int totalKillEvents = 0;
    initializeHashmap();

    Queue readyQueue;
    Queue waitingQueue;
    Queue terminatedQueue;

    initializeQueue(&readyQueue);
    initializeQueue(&waitingQueue);
    initializeQueue(&terminatedQueue);

    printf("Enter total number of processes: ");
    scanf("%d", &totalProcesses);
    getchar();
    takeTotalProcessesInput(totalProcesses, &readyQueue);

    printf("Enter total number of kill events: ");
    scanf("%d", &totalKillEvents);
    getchar();
    takeTotalKillEventsInput(totalKillEvents);

    handleSchedulingOfProcesses(&readyQueue, &waitingQueue, &terminatedQueue);
    printFinalReport(&terminatedQueue);

    return 0;
}

bool validateInputLine(char inputLine[MAX_INPUT_LENGTH])
{
    if (inputLine[0] == '\0')
    {
        printf("Input can't be empty. Please enter processName, processId, burstTime, inputOutputStartTime, inputOutputDuration. \n");
        return false;
    }

    return true;
}

void takeTotalProcessesInput(int totalProcesses, Queue *readyQueue)
{
    int processInputIndex = 0;

    while (processInputIndex < totalProcesses)
    {
        char inputLine[MAX_INPUT_LENGTH];
        fgets(inputLine, sizeof(inputLine), stdin);

        if (validateInputLine(inputLine) == false)
        {
            continue;
        }

        char processName[MAX_NAME_LENGTH];
        char inputedProcessId[MAX_INTEGER_LENGTH];
        int processId;
        char inputedtBurstTime[MAX_INTEGER_LENGTH];
        int burstTime;
        char inputedInputOutputStartTime[MAX_INTEGER_LENGTH];
        int inputOutputStartTime;
        char inputedInputOutputDuration[MAX_INTEGER_LENGTH];
        int inputOutputDuration;

        int inputValuesCount = sscanf(inputLine, "%s %s %s %s %s", processName, inputedProcessId, inputedtBurstTime, inputedInputOutputStartTime, inputedInputOutputDuration);

        if (inputValuesCount != 5)
        {
            printf("Invalid input. Process input format must be: <process_name> <pid> <burst_time> <io_start_time> <io_duration> \n");
            continue;
        }

        if (validateProcessInputValues(processName, inputedProcessId, inputedtBurstTime, inputedInputOutputStartTime, inputedInputOutputDuration) == false)
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
        processInputIndex++;
    }
}

void takeTotalKillEventsInput(int totalKillEvents)
{
    int currentKillEventIndex = 0;

    while (currentKillEventIndex < totalKillEvents)
    {
        char inputLine[MAX_INPUT_LENGTH];
        fgets(inputLine, sizeof(inputLine), stdin);

        if (validateInputLine(inputLine) == false)
        {
            continue;
        }

        char inputedProcessId[MAX_INTEGER_LENGTH];
        int processId;
        char inputedTime[MAX_INTEGER_LENGTH];
        int time;

        int totalInputValuesCount = sscanf(inputLine, "kill %s %s", inputedProcessId, inputedTime);

        if (totalInputValuesCount != 2)
        {
            printf("Invalid input. Kill event format must be: kill <PID> <kill_time> \n");
            continue;
        }

        if (validateKillEventInputValues(inputedProcessId, inputedTime) == false)
        {
            continue;
        }

        processId = atoi(inputedProcessId);
        time = atoi(inputedTime);

        addKillEvent(processId, time);
        currentKillEventIndex++;
    }
}

bool isDashOrInteger(char *inputValue)
{
    if (strcmp(inputValue, "-") == 0)
    {
        return true;
    }

    return isValidInteger(inputValue);
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

bool validateProcessInputValues(char processName[MAX_NAME_LENGTH], char inputedProcessId[MAX_INTEGER_LENGTH], char inputedtBurstTime[MAX_INTEGER_LENGTH], char inputedInputOutputStartTime[MAX_INTEGER_LENGTH], char inputedInputOutputDuration[MAX_INTEGER_LENGTH])
{
    if (isValidProcessName(processName) == false)
    {
        return false;
    }

    if (isValidInteger(inputedProcessId) == false)
    {
        printf("Invalid input. Process id must be an integer value. \n");
        return false;
    }

    if (isValidInteger(inputedtBurstTime) == false)
    {
        printf("Invalid input. Burst must be an integer value. \n");
        return false;
    }

    if (!isDashOrInteger(inputedInputOutputStartTime))
    {
        printf("Invalid input. I/O start time must be integer or '-'\n");
        return false;
    }

    if (!isDashOrInteger(inputedInputOutputDuration))
    {
        printf("Invalid input. I/O duration must be integer or '-'\n");
        return false;
    }

    return true;
}

bool validateKillEventInputValues(char inputedProcessId[MAX_INTEGER_LENGTH], char inputedTime[MAX_INTEGER_LENGTH])
{
    if (isValidInteger(inputedProcessId) == false)
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

    if (KILLHead == NULL)
    {
        KILLHead = newNode;
    }
    else
    {
        KILLNode *currentNode = KILLHead;

        while (currentNode->next != NULL)
        {
            currentNode = currentNode->next;
        }

        currentNode->next = newNode;
    }
}

void processKillEvent(int currentTime, Queue *ready, Queue *waiting, Queue *terminated, PCBNode **running)
{
    KILLNode *currentKillNode = KILLHead;
    KILLNode *previousKillNode = NULL;

    while (currentKillNode != NULL)
    {
        if (currentKillNode->KillTime == currentTime)
        {
            PCBNode *pcb = hashmapGet(currentKillNode->processId);

            if (pcb != NULL)
            {
                pcb->state = KILLED;
                pcb->completionTime = currentTime;

                removeFromQueue(ready, pcb->processId);
                removeFromQueue(waiting, pcb->processId);

                if (*running != NULL && (*running)->processId == currentKillNode->processId)
                {
                    *running = NULL;
                }

                enqueue(terminated, pcb);
            }

            if (previousKillNode == NULL)
            {
                KILLHead = KILLHead->next;
                free(currentKillNode);
                currentKillNode = KILLHead;
            }
            else
            {
                previousKillNode->next = currentKillNode->next;
                free(currentKillNode);
                currentKillNode = previousKillNode->next;
            }

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

        pcb->currentInputOutputTime++;
        pcb->remainingInputOutputTime--;

        if (pcb->remainingInputOutputTime <= 0)
        {
            pcb->state = READY;
            enqueue(readyQueue, pcb);

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

            QueueNode *nodeToBeDeleted = currentNode;
            currentNode = currentNode->next;
            free(nodeToBeDeleted);
            waitingQueue->sizeOfQueue--;

            continue;
        }

        previousNode = currentNode;
        currentNode = currentNode->next;
    }
}

void handleSchedulingOfProcesses(Queue *readyQueue, Queue *waitingQueue, Queue *terminatedQueue)
{
    int currentTime = 0;
    PCBNode *runningProcess = NULL;

    while (readyQueue->sizeOfQueue > 0 || waitingQueue->sizeOfQueue > 0 || runningProcess != NULL)
    {
        processKillEvent(currentTime, readyQueue, waitingQueue, terminatedQueue, &runningProcess);

        if (runningProcess == NULL && readyQueue->sizeOfQueue > 0)
        {
            runningProcess = dequeue(readyQueue);
            runningProcess->state = RUNNING;
        }

        if (runningProcess != NULL)
        {
            runningProcess->currentBurstTime++;
            runningProcess->remainingBurstTime--;

            if (runningProcess->currentBurstTime == runningProcess->inputOutputStartTime && runningProcess->inputOutputDurationTime > 0)
            {
                runningProcess->state = WAITING;
                runningProcess->remainingInputOutputTime = runningProcess->inputOutputDurationTime;
                runningProcess->currentInputOutputTime = 0;

                enqueue(waitingQueue, runningProcess);
                runningProcess = NULL;
            }
            else if (runningProcess->remainingBurstTime <= 0)
            {
                runningProcess->completionTime = currentTime + 1;
                runningProcess->state = TERMINATED;

                enqueue(terminatedQueue, runningProcess);
                runningProcess = NULL;
            }
        }

        handleInputOutputTime(waitingQueue, readyQueue);
        currentTime++;
    }
}

void printFinalReport(Queue *terminatedQueue)
{
    PCBNode *pcbNodesList[MAX_PCB_NODES];
    int totalPcbNodes = 0;
    QueueNode *currentTerminatedNode = terminatedQueue->front;

    while (currentTerminatedNode != NULL)
    {
        pcbNodesList[totalPcbNodes++] = currentTerminatedNode->pcb;
        currentTerminatedNode = currentTerminatedNode->next;
    }

    for (int turn = 0; turn < totalPcbNodes - 1; turn++)
    {
        for (int index = turn + 1; index < totalPcbNodes; index++)
        {
            if (pcbNodesList[turn]->processId > pcbNodesList[index]->processId)
            {
                PCBNode *temporaryNode = pcbNodesList[turn];
                pcbNodesList[turn] = pcbNodesList[index];
                pcbNodesList[index] = temporaryNode;
            }
        }
    }

    printf("\n%-5s %-10s %-5s %-5s %-15s %-12s %-8s\n",
           "PID", "Name", "CPU", "IO", "Status", "Turnaround", "Waiting");

    int currentIndex = 0;
    PCBNode *currentNode = pcbNodesList[currentIndex];

    while (currentIndex < totalPcbNodes)
    {
        int cpuTime = currentNode->burstTime;
        int inputOutputTime = currentNode->inputOutputDurationTime;

        if (currentNode->state == KILLED)
        {
            printf("%-5d %-10s %-5d %-5d KILLED at %-7d %-12s %-8s\n",
                   currentNode->processId,
                   currentNode->processName,
                   cpuTime,
                   inputOutputTime,
                   currentNode->completionTime,
                   "-",
                   "-");
        }
        else
        {
            int turnaroundTime = currentNode->completionTime - currentNode->arrivalTime;
            int waitingTime = turnaroundTime - cpuTime;

            printf("%-5d %-10s %-5d %-5d OK%-12s %-12d %-8d\n",
                   currentNode->processId,
                   currentNode->processName,
                   cpuTime,
                   inputOutputTime,
                   "",
                   turnaroundTime,
                   waitingTime);
        }

        currentIndex++;
        currentNode = pcbNodesList[currentIndex];
    }
}
