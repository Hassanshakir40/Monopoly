#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PLAYERS 4
#define MAX_PROPERTIES 40

typedef struct
{
    char name[20];
    int position;
    int money;
    int properties[MAX_PROPERTIES];
    int isInJail;
} Player;

typedef struct
{
    char name[20];
    int cost;
    int rent;
    int owner; // -1 if unowned
} Property;

typedef struct
{
    char description[100];
    int moneyChange;
    int moveToPosition;
} Card;

// Function to initialize the properties on the board
void initializeProperties(Property properties[], int size)
{
    for (int i = 0; i < size; i++)
    {
        sprintf(properties[i].name, "P%d", i + 1); // Assign property names dynamically
        properties[i].cost = (i + 1) * 10;         // Example cost formula
        properties[i].rent = (i + 1) * 5;          // Example rent formula
        properties[i].owner = -1;                  // No owner initially
    }
}

// Function to initialize Chance and Community Chest cards
void initializeCards(Card chance[], Card community[])
{
    strcpy(chance[0].description, "Go to Jail");
    chance[0].moneyChange = 0;
    chance[0].moveToPosition = 10;

    strcpy(chance[1].description, "Collect $20");
    chance[1].moneyChange = 20;
    chance[1].moveToPosition = -1;

    strcpy(chance[2].description, "Pay $50 fine");
    chance[2].moneyChange = -50;
    chance[2].moveToPosition = -1;

    strcpy(chance[3].description, "Go to Start, collect $30");
    chance[3].moneyChange = 30;
    chance[3].moveToPosition = 0;

    strcpy(chance[4].description, "Pay $150 fine");
    chance[4].moneyChange = -150;
    chance[4].moveToPosition = -1;

    strcpy(community[0].description, "Bank error, collect $100");
    community[0].moneyChange = 100;
    community[0].moveToPosition = -1;

    strcpy(community[1].description, "Go to Start, collect $30");
    community[1].moneyChange = 30;
    community[1].moveToPosition = 0;

    strcpy(community[2].description, "Pay hospital fees $100");
    community[2].moneyChange = -100;
    community[2].moveToPosition = -1;

    strcpy(community[3].description, "You helped an old lady cross the road, you win $150");
    community[3].moneyChange = 150;
    community[3].moveToPosition = -1;

    strcpy(community[4].description, "You befriended Ilsa, pay $200 fine");
    community[4].moneyChange = -200;
    community[4].moveToPosition = -1;
}

// Function to roll dice
int rollDice()
{
    return rand() % 6 + 1; // Random number between 1 and 6
}

// Function to display the board as a Monopoly-like grid
void displayBoard(Player players[], int numPlayers, int boardSize)
{
    char board[40][10]; // Array to store board symbols

    // Initialize default board markers
    for (int i = 0; i < boardSize; i++)
    {
        if (i == 0)
            strcpy(board[i], "GO"); // Start position
        else if (i == 10)
            strcpy(board[i], "J"); // Jail
        else if (i == 20)
            strcpy(board[i], "FP"); // Free Parking
        else if (i == 30)
            strcpy(board[i], "GJ"); // Go to Jail
        else if (i == 5 || i == 25)
            strcpy(board[i], "CS"); // Chance Space
        else if (i == 15 || i == 35)
            strcpy(board[i], "CC"); // Community Chest
        else
            sprintf(board[i], "P%d", i + 1); // Generic property marker
    }

    // Overlay players onto the board
    for (int i = 0; i < numPlayers; i++)
    {
        int pos = players[i].position;

        // Create a marker for each player (e.g., PL1, PL2, etc.)
        char playerMarker[5];
        sprintf(playerMarker, "PL%d", i + 1);

        // Replace the marker on the board with the player marker
        // If multiple players are on the same position, we append the markers
        if (strstr(board[pos], "PL") == NULL) // No existing player marker
        {
            strcpy(board[pos], playerMarker);
        }
        else
        { // Add multiple players at the same position
            strcat(board[pos], playerMarker);
        }
    }

    // Display the board
    printf("\n");

    // Top row of the board
    for (int i = 0; i < 11; i++)
        printf("[%s] ", board[i]);
    printf("\n");

    // Left and Right sides
    for (int i = 39; i >= 30; i--)
    {
        printf("[%s]", board[i]);   // Left side
        for (int j = 0; j < 9; j++) // Empty spaces in between
            printf("      ");
        printf("[%s]\n", board[11 + 39 - i]); // Right side
    }

    // Bottom row of the board
    for (int i = 29; i >= 20; i--)
        printf("[%s] ", board[i]);
    printf("\n");
}

// Function to move a player around the board
void movePlayer(Player *player, int boardSize)
{
    int dice = rollDice();
    printf("%s rolled a %d!\n", player->name, dice);
    player->position = (player->position + dice) % boardSize;
    printf("%s moved to position %d\n", player->name, player->position);
}

