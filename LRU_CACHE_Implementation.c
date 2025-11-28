#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MIN_CAPACITY 1
#define MAX_CAPACITY 1000
#define INPUT_LINE_SIZE 100
#define COMMAND_SIZE 100
#define ARGUMENT_SIZE 100
#define DATA_SIZE 100

typedef struct LRUCache
{
    int key;
    char *data;
    struct LRUCache *prev;
    struct LRUCache *next;
} LRUCache;

typedef struct HashNode
{
    int key;
    LRUCache *lruCacheNode;
} HashNode;

typedef struct Hashmap
{
    HashNode **buckets;
    int hashmap_Capacity;
} Hashmap;

Hashmap *map;
LRUCache *LRUCacheHead = NULL;
LRUCache *LRUCacheTail = NULL;
int LRUCache_Capacity = 0;
int LRUCacheCurrentSize = 0;

void createCache(int capacity);
bool checkCacheCreated();
int findKeyInHashMap(int key);
char *copyString(char *source);
void moveToFront(LRUCache *LRUCacheNode);
LRUCache *addAtFrontOfLRUCache(int key, char *data);
void addAtBucketIndexOfHashmap(int key, char *data, int bucketIndex, LRUCache *newNode);
void put(int key, char *value);
void updateData(int key, char *data, int bucketIndex);
void removeFromLRUCache();
void removeFromHashmap(int key, int bucketIndex);
void evictLRU();
char *get(int key);
bool isValidCapacity(int capacity);
bool isValidInteger(char capacity[ARGUMENT_SIZE], int *integerValue);
void freeCache();

int main()
{
    char inputLine[INPUT_LINE_SIZE];
    char command[COMMAND_SIZE];
    char argument[ARGUMENT_SIZE];
    char data[DATA_SIZE];
    int scannedCount = 0;

    while (1)
    {
        if (fgets(inputLine, sizeof(inputLine), stdin) == NULL)
        {
            continue;
        }

        inputLine[strcspn(inputLine, "\n")] = '\0';
        command[0] = '\0';
        argument[0] = '\0';
        data[0] = '\0';

        if (strlen(inputLine) == 0)
        {
            continue;
        }

        scannedCount = sscanf(inputLine, "%s %s %[^\n]", command, argument, data);

        if (strcmp(command, "createCache") == 0)
        {
            int capacity = 0;

            if (scannedCount != 2)
            {
                printf("Error: createCache commmand requires capacity.\n");
                continue;
            }

            if (isValidInteger(argument, &capacity) == false)
            {
                printf("Capacity must be an integer value in range (1-1000). \n");
                continue;
            }

            if (isValidCapacity(capacity) == false)
            {
                continue;
            }

            freeCache();
            createCache(capacity);
        }
        else if (strcmp(command, "put") == 0)
        {
            int key = 0;

            if (checkCacheCreated() == false)
            {
                continue;
            }

            if (map == NULL)
            {
                printf("Cache not created. Call createCache(capacity) first.\n");
                continue;
            }

            if (scannedCount != 3)
            {
                printf("Error: put command requires a key and a value separated by space.\n");
                continue;
            }

            if (isValidInteger(argument, &key) == false)
            {
                printf("Error: In put command Key must be an integer value. \n");
                continue;
            }

            put(key, data);
        }
        else if (strcmp(command, "get") == 0)
        {
            int key = 0;

            if (checkCacheCreated() == false)
            {
                continue;
            }

            if (scannedCount != 2)
            {
                printf("Error: get commmand requires a key.\n");
                continue;
            }

            if (isValidInteger(argument, &key) == false)
            {
                printf("Key must be an integer value. \n");
                continue;
            }

            char *value = get(key);

            if (value == NULL)
            {
                printf("NULL\n");
            }
            else
            {
                printf("%s\n", value);
            }
        }
        else if (strcmp(command, "exit") == 0)
        {
            if (scannedCount != 1)
            {
                printf("Error: exit command doesn't have any arguments. \n");
                continue;
            }

            freeCache();
            break;
        }
        else
        {
            printf("Command is not valid. \n");
        }
    }

    return 0;
}

