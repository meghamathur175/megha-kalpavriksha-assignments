#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "Players_data.h"

#define MAX_NAME_LENGTH 50
#define MIN_NAME_LENGTH 1
#define MAX_ID_LENGTH 5
#define MAX_ROLE_LENGTH 15
#define MAX_TEAMS 10
#define MIN_TEAMS 1
#define MIN_ID 1
#define MAX_ID 1000
#define MAX_NUMBER_OF_PLATERS_PER_TEAM 50
#define TOTAL_NUMBER_OF_PLAYERS 200
#define MAX_CHOICE_LENGTH 2

typedef struct PlayerModel
{
    int playerId;
    char playerName[MAX_NAME_LENGTH];
    char teamName[MAX_NAME_LENGTH];
    char role[MAX_ROLE_LENGTH];
    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    float performanceIndex;
    struct PlayerModel *next;
} PlayerModel;

typedef struct TeamModel
{
    int teamId;
    char teamName[MAX_NAME_LENGTH];
    int totalPlayers;
    int totalBatsmen;
    int totalAllRounders;
    float averageBattingStrikeRate;
    float totalStrikeRate;
    PlayerModel *batsmenHead;
    PlayerModel *bowlersHead;
    PlayerModel *allRoundersHead;
} TeamModel;

TeamModel allTeams[MAX_TEAMS];

void initializeTeam();
void addPlayerToTeam(PlayerModel *newPlayer);
int findTeamIndex(char *teamName);
void insertPlayersSortedByPerformanceIndex(PlayerModel **currentListHead, PlayerModel *newPlayer);
void initializePlayers();
bool validateChoice(char inputChoice[MAX_CHOICE_LENGTH]);
float computePerformanceIndexOfPlayer(PlayerModel *newPlayer);
void displayPlayersOfSpecificTeam();
void swapTeams(TeamModel *firstTeam, TeamModel *secondTeam2);
int partitionBasedOnAverageBattingStrikeRate(TeamModel temporaryTeams[], int startIndex, int endIndex);
void quickSortTeams(TeamModel temporaryTeams[], int startIndex, int endIndex);
void swapPlayers(PlayerModel **firstRoleHead, PlayerModel **secondRoleHead);
void quickSortPlayers(PlayerModel *temporaryPlayers[], int startIndex, int endIndex);
int partitionBasedOnPerformanceIndex(PlayerModel *temporaryPlayers[], int startIndex, int endIndex);
void displayTeamsByAverageBattingStrikeRate();
void displayTopKPlayersOfSpecificTeamByRole();
void displayPlayersOfAllTeamsFilterByRoleAndPerformanceIndex();
bool inputAndValidateTeamId(int *teamId, int *teamIndex);
bool validateId(char inputTeamId[MAX_ID_LENGTH]);
bool inputAndValidateNumberOfPlayers(int *numberOfPlayers);
bool inputAndValidatePlayerRole(int *roleChoice, char *selectedRole);
void freeAllPlayers();
void cleanInputBuffer();
void exitMenu();

int main()
{
    initializeTeam();
    initializePlayers();

    while (1)
    {
        char inputChoice[MAX_CHOICE_LENGTH];
        int choice;

        printf("\n==============================================================================\n");
        printf("ICC ODI Player Performance Analyzer \n");
        printf("==============================================================================\n");

        printf("1. Display Players of a Specific Team \n");
        printf("2. Display Teams by Average Batting Strike Rate \n");
        printf("3. Display Top K Players of a Specific Team by Role \n");
        printf("4. Display all Players of specific role Across All Teams by performance index \n");
        printf("5. Exit \n");

        printf("==============================================================================\n");
        printf("Enter your choice: ");
        scanf("%s", inputChoice);
        if (validateChoice(inputChoice) == false)
        {
            return 0;
        }

        choice = atoi(inputChoice);

        switch (choice)
        {
        case 1:
            displayPlayersOfSpecificTeam();
            break;
        case 2:
            displayTeamsByAverageBattingStrikeRate();
            break;
        case 3:
            displayTopKPlayersOfSpecificTeamByRole();
            break;
        case 4:
            displayPlayersOfAllTeamsFilterByRoleAndPerformanceIndex();
            break;
        case 5:
            exitMenu();
            return 0;
        default:
            printf("Not a valid choice. \n");
        }
    }
}

