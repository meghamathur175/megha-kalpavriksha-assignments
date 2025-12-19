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
    COMPLETED,
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

typedef struct KillNode
{
    int processId;
    int KillTime;
    struct KillNode *next;
} KillNode;

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
KillNode *KillHead = NULL;

void initializeQueue(Queue *queue);
void initializeHashmap();
void enqueue(Queue *queue, PCBNode *pcb);
PCBNode *dequeue(Queue *queue);
int hashFunction(int processId);
void hashmapPut(PCBNode *pcb);
PCBNode *hashmapGet(int processId);
void initializePCB(PCBNode *pcb, const ProcessInput *input);
PCBNode *createPCB(const ProcessInput *input);
int removeFromQueue(Queue *queue, int processId);
void trimWhitespaces(char *inputLine);
void handleProcessInput(const ProcessInput *input, Queue *readyQueue);
void handleKillEventInput(int pid, int time);
void printProcessValidationError(const ProcessInput *input);
bool validateProcessInput(const ProcessInput *input);
bool parseProcessCommand(const char *inputLine, ProcessInput *inputProcessDetails);
bool parseKillCommand(const char *inputLine, int *processId, int *time);
bool isKillCommand(const char *inputLine);
bool readInputLine(char *inputLine);
void takeUserInput(Queue *readyQueue);
void addKillEvent(int processId, int time);
bool isValidProcessName(const char *processName);
bool isValidProcessId(const char *inputedProcessId);
bool isValidInteger(const char *integerValue);
bool isValidBurstTime(const char *inputedtBurstTime);
bool isValidInputOutputStartTime(const char *inputedInputOutputStartTime);
bool isValidInputOutputDuration(const char *inputedInputOutputDuration);
bool isDash(const char *inputValue);
void processKillEvent(int currentTime, SystemQueues *systemQueues, PCBNode **running);
void handleInputOutputTime(Queue *waiting, Queue *ready);
void checkProcessStateTransition(SystemQueues *queues, PCBNode **running, int currentTime);
void executeCPU(PCBNode *running);
void dispatchProcess(SystemQueues *queues, PCBNode **running);
bool systemHasActiveProcesses(SystemQueues *queues, PCBNode *running);
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

bool readInputLine(char *inputLine)
{
    if (fgets(inputLine, MAX_INPUT_LENGTH, stdin) == NULL)
    {
        return false;
    }

    inputLine[strcspn(inputLine, "\n")] = '\0';
    return true;
}

bool isKillCommand(const char *inputLine)
{
    if (strncmp(inputLine, "KILL ", 5) == 0)
    {
        return true;
    }

    return false;
}

bool parseKillCommand(const char *inputLine, int *processId, int *time)
{
    if (sscanf(inputLine, "KILL %d %d", processId, time) == 2)
    {
        return true;
    }

    return false;
}

bool parseProcessCommand(const char *inputLine, ProcessInput *inputProcessDetails)
{
    if (sscanf(inputLine, "%s %s %s %s %s",
               inputProcessDetails->processName,
               inputProcessDetails->processId,
               inputProcessDetails->burstTime,
               inputProcessDetails->inputOutputStartTime,
               inputProcessDetails->inputOutputDuration) == 5)
    {
        return true;
    }

    return false;
}

bool validateProcessInput(const ProcessInput *input)
{
    if (isValidProcessName(input->processName) && isValidProcessId(input->processId) && isValidBurstTime(input->burstTime) &&
        isValidInputOutputStartTime(input->inputOutputStartTime) && isValidInputOutputDuration(input->inputOutputDuration))
    {
        return true;
    }

    return false;
}

void printProcessValidationError(const ProcessInput *input)
{
    if (isValidProcessName(input->processName) == false)
    {
        printf("Process name can't be empty.\n");
    }
    else if (isValidProcessId(input->processId) == false)
    {
        printf("Invalid input. Process ID must be an integer.\n");
    }
    else if (isValidBurstTime(input->burstTime) == false)
    {
        printf("Invalid input. Burst time must be an integer.\n");
    }
    else if (isValidInputOutputStartTime(input->inputOutputStartTime) == false)
    {
        printf("Invalid input. I/O start time must be integer or '-'.\n");
    }
    else if (isValidInputOutputDuration(input->inputOutputDuration) == false)
    {
        printf("Invalid input. I/O duration must be integer or '-'.\n");
    }
}

