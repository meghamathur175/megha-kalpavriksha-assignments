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
    float averageBattingStrikeRate;
    PlayerModel *teamHead;
} TeamModel;

PlayerModel *playerModelHead = NULL;
TeamModel allTeams[MAX_TEAMS];

void initializePlayers();
void initializeTeam();
void displayPlayersOfSpecificTeam();
void quickSortTeams(TeamModel temporaryTeams[], int startIndex, int endIndex);
int partition(TeamModel temporaryTeams[], int startIndex, int endIndex);
void swapTeams(TeamModel *firstTeam, TeamModel *secondTeam2);
void computeAllTeamsAverageBattingStrikeRate();
void displayTeamsByAverageBattingStrikeRate();
void getTopKPlayers(PlayerModel *allPlayers[], int totalPlayers, int K);
void displayTopKPlayersOfTeamByRole();
void displayPlayersOfAllTeamsFilterByRoleAndPerformanceIndex();
void freeAllPlayers();
void freeAllTeamPlayers();
void exitMenu();

int main()
{
    initializePlayers();
    initializeTeam();
    computeAllTeamsAverageBattingStrikeRate();

    while (1)
    {
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
        if (scanf("%d", &choice) != 1)
        {
            printf("Choice must be an integer value in range (1-6). \n");
            return 0;
        }

        switch (choice)
        {
        case 1:
            displayPlayersOfSpecificTeam();
            break;
        case 2:
            displayTeamsByAverageBattingStrikeRate();
            break;
        case 3:
            displayTopKPlayersOfTeamByRole();
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

        if (strcmp(newPlayer->role, "Batsman") == 0)
        {
            newPlayer->performanceIndex = (newPlayer->battingAverage * newPlayer->strikeRate) / 100;
        }
        else if (strcmp(newPlayer->role, "Bowler") == 0)
        {
            newPlayer->performanceIndex = (newPlayer->wickets * 2) + (100 - newPlayer->economyRate);
        }
        else
        {
            newPlayer->performanceIndex = ((newPlayer->battingAverage * newPlayer->strikeRate) / 100) + (newPlayer->wickets * 2);
        }

        if (playerModelHead == NULL)
        {
            playerModelHead = newPlayer;
        }
        else
        {
            newPlayer->next = playerModelHead;
            playerModelHead = newPlayer;
        }
    }
}

bool isTeamExist(PlayerModel *player, int totalTeamsAdded, int *teamIndex)
{
    for (int currentTeamIndex = 0; currentTeamIndex < totalTeamsAdded; currentTeamIndex++)
    {
        if (strcmp(allTeams[currentTeamIndex].teamName, player->teamName) == 0)
        {
            *teamIndex = currentTeamIndex;
            return true;
        }
    }

    return false;
}

void initializeTeam()
{
    int totalTeamsAdded = 0;
    PlayerModel *currentPlayer = playerModelHead;
    int teamIndex = -1;

    while (currentPlayer != NULL)
    {
        if (isTeamExist(currentPlayer, totalTeamsAdded, &teamIndex) == false)
        {
            teamIndex = totalTeamsAdded;

            allTeams[teamIndex].teamId = totalTeamsAdded + 1;
            strcpy(allTeams[teamIndex].teamName, currentPlayer->teamName);
            allTeams[teamIndex].totalPlayers = 0;
            allTeams[teamIndex].averageBattingStrikeRate = 0.0;
            allTeams[teamIndex].teamHead = NULL;

            totalTeamsAdded++;
        }

        PlayerModel *playerCopy = calloc(1, sizeof(PlayerModel));
        if (playerCopy == NULL)
        {
            printf("Memory allocation failed. \n");
            return;
        }

        playerCopy->playerId = currentPlayer->playerId;
        strcpy(playerCopy->playerName, currentPlayer->playerName);
        strcpy(playerCopy->teamName, currentPlayer->teamName);
        strcpy(playerCopy->role, currentPlayer->role);
        playerCopy->totalRuns = currentPlayer->totalRuns;
        playerCopy->battingAverage = currentPlayer->battingAverage;
        playerCopy->strikeRate = currentPlayer->strikeRate;
        playerCopy->wickets = currentPlayer->wickets;
        playerCopy->economyRate = currentPlayer->economyRate;
        playerCopy->performanceIndex = currentPlayer->performanceIndex;

        playerCopy->next = allTeams[teamIndex].teamHead;
        allTeams[teamIndex].teamHead = playerCopy;
        allTeams[teamIndex].totalPlayers++;

        currentPlayer = currentPlayer->next;
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
            return false;
        }
    }

    int numericTeamId = (atoi)(inputTeamId);

    if (numericTeamId < MIN_ID || numericTeamId > MAX_ID)
    {
        printf("Team ID must be in range (1-1000). \n");
        return false;
    }

    return true;
}