bool checkCacheCreated()
{
    if (map == NULL)
    {
        printf("Cache not created. Call createCache(capacity) first.\n");
        return false;
    }

    return true;
}

bool isValidCapacity(int capacity)
{
    if (capacity < MIN_CAPACITY || capacity > MAX_CAPACITY)
    {
        printf("Invalid capacity. Must be in range (1-1000). \n");
        return false;
    }

    return true;
}

bool isValidInteger(char capacity[ARGUMENT_SIZE], int *integerValue)
{
    for (int currentCharacterIndex = 0; currentCharacterIndex < strlen(capacity); currentCharacterIndex++)
    {
        if (!isdigit((unsigned char)(capacity[currentCharacterIndex])))
        {
            return false;
        }
    }

    *integerValue = atoi(capacity);
    return true;
}

int hash(int key)
{
    int hashed = key % map->hashmap_Capacity;

    if (hashed < 0)
    {
        hashed += map->hashmap_Capacity;
    }

    return hashed;
}

char *copyString(char *source)
{
    char *destination = malloc(strlen(source) + 1);

    if (destination == NULL)
    {
        printf("Memory allocation failed. \n");
        return NULL;
    }

    strcpy(destination, source);
    return destination;
}

void createCache(int capacity)
{
    LRUCache_Capacity = capacity;
    LRUCacheCurrentSize = 0;
    LRUCacheHead = NULL;
    LRUCacheTail = NULL;
    map = malloc(sizeof(Hashmap));
    map->hashmap_Capacity = capacity;
    map->buckets = calloc(map->hashmap_Capacity, sizeof(HashNode *));
}

void moveToFront(LRUCache *LRUCacheNode)
{
    if (LRUCacheHead == LRUCacheNode)
    {
        return;
    }

    if (LRUCacheHead == NULL)
    {
        LRUCacheHead = LRUCacheTail = LRUCacheNode;
        return;
    }

    if (LRUCacheNode->prev != NULL)
    {
        LRUCacheNode->prev->next = LRUCacheNode->next;
    }

    if (LRUCacheNode->next != NULL)
    {
        LRUCacheNode->next->prev = LRUCacheNode->prev;
    }

    if (LRUCacheNode == LRUCacheTail)
    {
        LRUCacheTail = LRUCacheTail->prev;
    }

    LRUCacheNode->prev = NULL;
    LRUCacheNode->next = LRUCacheHead;

    if (LRUCacheHead != NULL)
    {
        LRUCacheHead->prev = LRUCacheNode;
    }

    LRUCacheHead = LRUCacheNode;
}

int findKeyInHashMap(int key)
{
    int bucketIndex = hash(key);

    for (int index = 0; index < LRUCache_Capacity; index++)
    {
        int tryBucketIndex = (bucketIndex + index) % LRUCache_Capacity;

        if (map->buckets[tryBucketIndex] != NULL && map->buckets[tryBucketIndex]->key == key)
        {
            return tryBucketIndex;
        }
    }

    return -1;
}

int findFreeBucket(int key)
{
    int bucketIndex = hash(key);

    for (int index = 0; index < LRUCache_Capacity; index++)
    {
        int tryBucketIndex = (bucketIndex + index) % LRUCache_Capacity;

        if (map->buckets[tryBucketIndex] == NULL)
        {
            return tryBucketIndex;
        }
    }

    return -1;
}

void removeFromHashmap(int key, int bucketIndex)
{
    while (map->buckets[bucketIndex] != NULL)
    {
        if (map->buckets[bucketIndex]->key == key)
        {
            break;
        }

        bucketIndex = (bucketIndex + 1) % map->hashmap_Capacity;
    }

    if (map->buckets[bucketIndex] != NULL && map->buckets[bucketIndex]->key == key)
    {
        free(map->buckets[bucketIndex]);
        map->buckets[bucketIndex] = NULL;
    }
}

void removeFromLRUCache()
{
    if (LRUCacheTail == NULL)
    {
        return;
    }

    LRUCache *temporaryLRUCache = LRUCacheTail;

    if (temporaryLRUCache->data != NULL)
    {
        free(temporaryLRUCache->data);
    }

    if (temporaryLRUCache->prev != NULL)
    {
        LRUCacheTail = temporaryLRUCache->prev;
        temporaryLRUCache->prev->next = NULL;
    }
    else
    {
        LRUCacheHead = LRUCacheTail = NULL;
    }

    free(temporaryLRUCache);
    LRUCacheCurrentSize--;
}