void handleKillEventInput(int pid, int time)
{
    addKillEvent(pid, time);
}

void handleProcessInput(const ProcessInput *input, Queue *readyQueue)
{
    PCBNode *pcb = createPCB(input);

    if (pcb == NULL)
    {
        return;
    }

    enqueue(readyQueue, pcb);
    hashmapPut(pcb);
}

void takeUserInput(Queue *readyQueue)
{
    char inputLine[MAX_INPUT_LENGTH];

    while (readInputLine(inputLine))
    {
        trimWhitespaces(inputLine);

        if (strlen(inputLine) == 0)
        {
            break;
        }

        if (isKillCommand(inputLine))
        {
            int processId;
            int time;

            if (parseKillCommand(inputLine, &processId, &time) == false)
            {
                printf("Invalid KILL command format. Format is: KILL <PID> <kill_time>\n");
                continue;
            }

            handleKillEventInput(processId, time);
            continue;
        }

        ProcessInput inputProcessDetails;

        if (parseProcessCommand(inputLine, &inputProcessDetails) == false)
        {
            printf("Invalid input.\n");
            printf("Process input format must be: <process_name> <pid> <burst_time> <io_start_time> <io_duration> \n");
            printf("Kill event format must be: KILL <PID> <kill_time> \n");
            
            continue;
        }

        if (validateProcessInput(&inputProcessDetails) == false)
        {
            printProcessValidationError(&inputProcessDetails);
            continue;
        }

        handleProcessInput(&inputProcessDetails, readyQueue);
    }
}

bool isDash(const char *inputValue)
{
    if (strcmp(inputValue, "-") == 0)
    {
        return true;
    }

    return false;
}

bool isValidProcessName(const char *processName)
{
    int currentNameIndex = 0;

    while (processName[currentNameIndex] == ' ')
    {
        currentNameIndex++;
    }

    if (processName[currentNameIndex] == '\0')
    {
        return false;
    }

    return true;
}

bool isValidProcessId(const char *inputedProcessId)
{
    if (isValidInteger(inputedProcessId) == false)
    {
        return false;
    }

    return true;
}