bool validateChoice(char inputChoice[MAX_CHOICE_LENGTH])
{
    int currentIndex = 0;

    while (inputChoice[currentIndex] == ' ')
    {
        currentIndex++;
    }

    if (inputChoice[currentIndex] == '\0')
    {
        printf("Choice can't be empty. \n");
        return false;
    }

    for (int index = 0; index < strlen(inputChoice); index++)
    {
        if (!isdigit((unsigned char)(inputChoice[index])))
        {
            printf("Choice must be an integer value in range (1-5). \n");
            return false;
        }
    }

    return true;
}

void insertPlayersSortedByPerformanceIndex(PlayerModel **currentListHead, PlayerModel *newPlayer)
{
    if (*currentListHead == NULL)
    {
        newPlayer->next = *currentListHead;
        *currentListHead = newPlayer;
        return;
    }

    if ((*currentListHead)->performanceIndex < newPlayer->performanceIndex)
    {
        newPlayer->next = *currentListHead;
        *currentListHead = newPlayer;
        return;
    }

    PlayerModel *currentPlayer = *currentListHead;

    while (currentPlayer->next != NULL && currentPlayer->next->performanceIndex > newPlayer->performanceIndex)
    {
        currentPlayer = currentPlayer->next;
    }

    newPlayer->next = currentPlayer->next;
    currentPlayer->next = newPlayer;
}

float computePerformanceIndexOfPlayer(PlayerModel *newPlayer)
{
    float performanceIndex = 0;

    if (strcmp(newPlayer->role, "Batsman") == 0)
    {
        performanceIndex = (newPlayer->battingAverage * newPlayer->strikeRate) / 100;
    }
    else if (strcmp(newPlayer->role, "Bowler") == 0)
    {
        performanceIndex = (newPlayer->wickets * 2) + (100 - newPlayer->economyRate);
    }
    else
    {
        performanceIndex = ((newPlayer->battingAverage * newPlayer->strikeRate) / 100) + (newPlayer->wickets * 2);
    }

    return performanceIndex;
}

int findTeamIndex(char *teamName)
{
    int startIndex = 0;
    int endIndex = teamCount - 1;

    while (startIndex <= endIndex)
    {
        int mid = startIndex + (endIndex - startIndex) / 2;

        if (strcmp(allTeams[mid].teamName, teamName) == 0)
        {
            return mid;
        }
        else if (strcmp(allTeams[mid].teamName, teamName) > 0)
        {
            endIndex = mid - 1;
        }
        else
        {
            startIndex = mid + 1;
        }
    }

    return -1;
}

void addPlayerToTeam(PlayerModel *newPlayer)
{
    int teamIndex = findTeamIndex(newPlayer->teamName);
    if (teamIndex == -1)
    {
        printf("Team doesn't exist. \n");
        return;
    }

    int totalBatsmanAndAllRounders = 0;

    if (strcmp(newPlayer->role, "Batsman") == 0)
    {
        insertPlayersSortedByPerformanceIndex(&allTeams[teamIndex].batsmenHead, newPlayer);
        allTeams[teamIndex].totalStrikeRate += newPlayer->strikeRate;
        allTeams[teamIndex].totalBatsmen++;
    }
    else if (strcmp(newPlayer->role, "Bowler") == 0)
    {
        insertPlayersSortedByPerformanceIndex(&allTeams[teamIndex].bowlersHead, newPlayer);
    }
    else
    {
        insertPlayersSortedByPerformanceIndex(&allTeams[teamIndex].allRoundersHead, newPlayer);
        allTeams[teamIndex].totalStrikeRate += newPlayer->strikeRate;
        allTeams[teamIndex].totalAllRounders++;
    }

    totalBatsmanAndAllRounders = allTeams[teamIndex].totalAllRounders + allTeams[teamIndex].totalBatsmen;

    if (totalBatsmanAndAllRounders > 0)
    {
        allTeams[teamIndex].averageBattingStrikeRate = allTeams[teamIndex].totalStrikeRate / totalBatsmanAndAllRounders;
    }

    allTeams[teamIndex].totalPlayers++;
}