// Function to handle a player landing on a property
void handleProperty(Player *player, Property properties[], Player players[])
{
    Property *currentProperty = &properties[player->position];
    if (currentProperty->owner == -1)
    {
        printf("%s landed on %s. It's available for $%d. Do you want to buy it? (1 for Yes, 0 for No)\n",
               player->name, currentProperty->name, currentProperty->cost);
        int choice;
        scanf("%d", &choice); 
        if(choice != 1 && choice != 0){
            printf("Please Enter a valid input \n Enter Again : ");
            scanf("%d", &choice);
        }
        if (choice == 1 && player->money >= currentProperty->cost)
        {
            player->money -= currentProperty->cost;
            currentProperty->owner = player - players;
            printf("%s bought %s for $%d\n", player->name, currentProperty->name, currentProperty->cost);
        }
        else
        {
            printf("You don't have enough money or chose not to buy.\n");
        }
    }
    else if (currentProperty->owner != (player - players))
    {
        int ownerIndex = currentProperty->owner;
        int rent = currentProperty->rent;
        printf("%s landed on %s owned by %s. You must pay rent of $%d\n",
               player->name, currentProperty->name, players[ownerIndex].name, rent);
        if (player->money >= rent)
        {
            player->money -= rent;
            players[ownerIndex].money += rent;
            printf("%s paid $%d to %s. Remaining money: $%d\n",
                   player->name, rent, players[ownerIndex].name, player->money);
        }
        else
        {
            printf("You don't have enough money to pay rent! You're bankrupt!\n");
            player->money = 0;
        }
    }
    else
    {
        printf("You landed on your own property.\n");
    }
}

// Function to draw a random Chance or Community Chest card
void drawCard(Card deck[], Player *player)
{
    int cardIndex = rand() % 5;
    Card *drawnCard = &deck[cardIndex];
    printf("%s drew a card: %s\n", player->name, drawnCard->description);
    player->money += drawnCard->moneyChange;
    if (drawnCard->moveToPosition != -1)
    {
        player->position = drawnCard->moveToPosition;
    }
}

// Function to check if the game is over
int checkIfGameOver(Player players[], int numPlayers)
{
    int activePlayers = 0;
    for (int i = 0; i < numPlayers; i++)
    {
        if (players[i].money > 0)
        {
            activePlayers++;
        }
    }
    return activePlayers <= 1; // Game ends if only one player has money
}

void showInstructions()
{
    printf("\n--- Monopoly Game Instructions ---\n");
    printf("1. Each player starts with $1500 and takes turns rolling the dice.\n");
    printf("2. Players move around the board based on their dice rolls.\n");
    printf("3. Landing on properties allows you to buy them if unowned or pay rent if owned.\n");
    printf("4. Special spaces:\n");
    printf("   - 'GO': Collect $200 when you pass.\n");
    printf("   - 'J': Jail. Landing here does nothing, but 'Go to Jail' sends you here.\n");
    printf("   - 'FP': Free Parking. Just a resting spot.\n");
    printf("   - 'GJ': Go to Jail. Sends you directly to Jail.\n");
    printf("   - 'CS': Chance. Draw a random card.\n");
    printf("   - 'CC': Community Chest. Draw a random card.\n");
    printf("5. The game ends when all but one player are bankrupt. The remaining player wins.\n");
    printf("6. Manage your money wisely and strategize to win!\n");
    printf("----------------------------------\n\n");
}
int main()
{
    srand(time(0));

    printf("Welcome to Monopoly Game!\n");
    printf("Choose an option:\n");
    printf("1. Start Game\n");
    printf("2. Instructions\n");
    printf("Enter your choice: ");

    int choice;
    scanf("%d", &choice);

    if (choice == 2)
    {
        showInstructions();
        printf("\nWould you like to start the game now? (1 for Yes, 0 for No): ");
        scanf("%d", &choice);
        if (choice != 1)
        {
            printf("Exiting the game. Goodbye!\n");
            return 0;
        }
    }
    else if (choice != 1)
    {
        printf("Invalid choice. Exiting...\n");
        return 0;
    }

    int boardSize = 40;
    int numPlayers;

    printf("Enter number of players (2-4): ");
    scanf("%d", &numPlayers);

    if (numPlayers < 2 || numPlayers > 4)
    {
        printf("Invalid number of players. Exiting...\n");
        return 1;
    }

    Player players[MAX_PLAYERS];
    Property properties[MAX_PROPERTIES];
    Card chance[5], community[5];

    initializeProperties(properties, boardSize);
    initializeCards(chance, community);

    for (int i = 0; i < numPlayers; i++)
    {
        printf("Enter name for player %d: ", i + 1);
        scanf("%s", players[i].name);
        players[i].position = 0;
        players[i].money = 1500; // Starting money
        players[i].isInJail = 0;
        memset(players[i].properties, 0, sizeof(players[i].properties));
    }

    int gameOver = 0;
    while (!gameOver)
    {
        for (int i = 0; i < numPlayers; i++)
        {
            if (players[i].money > 0)
            {
                printf("\n%s's turn:\n", players[i].name);
                printf("\n%s's money:%d\n", players[i].name,players[i].money);
                movePlayer(&players[i], boardSize);
                if (players[i].position == 5 || players[i].position == 25)
                {
                    drawCard(chance, &players[i]);
                }
                else if (players[i].position == 15 || players[i].position == 35)
                {
                    drawCard(community, &players[i]);
                }
                else if (players[i].position != 10 && players[i].position != 20 && players[i].position != 30)
                {
                    handleProperty(&players[i], properties, players);
                }
                else if (players[i].position == 30)
                {
                    printf("Go to Jail! Moving to position 10 , fine $150.\n");
                    players[i].position = 10;
                    players[i].isInJail = 1;
                    players[i].money=players[i].money-150;
                }
                displayBoard(players, numPlayers, boardSize);
            }
        }
        gameOver = checkIfGameOver(players, numPlayers);
    }

    printf("Game over! The winner is ");
    for (int i = 0; i < numPlayers; i++)
    {
        if (players[i].money > 0)
        {
            printf("%s!\n", players[i].name);
            break;
        }
    }

    return 0;
}
