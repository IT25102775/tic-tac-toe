#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PLAYERS 3
#define MAX_SYMBOLS 3
const char symbols[MAX_SYMBOLS] = {'X', 'O', 'Z'};

typedef struct {
    int isComputer;
    char symbol;
} Player;

char **board;
int N;
FILE *logFile;

void initializeBoard(int size) {
    N = size;
    board = (char **)malloc(N * sizeof(char *));
    for (int i = 0; i < N; i++) {
        board[i] = (char *)malloc(N * sizeof(char));
        for (int j = 0; j < N; j++) {
            board[i][j] = ' ';
        }
    }
}

void displayBoard() {
    printf("\n");
    for (int i = 0; i < N; i++) {
        printf(" ");
        for (int j = 0; j < N; j++) {
            printf(" %c ", board[i][j]);
            if (j != N - 1) printf("|");
        }
        printf("\n");
        if (i != N - 1) {
            printf(" ");
            for (int j = 0; j < N; j++) {
                printf("---");
                if (j != N - 1) printf("+");
            }
            printf("\n");
        }
    }
    printf("\n");
}

int validateMove(int row, int col) {
    return (row >= 0 && row < N && col >= 0 && col < N && board[row][col] == ' ');
}

void getUserMove(int *row, int *col, char symbol) {
    do {
        printf("Player '%c', enter your move (row column): ", symbol);
        scanf("%d %d", row, col);
        (*row)--; (*col)--;  // Convert to 0-based indexing
        if (!validateMove(*row, *col)) {
            printf("Invalid move. Try again.\n");
        }
    } while (!validateMove(*row, *col));
}

void getComputerMove(int *row, int *col) {
    do {
        *row = rand() % N;
        *col = rand() % N;
    } while (!validateMove(*row, *col));
    printf("Computer plays at: %d %d\n", *row + 1, *col + 1);
}

void logMove(int playerNum, char symbol, int row, int col) {
    if (logFile) {
        fprintf(logFile, "Player %d (%c) moved to (%d, %d)\n", playerNum + 1, symbol, row + 1, col + 1);
        fflush(logFile);
    }
}

int checkWin(char symbol) {
    // Check rows and columns
    for (int i = 0; i < N; i++) {
        int rowWin = 1, colWin = 1;
        for (int j = 0; j < N; j++) {
            if (board[i][j] != symbol) rowWin = 0;
            if (board[j][i] != symbol) colWin = 0;
        }
        if (rowWin || colWin) return 1;
    }

    // Check diagonals
    int diag1Win = 1, diag2Win = 1;
    for (int i = 0; i < N; i++) {
        if (board[i][i] != symbol) diag1Win = 0;
        if (board[i][N - 1 - i] != symbol) diag2Win = 0;
    }
    return diag1Win || diag2Win;
}

int checkDraw() {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (board[i][j] == ' ') return 0;
    return 1;
}

void configurePlayers(Player players[], int numPlayers) {
    for (int i = 0; i < numPlayers; i++) {
        char choice;
        printf("Is Player %d (%c) a computer? (y/n): ", i + 1, symbols[i]);
        scanf(" %c", &choice);
        players[i].isComputer = (choice == 'y' || choice == 'Y') ? 1 : 0;
        players[i].symbol = symbols[i];
    }
}

void freeBoard() {
    for (int i = 0; i < N; i++)
        free(board[i]);
    free(board);
}

int main() {
    srand(time(NULL));
    int numPlayers;

    printf("Welcome to Multi-Player Tic-Tac-Toe!\n");

    // Board size input
    do {
        printf("Enter board size (3-10): ");
        scanf("%d", &N);
    } while (N < 3 || N > 10);

    // Player count input
    do {
        printf("Select number of players (2 or 3): ");
        scanf("%d", &numPlayers);
    } while (numPlayers != 2 && numPlayers != 3);

    Player players[MAX_PLAYERS];
    configurePlayers(players, numPlayers);
    initializeBoard(N);

    // Open log file
    logFile = fopen("./game_log.txt", "w");
    if (!logFile) {
        perror("Error opening log file");
        printf("Continuing without logging...\n");
    }

    int currentPlayer = 0;
    int gameOver = 0;
    displayBoard();

    while (!gameOver) {
        int row, col;
        Player p = players[currentPlayer];

        printf("Turn: Player %d (%c)\n", currentPlayer + 1, p.symbol);

        if (p.isComputer) {
            getComputerMove(&row, &col);
        } else {
            getUserMove(&row, &col, p.symbol);
        }

        board[row][col] = p.symbol;
        logMove(currentPlayer, p.symbol, row, col);
        displayBoard();

        if (checkWin(p.symbol)) {
            printf("Player %d (%c) WINS!\n", currentPlayer + 1, p.symbol);
            if (logFile)
                fprintf(logFile, "Player %d (%c) wins!\n", currentPlayer + 1, p.symbol);
            gameOver = 1;
        } else if (checkDraw()) {
            printf("The game is a DRAW.\n");
            if (logFile)
                fprintf(logFile, "Game ended in a draw.\n");
            gameOver = 1;
        }

        currentPlayer = (currentPlayer + 1) % numPlayers;
    }

    if (logFile) fclose(logFile);
    freeBoard();
    return 0;
}