void initializePlayers()
{
    for (int currentPlayerIndex = 0; currentPlayerIndex < playerCount; currentPlayerIndex++)
    {
        PlayerModel *newPlayer = calloc(1, sizeof(PlayerModel));

        if (newPlayer == NULL)
        {
            printf("Memory allocation failed. \n");
            return;
        }

        newPlayer->next = NULL;

        newPlayer->playerId = players[currentPlayerIndex].id;
        strcpy(newPlayer->playerName, players[currentPlayerIndex].name);
        strcpy(newPlayer->teamName, players[currentPlayerIndex].team);
        strcpy(newPlayer->role, players[currentPlayerIndex].role);
        newPlayer->totalRuns = players[currentPlayerIndex].totalRuns;
        newPlayer->battingAverage = players[currentPlayerIndex].battingAverage;
        newPlayer->strikeRate = players[currentPlayerIndex].strikeRate;
        newPlayer->wickets = players[currentPlayerIndex].wickets;
        newPlayer->economyRate = players[currentPlayerIndex].economyRate;
        newPlayer->performanceIndex = computePerformanceIndexOfPlayer(newPlayer);

        addPlayerToTeam(newPlayer);
    }
}

void initializeTeam()
{
    for (int currentTeamIndex = 0; currentTeamIndex < teamCount; currentTeamIndex++)
    {
        allTeams[currentTeamIndex].teamId = currentTeamIndex + 1;
        strcpy(allTeams[currentTeamIndex].teamName, teams[currentTeamIndex]);
        allTeams[currentTeamIndex].totalPlayers = 0;
        allTeams[currentTeamIndex].batsmenHead = NULL;
        allTeams[currentTeamIndex].bowlersHead = NULL;
        allTeams[currentTeamIndex].allRoundersHead = NULL;
    }
}

bool validateId(char inputTeamId[MAX_ID_LENGTH])
{
    int teamIdLength = strlen(inputTeamId);

    for (int index = 0; index < teamIdLength; index++)
    {
        if (!isdigit((unsigned char)(inputTeamId[index])))
        {
            printf("Team ID must contain digits only. \n");
            cleanInputBuffer();
            return false;
        }
    }

    int numericTeamId = (atoi)(inputTeamId);

    if (numericTeamId < MIN_ID || numericTeamId > MAX_ID)
    {
        printf("Invalid ID. Team ID must be in range (1-1000). \n");
        cleanInputBuffer();
        return false;
    }

    return true;
}

void displayPlayersOfSpecificTeam()
{
    int teamId = 0;
    int teamIndex = 0;

    if (inputAndValidateTeamId(&teamId, &teamIndex) == false)
    {
        return;
    }

    TeamModel *team = &allTeams[teamIndex];

    if (team->totalPlayers == 0)
    {
        printf("No players found in this team. \n");
        return;
    }

    printf("Players of team %s: \n", team->teamName);
    printf("====================================================================================\n");
    printf("%-6s %-20s %-13s %-6s %-6s %-6s %-6s %-6s %-12s \n",
           "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Pref.Index");
    printf("====================================================================================\n");

    PlayerModel *currentPlayer = team->batsmenHead;

    while (currentPlayer != NULL)
    {
        printf("%-6d %-20s %-13s %-6d %-6.2f %-6.2f %-6d %-6.2f %-12.2f\n", currentPlayer->playerId, currentPlayer->playerName, currentPlayer->role,
               currentPlayer->totalRuns, currentPlayer->battingAverage,
               currentPlayer->strikeRate, currentPlayer->wickets,
               currentPlayer->economyRate, currentPlayer->performanceIndex);

        currentPlayer = currentPlayer->next;
    }

    currentPlayer = team->bowlersHead;
    while (currentPlayer != NULL)
    {
        printf("%-6d %-20s %-13s %-6d %-6.2f %-6.2f %-6d %-6.2f %-12.2f\n", currentPlayer->playerId, currentPlayer->playerName, currentPlayer->role,
               currentPlayer->totalRuns, currentPlayer->battingAverage,
               currentPlayer->strikeRate, currentPlayer->wickets,
               currentPlayer->economyRate, currentPlayer->performanceIndex);

        currentPlayer = currentPlayer->next;
    }

    currentPlayer = team->allRoundersHead;
    while (currentPlayer != NULL)
    {
        printf("%-6d %-20s %-13s %-6d %-6.2f %-6.2f %-6d %-6.2f %-12.2f\n", currentPlayer->playerId, currentPlayer->playerName, currentPlayer->role,
               currentPlayer->totalRuns, currentPlayer->battingAverage,
               currentPlayer->strikeRate, currentPlayer->wickets,
               currentPlayer->economyRate, currentPlayer->performanceIndex);

        currentPlayer = currentPlayer->next;
    }

    printf(" ====================================================================================\n");
    printf("Total players: %d \n", team->totalPlayers);
    printf("Average Batting Strike Rate: %.2f \n", team->averageBattingStrikeRate);
}