void displayPlayersOfSpecificTeam()
{
    char inputTeamId[MAX_ID_LENGTH];
    int teamId = 0;
    int teamIndex = 0;

    printf("Enter team ID: ");
    scanf("%s", inputTeamId);

    if (validateId(inputTeamId) == false)
    {
        return;
    }

    teamId = atoi(inputTeamId);
    teamIndex = teamId - 1;

    if (teamIndex < 0 || teamIndex >= MAX_TEAMS)
    {
        printf("Invalid team ID. \n");
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

    PlayerModel *currentPlayer = team->teamHead;

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

int partition(TeamModel temporaryTeams[], int startIndex, int endIndex)
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

    int pivot = partition(temporaryTeams, startIndex, endIndex);

    quickSortTeams(temporaryTeams, startIndex, pivot - 1);
    quickSortTeams(temporaryTeams, pivot + 1, endIndex);
}

void computeAllTeamsAverageBattingStrikeRate()
{
    for (int index = 0; index < MAX_TEAMS; index++)
    {
        TeamModel *currentTeam = &allTeams[index];
        float totalStrikeRate = 0;
        float averageStrikeRate = 0;
        int totalBatsmanAndAllRounders = 0;

        PlayerModel *currentPlayer = currentTeam->teamHead;

        while (currentPlayer != NULL)
        {
            if (strcmp(currentPlayer->role, "Batsman") == 0 || strcmp(currentPlayer->role, "All-rounder") == 0)
            {
                totalStrikeRate += currentPlayer->strikeRate;
                totalBatsmanAndAllRounders++;
            }

            currentPlayer = currentPlayer->next;
        }

        if (totalBatsmanAndAllRounders > 0)
        {
            averageStrikeRate = totalStrikeRate / totalBatsmanAndAllRounders;
        }

        currentTeam->averageBattingStrikeRate = averageStrikeRate;
    }
}

void displayTeamsByAverageBattingStrikeRate()
{
    computeAllTeamsAverageBattingStrikeRate();
    TeamModel temporaryTeams[MAX_TEAMS];

    for (int index = 0; index < MAX_TEAMS; index++)
    {
        temporaryTeams[index] = allTeams[index];
    }

    quickSortTeams(temporaryTeams, 0, MAX_TEAMS - 1);

    printf("Teams Sorted by Average Batting Strike Rate:\n");
    printf("==============================================================================\n");
    printf("%-5s %-20s %-15s %-15s \n", "ID", "Team Name", "Avg Bat SR", "TotalPlayers");
    printf("==============================================================================\n");

    for (int index = 0; index < MAX_TEAMS; index++)
    {
        printf("%-5d %-20s %-15.2f %-15d \n", temporaryTeams[index].teamId, temporaryTeams[index].teamName,
               temporaryTeams[index].averageBattingStrikeRate, temporaryTeams[index].totalPlayers);
    }

    printf("==============================================================================\n");
}

void displayTopKPlayersOfTeamByRole()
{
    char inputTeamId[MAX_ID_LENGTH];
    int teamId = 0;
    int teamIndex = 0;
    int roleChoice = 0;
    char selectedRole[MAX_ROLE_LENGTH];
    int numberOfPlayers = 0;

    printf("Enter Team ID: ");
    scanf("%s", inputTeamId);

    if (!validateId(inputTeamId))
    {
        return;
    }

    teamId = atoi(inputTeamId);
    teamIndex = teamId - 1;

    if (teamIndex < MIN_TEAMS - 1 || teamIndex >= MAX_TEAMS)
    {
        printf("Invalid team ID. \n");
        return;
    }

    TeamModel *team = &allTeams[teamIndex];

    printf("Enter Role(1-Batsman, 2-Bowler, 3-All-rounder): ");
    if (scanf("%d", &roleChoice) != 1)
    {
        printf("Invalid input. Role choice must be an integer in range (1-3). \n");
    }

    if (roleChoice == 1)
    {
        strcpy(selectedRole, "Batsman");
    }
    else if (roleChoice == 2)
    {
        strcpy(selectedRole, "Bowler");
    }
    else if (roleChoice == 3)
    {
        strcpy(selectedRole, "All-rounder");
    }
    else
    {
        printf("Invalid role choice. Choose an integer in range (1-3). \n");
        return;
    }

    printf("Enter number of players: ");
    if (scanf("%d", &numberOfPlayers) != 1)
    {
        printf("Number of players must be an integer. \n");
        return;
    }

    if (numberOfPlayers > MAX_NUMBER_OF_PLATERS_PER_TEAM)
    {
        printf("Number of players must be in range (1-50).");
        return;
    }

    PlayerModel *currentPlayer = team->teamHead;
    PlayerModel *filteredPlayers[MAX_NUMBER_OF_PLATERS_PER_TEAM];
    int filteredPlayersIndex = 0;
    int totalPlayersForSelectedRole = 0;

    while (currentPlayer != NULL)
    {
        if (strcmp(currentPlayer->role, selectedRole) == 0)
        {
            totalPlayersForSelectedRole++;
            filteredPlayers[filteredPlayersIndex] = currentPlayer;
            filteredPlayersIndex++;
        }

        currentPlayer = currentPlayer->next;
    }

    if (totalPlayersForSelectedRole == 0)
    {
        printf("No player found for selected role in this team. \n");
        return;
    }

    if (totalPlayersForSelectedRole < numberOfPlayers)
    {
        printf("Team %s doesn't have %d %s. \n", team->teamName, numberOfPlayers, selectedRole);
        return;
    }

    getTopKPlayers(filteredPlayers, totalPlayersForSelectedRole, numberOfPlayers);

    printf("Top %d %s of Team %s: \n", numberOfPlayers, selectedRole, team->teamName);
    printf("====================================================================================\n");
    printf("%-6s %-20s %-12s %-6s %-6s %-6s %-6s %-6s %-12s\n",
           "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printf("====================================================================================\n");

    for (int currentPlayerIndex = 0; currentPlayerIndex < numberOfPlayers; currentPlayerIndex++)
    {
        PlayerModel *player = filteredPlayers[currentPlayerIndex];

        printf("%-6d %-20s %-12s %-6d %-6.2f %-6.2f %-6d %-6.2f %-12.2f\n", player->playerId, player->playerName,
               player->role, player->totalRuns, player->battingAverage, player->strikeRate, player->wickets,
               player->economyRate, player->performanceIndex);
    }

    printf("====================================================================================\n");
}

void getTopKPlayers(PlayerModel *allPlayers[], int totalPlayers, int K)
{
    for (int index = 0; index < K; index++)
    {
        int maximumIndex = index;

        for (int nextIndex = index + 1; nextIndex < totalPlayers; nextIndex++)
        {
            if (allPlayers[nextIndex]->performanceIndex > allPlayers[maximumIndex]->performanceIndex)
            {
                maximumIndex = nextIndex;
            }
        }

        PlayerModel *temp = allPlayers[index];
        allPlayers[index] = allPlayers[maximumIndex];
        allPlayers[maximumIndex] = temp;
    }
}

void displayPlayersOfAllTeamsFilterByRoleAndPerformanceIndex()
{
    int roleChoice = 0;
    char selectedRole[MAX_ROLE_LENGTH];
    int totalPlayersForSelectedRole = 0;
    int totalPlayers = MAX_TEAMS * MAX_NUMBER_OF_PLATERS_PER_TEAM;
    PlayerModel *allPlayerForSelectedRole[totalPlayers];
    int playerIndex = 0;

    printf("Enter Role(1-Batsman, 2-Bowler, 3-All-rounder): ");
    if (scanf("%d", &roleChoice) != 1)
    {
        printf("Invalid input. Role choice must be an integer in range (1-3). \n");
    }

    if (roleChoice == 1)
    {
        strcpy(selectedRole, "Batsman");
    }
    else if (roleChoice == 2)
    {
        strcpy(selectedRole, "Bowler");
    }
    else if (roleChoice == 3)
    {
        strcpy(selectedRole, "All-rounder");
    }
    else
    {
        printf("Invalid role choice. Choose an integer in range (1-3). \n");
        return;
    }

    for (int currentTeamIndex = 0; currentTeamIndex < MAX_TEAMS; currentTeamIndex++)
    {
        PlayerModel *currentPlayer = allTeams[currentTeamIndex].teamHead;

        while (currentPlayer != NULL)
        {
            if (strcmp(currentPlayer->role, selectedRole) == 0)
            {
                totalPlayersForSelectedRole++;
                allPlayerForSelectedRole[playerIndex] = currentPlayer;
                playerIndex++;
            }

            currentPlayer = currentPlayer->next;
        }
    }

    if (totalPlayersForSelectedRole == 0)
    {
        printf("No player found for selected role. \n");
        return;
    }

    getTopKPlayers(allPlayerForSelectedRole, totalPlayersForSelectedRole, totalPlayersForSelectedRole);

    printf("%s of all teams: \n", selectedRole);
    printf("==============================================================================\n");
    printf("%-6s %-20s %-15s %-12s %-6s %-6s %-6s %-6s %-12s \n", "ID", "Name", "Team", "Role", "Avg", "SR", "Wkts",
           "ER", "Perf. Index");
    printf("==============================================================================\n");

    for (int index = 0; index < totalPlayersForSelectedRole; index++)
    {
        PlayerModel *currentPlayer = allPlayerForSelectedRole[index];

        printf("%-6d %-20s %-15s %-12s %-6d %-6.2f %-6.2f %-6d %-12.2f\n", currentPlayer->playerId,
               currentPlayer->playerName, currentPlayer->teamName, currentPlayer->role,
               currentPlayer->totalRuns, currentPlayer->battingAverage, currentPlayer->strikeRate,
               currentPlayer->wickets, currentPlayer->performanceIndex);
    }

    printf("==============================================================================\n");
}

void freeAllPlayers()
{
    PlayerModel *currentPlayer = playerModelHead;

    while (currentPlayer != NULL)
    {
        PlayerModel *temp = currentPlayer;
        currentPlayer = currentPlayer->next;
        free(temp);
    }
}

void freeAllTeamPlayers()
{
    for (int index = 0; index < MAX_TEAMS; index++)
    {
        PlayerModel *currentTeam = allTeams[index].teamHead;

        while (currentTeam != NULL)
        {
            PlayerModel *temp = currentTeam;
            currentTeam = currentTeam->next;
            free(temp);
        }
    }
}

void exitMenu()
{
    freeAllPlayers();
    freeAllTeamPlayers();
    printf("Exiting the program...\n");

    return;
}