void evictLRU()
{
    if (LRUCacheTail == NULL)
    {
        return;
    }

    int key = LRUCacheTail->key;
    int bucketIndex = findKeyInHashMap(key);

    removeFromLRUCache();
    removeFromHashmap(key, bucketIndex);
}

LRUCache *addAtFrontOfLRUCache(int key, char *data)
{
    if (LRUCache_Capacity == LRUCacheCurrentSize)
    {
        evictLRU();
    }

    LRUCache *newNode = malloc(sizeof(LRUCache));
    if (newNode == NULL)
    {
        printf("Memory allocation failed. \n");
        return NULL;
    }

    newNode->key = key;
    newNode->data = copyString(data);
    newNode->prev = NULL;
    newNode->next = NULL;

    if (LRUCacheHead == NULL && LRUCacheTail == NULL)
    {
        LRUCacheHead = LRUCacheTail = newNode;
    }
    else
    {
        newNode->next = LRUCacheHead;
        LRUCacheHead->prev = newNode;
        LRUCacheHead = newNode;
    }

    LRUCacheCurrentSize++;
    return newNode;
}

void addAtBucketIndexOfHashmap(int key, char *data, int bucketIndex, LRUCache *newNode)
{
    if (bucketIndex == -1)
    {
        bucketIndex = findFreeBucket(key);
    }

    HashNode *hashmapNode = malloc(sizeof(HashNode));
    if (hashmapNode == NULL)
    {
        printf("Memory allocation failed. \n");

        if (newNode->data != NULL)
        {
            free(newNode->data);
        }

        free(newNode);
        return;
    }

    hashmapNode->key = key;
    hashmapNode->lruCacheNode = newNode;
    map->buckets[bucketIndex] = hashmapNode;
}

void updateData(int key, char *data, int bucketIndex)
{
    LRUCache *oldNode = map->buckets[bucketIndex]->lruCacheNode;

    if (oldNode->data != NULL)
    {
        free(oldNode->data);
    }

    oldNode->data = copyString(data);
    moveToFront(oldNode);
    return;
}

void put(int key, char *data)
{
    if (map == NULL)
    {
        printf("Cache not created. Call createCache(capacity) first.\n");
        return;
    }

    int bucketIndex = findKeyInHashMap(key);

    if (bucketIndex != -1)
    {
        updateData(key, data, bucketIndex);
        return;
    }

    if (bucketIndex == -1)
    {
        bucketIndex = findFreeBucket(key);
    }

    LRUCache *newNode = addAtFrontOfLRUCache(key, data);
    addAtBucketIndexOfHashmap(key, data, bucketIndex, newNode);
}

char *get(int key)
{
    if (map == NULL)
    {
        return NULL;
    }

    int bucketIndex = findKeyInHashMap(key);

    if (bucketIndex == -1)
    {
        return NULL;
    }

    LRUCache *currentLruCacheNode = map->buckets[bucketIndex]->lruCacheNode;
    moveToFront(currentLruCacheNode);
    return currentLruCacheNode->data;

    return NULL;
}

void freeCache()
{
    if (map == NULL)
    {
        return;
    }

    for (int currentBucketIndex = 0; currentBucketIndex < map->hashmap_Capacity; currentBucketIndex++)
    {
        HashNode *hashmapNode = map->buckets[currentBucketIndex];

        if (hashmapNode != NULL)
        {
            free(hashmapNode);
        }
    }

    free(map->buckets);

    LRUCache *currentLRUCacheNode = LRUCacheHead;

    while (currentLRUCacheNode != NULL)
    {
        LRUCache *nextNode = currentLRUCacheNode->next;

        if (currentLRUCacheNode->data != NULL)
        {
            free(currentLRUCacheNode->data);
        }

        free(currentLRUCacheNode);
        currentLRUCacheNode = nextNode;
    }

    free(map);
    map = NULL;
    LRUCacheHead = LRUCacheTail = NULL;
    LRUCacheCurrentSize = 0;

    return;
}