void swapTeams(TeamModel *firstTeam, TeamModel *secondTeam2)
{
    TeamModel temporaryStore = *firstTeam;
    *firstTeam = *secondTeam2;
    *secondTeam2 = temporaryStore;
}

int partitionBasedOnAverageBattingStrikeRate(TeamModel temporaryTeams[], int startIndex, int endIndex)
{
    float pivot = temporaryTeams[endIndex].averageBattingStrikeRate;
    int index = startIndex - 1;

    for (int currentIndex = startIndex; currentIndex < endIndex; currentIndex++)
    {
        if (temporaryTeams[currentIndex].averageBattingStrikeRate > pivot)
        {
            index++;
            swapTeams(&temporaryTeams[currentIndex], &temporaryTeams[index]);
        }
    }

    index++;
    swapTeams(&temporaryTeams[index], &temporaryTeams[endIndex]);
    return index;
}

void quickSortTeams(TeamModel temporaryTeams[], int startIndex, int endIndex)
{
    if (startIndex >= endIndex)
    {
        return;
    }

    int pivot = partitionBasedOnAverageBattingStrikeRate(temporaryTeams, startIndex, endIndex);

    quickSortTeams(temporaryTeams, startIndex, pivot - 1);
    quickSortTeams(temporaryTeams, pivot + 1, endIndex);
}

void displayTeamsByAverageBattingStrikeRate()
{
    TeamModel temporaryTeams[teamCount];

    for (int index = 0; index < teamCount; index++)
    {
        temporaryTeams[index] = allTeams[index];
    }

    quickSortTeams(temporaryTeams, 0, teamCount - 1);

    printf("Teams Sorted by Average Batting Strike Rate:\n");
    printf("==============================================================================\n");
    printf("%-5s %-20s %-15s %-15s \n", "ID", "Team Name", "Avg Bat SR", "TotalPlayers");
    printf("==============================================================================\n");

    for (int index = 0; index < teamCount; index++)
    {
        printf("%-5d %-20s %-15.2f %-15d \n", temporaryTeams[index].teamId, temporaryTeams[index].teamName,
               temporaryTeams[index].averageBattingStrikeRate, temporaryTeams[index].totalPlayers);
    }

    printf("==============================================================================\n");
}

bool inputAndValidateNumberOfPlayers(int *numberOfPlayers)
{
    printf("Enter number of players: ");
    if (scanf("%d", numberOfPlayers) != 1)
    {
        printf("Number of players must be an integer. \n");
        cleanInputBuffer();
        return false;
    }

    if (*numberOfPlayers < 1 || *numberOfPlayers > MAX_NUMBER_OF_PLATERS_PER_TEAM)
    {
        printf("Number of players must be in range (1-50).");
        return false;
    }

    return true;
}

bool inputAndValidatePlayerRole(int *roleChoice, char *selectedRole)
{
    char takeRoleChoiceInput[MAX_CHOICE_LENGTH];
    printf("Enter Role(1-Batsman, 2-Bowler, 3-All-rounder): ");
    scanf("%s", takeRoleChoiceInput);

    for (int index = 0; index < strlen(takeRoleChoiceInput); index++)
    {
        if (!isdigit((unsigned char)(takeRoleChoiceInput[index])))
        {
            printf("Invalid input. Role choice must be an integer in range (1-3). \n");
            cleanInputBuffer();
            return false;
        }
    }

    *roleChoice = atoi(takeRoleChoiceInput);

    if (*roleChoice == 1)
    {
        strcpy(selectedRole, "Batsman");
    }
    else if (*roleChoice == 2)
    {
        strcpy(selectedRole, "Bowler");
    }
    else if (*roleChoice == 3)
    {
        strcpy(selectedRole, "All-rounder");
    }
    else
    {
        printf("Invalid role choice. Choose an integer in range (1-3). \n");
        cleanInputBuffer();
        return false;
    }

    return true;
}