bool isValidInteger(const char *integerValue)
{
    int currentIntegerIndex = 0;

    if (integerValue[0] == '\0')
    {
        return false;
    }

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

bool isValidInputOutputStartTime(const char *inputedInputOutputStartTime)
{
    if (isValidInteger(inputedInputOutputStartTime) == false)
    {
        if (isDash(inputedInputOutputStartTime) == false)
        {
            return false;
        }
    }

    return true;
}

bool isValidInputOutputDuration(const char *inputedInputOutputDuration)
{
    if (isValidInteger(inputedInputOutputDuration) == false)
    {
        if (isDash(inputedInputOutputDuration) == false)
        {
            return false;
        }
    }

    return true;
}

bool isValidBurstTime(const char *inputedtBurstTime)
{
    if (isValidInteger(inputedtBurstTime) == false)
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

void initializePCB(PCBNode *pcb, const ProcessInput *input)
{
    pcb->state = READY;
    pcb->processId = atoi(input->processId);
    strcpy(pcb->processName, input->processName);
    pcb->arrivalTime = 0;
    pcb->burstTime = atoi(input->burstTime);
    pcb->remainingBurstTime = pcb->burstTime;
    pcb->currentBurstTime = 0;
    pcb->inputOutputStartTime = isDash(input->inputOutputStartTime) ? -1 : atoi(input->inputOutputStartTime);
    pcb->inputOutputDurationTime = isDash(input->inputOutputDuration) ? 0 : atoi(input->inputOutputDuration);
    pcb->remainingInputOutputTime = 0;
    pcb->currentInputOutputTime = 0;
    pcb->completionTime = 0;
    pcb->executionTime = 0;
    pcb->inputOutputJustStartedFlag = 0;
    pcb->isKilled = false;
    pcb->next = NULL;
}

PCBNode *createPCB(const ProcessInput *input)
{
    PCBNode *newPCB = malloc(sizeof(PCBNode));

    if (newPCB == NULL)
    {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    initializePCB(newPCB, input);

    return newPCB;
}

void addKillEvent(int processId, int time)
{
    KillNode *newNode = malloc(sizeof(KillNode));

    if (newNode == NULL)
    {
        printf("Memory allocation failed. \n");
        return;
    }

    newNode->processId = processId;
    newNode->KillTime = time;
    newNode->next = NULL;

    if (KillHead == NULL || KillHead->KillTime > time)
    {
        newNode->next = KillHead;
        KillHead = newNode;
        return;
    }
    else
    {
        KillNode *currentNode = KillHead;

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
    KillNode *currentKillNode = KillHead;
    KillNode *previousKillNode = NULL;

    while (currentKillNode != NULL)
    {
        if (currentKillNode->KillTime == currentTime)
        {
            PCBNode *pcb = hashmapGet(currentKillNode->processId);

            if (pcb != NULL && pcb->isKilled == false && pcb->state != COMPLETED)
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

            KillNode *nodeToBeDeleted = currentKillNode;

            if (previousKillNode == NULL)
            {
                KillHead = KillHead->next;
                currentKillNode = KillHead;
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

bool systemHasActiveProcesses(SystemQueues *queues, PCBNode *running)
{
    if (queues->readyQueue->sizeOfQueue > 0 || queues->waitingQueue->sizeOfQueue > 0 || running != NULL)
    {
        return true;
    }

    return false;
}

void dispatchProcess(SystemQueues *queues, PCBNode **running)
{
    if (*running != NULL || queues->readyQueue->sizeOfQueue == 0)
    {
        return;
    }

    PCBNode *pcb = dequeue(queues->readyQueue);

    if (pcb == NULL)
    {
        return;
    }

    if (pcb->isKilled)
    {
        enqueue(queues->terminatedQueue, pcb);
        return;
    }

    pcb->state = RUNNING;
    *running = pcb;
}

void executeCPU(PCBNode *running)
{
    running->currentBurstTime++;
    running->executionTime++;
    running->remainingBurstTime--;
}

void checkProcessStateTransition(SystemQueues *queues, PCBNode **running, int currentTime)
{
    PCBNode *pcb = *running;

    if (pcb == NULL)
    {
        return;
    }

    if (pcb->currentBurstTime == pcb->inputOutputStartTime && pcb->inputOutputDurationTime > 0)
    {
        pcb->state = WAITING;
        pcb->remainingInputOutputTime = pcb->inputOutputDurationTime;
        pcb->inputOutputJustStartedFlag = 1;

        enqueue(queues->waitingQueue, pcb);
        *running = NULL;

        return;
    }

    if (pcb->remainingBurstTime <= 0)
    {
        pcb->state = COMPLETED;
        pcb->completionTime = currentTime + 1;

        enqueue(queues->terminatedQueue, pcb);
        *running = NULL;
    }
}

void handleSchedulingOfProcesses(SystemQueues *systemQueues)
{
    int currentTime = 0;
    PCBNode *runningProcess = NULL;

    while (systemHasActiveProcesses(systemQueues, runningProcess))
    {
        processKillEvent(currentTime, systemQueues, &runningProcess);
        dispatchProcess(systemQueues, &runningProcess);

        if (runningProcess != NULL)
        {
            executeCPU(runningProcess);
            checkProcessStateTransition(systemQueues, &runningProcess, currentTime);
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
    KillNode *currentKillNode = KillHead;

    while (currentKillNode != NULL)
    {
        KillNode *nextKillNode = currentKillNode->next;
        free(currentKillNode);
        currentKillNode = nextKillNode;
    }

    KillHead = NULL;
}

void freeMemory(SystemQueues *systemQueues)
{
    freeHashmap();
    freeKillNodes();

    freeQueueNodes(systemQueues->readyQueue);
    freeQueueNodes(systemQueues->waitingQueue);
    freeQueueNodes(systemQueues->terminatedQueue);
}