bool inputAndValidateTeamId(int *teamId, int *teamIndex)
{
    char inputTeamId[MAX_ID_LENGTH];

    printf("Enter Team ID: ");
    scanf("%s", inputTeamId);

    if (!validateId(inputTeamId))
    {
        return false;
    }

    *teamId = atoi(inputTeamId);
    *teamIndex = *teamId - 1;

    if (*teamIndex < MIN_TEAMS - 1 || *teamIndex >= teamCount)
    {
        printf("No team found with id %d. \n", *teamId);
        cleanInputBuffer();
        return false;
    }

    return true;
}

void displayTopKPlayersOfSpecificTeamByRole()
{
    int teamId = 0;
    int teamIndex = 0;
    int roleChoice = 0;
    char selectedRole[MAX_ROLE_LENGTH];
    int numberOfPlayers = 0;
    int numberOfAvailablePlayers = 0;

    if (inputAndValidateTeamId(&teamId, &teamIndex) == false)
    {
        return;
    }

    if (inputAndValidatePlayerRole(&roleChoice, selectedRole) == false)
    {
        return;
    }

    if (inputAndValidateNumberOfPlayers(&numberOfPlayers) == false)
    {
        return;
    }

    numberOfAvailablePlayers = numberOfPlayers;
    TeamModel *team = &allTeams[teamIndex];
    PlayerModel *currentPlayer = NULL;

    if (strcmp(selectedRole, "Batsman") == 0)
    {
        currentPlayer = team->batsmenHead;
    }
    else if (strcmp(selectedRole, "Bowler") == 0)
    {
        currentPlayer = team->bowlersHead;
    }
    else
    {
        currentPlayer = team->allRoundersHead;
    }

    if (currentPlayer == NULL)
    {
        printf("No player found for selected role in this team. \n");
        return;
    }

    printf("Top %d %s of Team %s: \n", numberOfPlayers, selectedRole, team->teamName);
    printf("====================================================================================\n");
    printf("%-6s %-20s %-12s %-6s %-6s %-6s %-6s %-6s %-12s\n",
           "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printf("====================================================================================\n");

    while (currentPlayer != NULL && numberOfAvailablePlayers > 0)
    {
        printf("%-6d %-20s %-12s %-6d %-6.2f %-6.2f %-6d %-6.2f %-12.2f\n",
               currentPlayer->playerId, currentPlayer->playerName, currentPlayer->role,
               currentPlayer->totalRuns, currentPlayer->battingAverage,
               currentPlayer->strikeRate, currentPlayer->wickets,
               currentPlayer->economyRate, currentPlayer->performanceIndex);

        currentPlayer = currentPlayer->next;
        numberOfAvailablePlayers--;
    }

    if (numberOfAvailablePlayers > 0)
    {
        printf("Only %d players are in team for role %s. \n", numberOfPlayers - numberOfAvailablePlayers, selectedRole);
    }

    printf("====================================================================================\n");
}

void swapPlayers(PlayerModel **firstRoleHead, PlayerModel **secondRoleHead)
{
    PlayerModel *temporaryStore = *firstRoleHead;
    *firstRoleHead = *secondRoleHead;
    *secondRoleHead = temporaryStore;
}

int partitionBasedOnPerformanceIndex(PlayerModel *temporaryPlayers[], int startIndex, int endIndex)
{
    float pivot = temporaryPlayers[endIndex]->performanceIndex;
    int index = startIndex - 1;

    for (int currentIndex = startIndex; currentIndex < endIndex; currentIndex++)
    {
        if (temporaryPlayers[currentIndex]->performanceIndex > pivot)
        {
            index++;
            swapPlayers(&temporaryPlayers[currentIndex], &temporaryPlayers[index]);
        }
    }

    index++;
    swapPlayers(&temporaryPlayers[index], &temporaryPlayers[endIndex]);
    return index;
}

void quickSortPlayers(PlayerModel *temporaryPlayers[], int startIndex, int endIndex)
{
    if (startIndex >= endIndex)
    {
        return;
    }

    int pivot = partitionBasedOnPerformanceIndex(temporaryPlayers, startIndex, endIndex);

    quickSortPlayers(temporaryPlayers, startIndex, pivot - 1);
    quickSortPlayers(temporaryPlayers, pivot + 1, endIndex);
}

void displayPlayersOfAllTeamsFilterByRoleAndPerformanceIndex()
{
    int roleChoice = 0;
    char selectedRole[MAX_ROLE_LENGTH];
    int totalRolePlayers = 0;

    if (inputAndValidatePlayerRole(&roleChoice, selectedRole) == false)
    {
        return;
    }

    for (int team = 0; team < teamCount; team++)
    {
        PlayerModel *currentRoleHead = NULL;

        if (strcmp(selectedRole, "Batsman") == 0)
        {
            currentRoleHead = allTeams[team].batsmenHead;
        }
        else if (strcmp(selectedRole, "Bowler") == 0)
        {
            currentRoleHead = allTeams[team].bowlersHead;
        }
        else
        {
            currentRoleHead = allTeams[team].allRoundersHead;
        }

        while (currentRoleHead != NULL)
        {
            totalRolePlayers++;
            currentRoleHead = currentRoleHead->next;
        }
    }

    if (totalRolePlayers == 0)
    {
        printf("No players found for role %s.\n", selectedRole);
        return;
    }

    PlayerModel **allRolePlayers = malloc(totalRolePlayers * sizeof(PlayerModel *));
    int allRolePlayersIndex = 0;

    for (int team = 0; team < teamCount; team++)
    {
        PlayerModel *currentRoleHead = NULL;

        if (strcmp(selectedRole, "Batsman") == 0)
        {
            currentRoleHead = allTeams[team].batsmenHead;
        }
        else if (strcmp(selectedRole, "Bowler") == 0)
        {
            currentRoleHead = allTeams[team].bowlersHead;
        }
        else
        {
            currentRoleHead = allTeams[team].allRoundersHead;
        }

        while (currentRoleHead != NULL)
        {
            allRolePlayers[allRolePlayersIndex] = currentRoleHead;
            allRolePlayersIndex++;
            currentRoleHead = currentRoleHead->next;
        }
    }

    quickSortPlayers(allRolePlayers, 0, totalRolePlayers - 1);

    printf("%s of all teams: \n", selectedRole);
    printf("==============================================================================\n");
    printf("%-6s %-20s %-15s %-12s %-6s %-6s %-6s %-6s %-6s %-6s\n", "ID", "Name", "Team", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf. Index");
    printf("==============================================================================\n");

    for (int i = 0; i < totalRolePlayers; i++)
    {
        PlayerModel *p = allRolePlayers[i];
        printf("%-6d %-20s %-15s %-12s %-6d %-6.2f %-6.2f %-6d %-6.2f %-12.2f\n",
               p->playerId, p->playerName, p->teamName, p->role,
               p->totalRuns, p->battingAverage, p->strikeRate,
               p->wickets, p->economyRate, p->performanceIndex);
    }

    printf("==============================================================================\n");
    free(allRolePlayers);
}

void cleanInputBuffer()
{
    int character;
    while ((character = getchar()) != '\n' && character != EOF)
        ;
}

void freeAllPlayers()
{
    for (int team = 0; team < teamCount; team++)
    {
        PlayerModel *currentRole = allTeams[team].batsmenHead;

        while (currentRole != NULL)
        {
            PlayerModel *storePlayerTemporary = currentRole;
            currentRole = currentRole->next;
            free(storePlayerTemporary);
        }

        currentRole = allTeams[team].bowlersHead;
        while (currentRole != NULL)
        {
            PlayerModel *storePlayerTemporary = currentRole;
            currentRole = currentRole->next;
            free(storePlayerTemporary);
        }

        currentRole = allTeams[team].allRoundersHead;
        while (currentRole != NULL)
        {
            PlayerModel *storePlayerTemporary = currentRole;
            currentRole = currentRole->next;
            free(storePlayerTemporary);
        }
    }
}

void exitMenu()
{
    freeAllPlayers();
    printf("Exiting the program...\n");

    return;
}
