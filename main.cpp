/**
 * ======================================================================================
 * PROJECT:      Ultimate Console Game Hub
 * AUTHOR:       Muhammad Taha
 * DESCRIPTION:  A multi-game console application featuring AI logic, input validation,
 * and a dynamic UI system.
 *
 * FEATURES:
 * - Dice Roll Simulation
 * - Secret Number Guessing (Binary Search Logic)
 * - Tic-Tac-Toe (PvP and PvAI)
 * - Rock, Paper, Scissors
 * - Hangman (String manipulation)
 *
 * GITHUB NOTES:
 * - Uses standard C++ libraries + <windows.h> for console coloring.
 * - Implements a robust input validation engine to prevent runtime crashes.
 * ======================================================================================
 */

#include <iostream>
#include <string>
#include <ctime>
#include <thread>
#include <chrono>
#include <vector>
#include <cctype>
#include <cstdlib>
#include <climits>
#include <algorithm> 

// Platform specific check for Windows console colors
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

// --- CONSTANTS & CONFIGURATION ---
const int COLOR_DEFAULT = 7;
const int COLOR_BLUE    = 11;     // Cyan/Blue
const int COLOR_GREEN   = 10;     // Bright Green
const int COLOR_RED     = 12;     // Bright Red
const int COLOR_YELLOW  = 14;     // Yellow
const int COLOR_PURPLE  = 13;     // Magenta
const int COLOR_CYAN    = 3;      // Dark Cyan

// --- GLOBAL STATE ---
// Note: In larger enterprise apps, we would wrap these in a Class or Struct.
char board[9] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };

// --- FUNCTION PROTOTYPES ---

// UI & System
void setColor(int color);
void drawHeader(string title);
void drawDivider();
void loadingScreen(string message);
void clearScreen();
void pauseGame();

// Input Validation Engine
int getValidatedInt(string prompt, int min, int max);

// Game Modules
void dice_roll();
void secret_numbers();
void tic_tac_toe_menu();
void rock_paper_scissors();
void hangman_game();

// Logic Helpers
void tic_tac_toe_pvp();
void tic_tac_toe_pvc();
void show_board();
char check_winner();
void clearboard();
void computer_turn(); 
bool place_marker(int slot, char marker);
void drawHangman(int lives);

/**
 * ======================================================================================
 * MAIN ENTRY POINT
 * ======================================================================================
 */
int main() {
    #ifdef _WIN32
    system("title Ultimate Console Game Hub - Dev: Muhammad Taha");
    #endif
    
    // Seed the random number generator once at startup
    srand(static_cast<unsigned int>(time(0))); 

    loadingScreen("INITIALIZING KERNEL");

    while (true) {
        clearScreen();
        drawHeader("MAIN MENU");
        
        setColor(COLOR_BLUE); cout << "\t[1] "; setColor(COLOR_DEFAULT); cout << "Dice Roll Challenge\n";
        setColor(COLOR_BLUE); cout << "\t[2] "; setColor(COLOR_DEFAULT); cout << "Secret Number Guessing\n";
        setColor(COLOR_BLUE); cout << "\t[3] "; setColor(COLOR_DEFAULT); cout << "Tic-Tac-Toe (PvP & PvCPU)\n";
        setColor(COLOR_BLUE); cout << "\t[4] "; setColor(COLOR_DEFAULT); cout << "Rock, Paper, Scissors\n";
        setColor(COLOR_BLUE); cout << "\t[5] "; setColor(COLOR_DEFAULT); cout << "Hangman (Word Survival)\n";
        
        drawDivider();
        setColor(COLOR_RED);  cout << "\t[0] "; setColor(COLOR_DEFAULT); cout << "Exit Application\n";
        
        int choice = getValidatedInt("\n\tSelect Module > ", 0, 5);

        switch (choice) {
            case 1: dice_roll(); break;
            case 2: secret_numbers(); break;
            case 3: tic_tac_toe_menu(); break;
            case 4: rock_paper_scissors(); break;
            case 5: hangman_game(); break;
            case 0:
                setColor(COLOR_GREEN);
                cout << "\n\tTerminating session. Goodbye!\n";
                setColor(COLOR_DEFAULT);
                this_thread::sleep_for(chrono::seconds(1));
                return 0;
        }
    }
    return 0;
}

/**
 * ======================================================================================
 * INPUT VALIDATION ENGINE
 * Ensures the program never crashes due to invalid data types (char vs int).
 * ======================================================================================
 */
int getValidatedInt(string prompt, int min, int max) {
    while (true) {
        cout << prompt;
        string input;
        getline(cin, input);

        // 1. Empty Check
        if (input.empty()) {
            setColor(COLOR_RED); cout << "\t[!] Input required.\n"; setColor(COLOR_DEFAULT);
            continue;
        }

        // 2. Numeric Check
        bool isNumber = true;
        if (input[0] == '-') isNumber = false; 
        for (char c : input) {
            if (!isdigit(c)) {
                isNumber = false;
                break;
            }
        }

        if (!isNumber) {
            setColor(COLOR_RED); cout << "\t[!] Invalid format. Numbers only.\n"; setColor(COLOR_DEFAULT);
            continue;
        }

        // 3. Range Check
        try {
            int value = stoi(input);
            if (value >= min && value <= max) {
                return value;
            } else {
                setColor(COLOR_RED); 
                cout << "\t[!] Range Error: Enter " << min << "-" << max << ".\n"; 
                setColor(COLOR_DEFAULT);
            }
        } catch (...) {
            setColor(COLOR_RED); cout << "\t[!] Overflow Error.\n"; setColor(COLOR_DEFAULT);
        }
    }
}

// --- UI & GRAPHICS FUNCTIONS ---

void setColor(int color) {
    #ifdef _WIN32
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
    #endif
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

/**
 * Draws the persistent Developer Branding and the specific screen title.
 * This ensures "Muhammad Taha" is visible on every screen.
 */
void drawHeader(string title) {
    // --- STYLISH LEFT-ALIGNED BRANDING ---
    setColor(COLOR_CYAN);
    cout << "\n  // DEV: MUHAMMAD TAHA // \n";
    setColor(COLOR_DEFAULT);

    // --- CENTERED TITLE ---
    setColor(COLOR_PURPLE);
    cout << "\t=========================================\n";
    cout << "\t   " << title << "\n";
    cout << "\t=========================================\n\n";
    setColor(COLOR_DEFAULT);
}

void drawDivider() {
    setColor(COLOR_PURPLE);
    cout << "\n\t-----------------------------------------";
    setColor(COLOR_DEFAULT);
}

void pauseGame() {
    cout << "\n\tPress [ENTER] to return...";
    cin.get();
}

void loadingScreen(string message) {
    cout << "\n\n\t" << message;
    for(int i=0; i<3; i++) {
        cout << ".";
        this_thread::sleep_for(chrono::milliseconds(200));
    }
    clearScreen();
}

/**
 * ======================================================================================
 * GAME MODULES
 * ======================================================================================
 */

// --- 1. DICE ROLL ---
void dice_roll() {
    while (true) {
        clearScreen();
        drawHeader("DICE SIMULATOR");
        cout << "\t[1] Roll Dice\n\t[0] Return\n";
        
        int choice = getValidatedInt("\n\tAction > ", 0, 1);
        if (choice == 0) break;

        setColor(COLOR_YELLOW); cout << "\n\tRolling physics..."; 
        this_thread::sleep_for(chrono::milliseconds(500));
        
        int d1 = rand() % 6 + 1;
        int d2 = rand() % 6 + 1;
        
        cout << "\r\t[ DIE 1: " << d1 << " ]   [ DIE 2: " << d2 << " ]     \n"; 
        
        if (d1 == d2) {
            setColor(COLOR_GREEN); cout << "\n\t>>> CRITICAL HIT! DOUBLES! <<<\n";
        } else {
            setColor(COLOR_RED); cout << "\n\tNo match.\n";
        }
        setColor(COLOR_DEFAULT);
        pauseGame();
    }
}

// --- 2. SECRET NUMBERS ---
void secret_numbers() {
    clearScreen();
    drawHeader("BINARY SEARCH GAME");
    
    int secret = rand() % 100 + 1;
    int attempts = 0;
    
    cout << "\tTarget Locked: Number between 1-100.\n";

    while(true) {
        int guess = getValidatedInt("\n\tInput Guess > ", 1, 100);
        attempts++;

        if (guess == secret) {
            setColor(COLOR_GREEN);
            cout << "\n\t[SUCCESS] Target neutralized in " << attempts << " attempts!\n";
            setColor(COLOR_DEFAULT);
            break;
        } else if (guess < secret) {
            setColor(COLOR_YELLOW); cout << "\t>>> Too Low. Adjust upwards.\n"; setColor(COLOR_DEFAULT);
        } else {
            setColor(COLOR_YELLOW); cout << "\t>>> Too High. Adjust downwards.\n"; setColor(COLOR_DEFAULT);
        }
    }
    pauseGame();
}

// --- 3. TIC TAC TOE LOGIC ---

void tic_tac_toe_menu() {
    while(true) {
        clearScreen();
        drawHeader("STRATEGY ARENA (TTT)");
        cout << "\t[1] PvHuman\n";
        cout << "\t[2] PvAI (CPU)\n";
        cout << "\t[0] Return\n";
        
        int choice = getValidatedInt("\n\tSelect Mode > ", 0, 2);

        if(choice == 0) break;
        
        clearboard();
        if(choice == 1) tic_tac_toe_pvp();
        else tic_tac_toe_pvc();
    }
}

void show_board() {
    setColor(COLOR_BLUE);
    cout << "\n\t     |     |     " << endl;
    cout << "\t  " << board[0] << "  |  " << board[1] << "  |  " << board[2] << "  " << endl;
    cout << "\t_____|_____|_____" << endl;
    cout << "\t     |     |     " << endl;
    cout << "\t  " << board[3] << "  |  " << board[4] << "  |  " << board[5] << "  " << endl;
    cout << "\t_____|_____|_____" << endl;
    cout << "\t     |     |     " << endl;
    cout << "\t  " << board[6] << "  |  " << board[7] << "  |  " << board[8] << "  " << endl;
    cout << "\t     |     |     " << endl << endl;
    setColor(COLOR_DEFAULT);
}

bool place_marker(int slot, char marker) {
    if(board[slot-1] == ' ') {
        board[slot-1] = marker;
        return true;
    }
    return false;
}

void tic_tac_toe_pvp() {
    char currentPlayer = 'X';
    while(true) {
        clearScreen();
        drawHeader("PvP MATCH");
        show_board();
        
        cout << "\tPlayer " << currentPlayer << "'s turn.";
        int slot = getValidatedInt("\n\tSelect Sector (1-9) > ", 1, 9);

        if (place_marker(slot, currentPlayer)) {
            char winner = check_winner();
            if (winner != 'C') {
                clearScreen();
                drawHeader("GAME OVER");
                show_board();
                if (winner == 'D') { setColor(COLOR_YELLOW); cout << "\n\tSTALEMATE (DRAW)!\n"; }
                else { setColor(COLOR_GREEN); cout << "\n\tPLAYER " << winner << " DOMINATED!\n"; }
                setColor(COLOR_DEFAULT);
                pauseGame();
                return;
            }
            currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
        } else {
            setColor(COLOR_RED); cout << "\tSector Occupied!\n"; setColor(COLOR_DEFAULT);
            this_thread::sleep_for(chrono::milliseconds(500));
        }
    }
}

void tic_tac_toe_pvc() {
    while(true) {
        clearScreen();
        drawHeader("MAN VS MACHINE");
        show_board();
        
        // Human Move
        int slot = getValidatedInt("\n\tYour Command (1-9) > ", 1, 9);

        if (!place_marker(slot, 'X')) {
            cout << "\n\tSector Invalid!";
            this_thread::sleep_for(chrono::milliseconds(500));
            continue;
        }

        if (check_winner() != 'C') break;

        // AI Move
        cout << "\n\tAI Calculating...";
        this_thread::sleep_for(chrono::milliseconds(600));
        computer_turn();

        if (check_winner() != 'C') break;
    }
    
    clearScreen();
    drawHeader("GAME RESULT");
    show_board();
    char winner = check_winner();
    if(winner == 'X') { setColor(COLOR_GREEN); cout << "\n\tHUMANITY WINS!\n"; }
    else if(winner == 'O') { setColor(COLOR_RED); cout << "\n\tMACHINE DOMINATION!\n"; }
    else { setColor(COLOR_YELLOW); cout << "\n\tTACTICAL DRAW.\n"; }
    setColor(COLOR_DEFAULT);
    pauseGame();
}

// Simple AI: Prioritizes center, then random available spots
void computer_turn() {
    if(board[4] == ' ') { board[4] = 'O'; return; } 

    int slot;
    while(true) {
        slot = rand() % 9;
        if (board[slot] == ' ') {
            board[slot] = 'O';
            break;
        }
    }
}

char check_winner() {
    // Horizontal & Vertical Checks
    for(int i=0; i<3; i++) {
        if(board[i*3] == board[i*3+1] && board[i*3+1] == board[i*3+2] && board[i*3] != ' ') return board[i*3];
        if(board[i] == board[i+3] && board[i+3] == board[i+6] && board[i] != ' ') return board[i];
    }
    // Diagonal Checks
    if (board[0] == board[4] && board[4] == board[8] && board[0] != ' ') return board[0];
    if (board[2] == board[4] && board[4] == board[6] && board[2] != ' ') return board[2];

    int filled = 0;
    for(int i=0; i<9; i++) if(board[i] != ' ') filled++;
    
    if (filled == 9) return 'D'; // Draw
    return 'C'; // Continue
}

void clearboard() {
    for (int i = 0; i < 9; i++) board[i] = ' ';
}

// --- 4. ROCK PAPER SCISSORS ---

void rock_paper_scissors() {
    string moves[3] = {"Rock", "Paper", "Scissors"};
    while(true) {
        clearScreen();
        drawHeader("R.P.S BATTLE");
        
        cout << "\t[1] Rock\n\t[2] Paper\n\t[3] Scissors\n\t[0] Return\n";
        
        int pMove = getValidatedInt("\n\tWeapon Choice > ", 0, 3);
        if (pMove == 0) break;
        pMove--; // Convert to 0-index

        cout << "\n\tYou deployed: " << moves[pMove] << "\n";
        
        int cMove = rand() % 3;
        cout << "\tCPU deployed: " << moves[cMove] << "\n";
        
        this_thread::sleep_for(chrono::milliseconds(500));
        drawDivider();

        if (pMove == cMove) {
            setColor(COLOR_YELLOW); cout << "\n\tEFFECT: NO DAMAGE (TIE)\n";
        }
        else if ((pMove == 0 && cMove == 2) || (pMove == 1 && cMove == 0) || (pMove == 2 && cMove == 1)) {
            setColor(COLOR_GREEN); cout << "\n\tEFFECT: CRITICAL HIT (WIN)\n";
        } else {
            setColor(COLOR_RED); cout << "\n\tEFFECT: DEFEAT\n";
        }
        setColor(COLOR_DEFAULT);
        pauseGame();
    }
}

// --- 5. HANGMAN ---

void drawHangman(int lives) {
    setColor(COLOR_RED);
    cout << "\n\t  _______";
    cout << "\n\t  |     |";
    cout << "\n\t  |     " << (lives < 6 ? "O" : "");
    cout << "\n\t  |    " << (lives < 4 ? "/" : " ") << (lives < 5 ? "|" : "") << (lives < 3 ? "\\" : "");
    cout << "\n\t  |    " << (lives < 2 ? "/" : " ") << " " << (lives < 1 ? "\\" : "");
    cout << "\n\t__|__\n";
    setColor(COLOR_DEFAULT);
}

void hangman_game() {
    // Dictionary of possible words
    vector<string> words = {"PROGRAMMING", "COMPUTER", "KEYBOARD", "DEVELOPER", "ALGORITHM", "VARIABLE", "POINTER"};
    string secretWord = words[rand() % words.size()];
    string guessWord(secretWord.length(), '_');
    int lives = 6;
    vector<char> guessedChars;

    while (lives > 0 && guessWord != secretWord) {
        clearScreen();
        drawHeader("HANGMAN SURVIVAL");
        drawHangman(lives);
        
        cout << "\n\tLives: " << lives;
        cout << "\n\tWord:  ";
        
        setColor(COLOR_BLUE);
        for(char c : guessWord) cout << c << " ";
        setColor(COLOR_DEFAULT);

        cout << "\n\n\tHistory: ";
        for(char c : guessedChars) cout << c << " ";

        cout << "\n\n\tEnter Char > ";
        string input;
        getline(cin, input);

        if(input.length() != 1 || !isalpha(input[0])) {
            cout << "\t[!] Single letter input required.";
            this_thread::sleep_for(chrono::seconds(1));
            continue;
        }

        char guess = toupper(input[0]);
        
        bool alreadyGuessed = false;
        for(char c : guessedChars) if(c == guess) alreadyGuessed = true;
        
        if(alreadyGuessed) {
            cout << "\t[!] Already attempted.";
            this_thread::sleep_for(chrono::seconds(1));
            continue;
        }

        guessedChars.push_back(guess);

        bool found = false;
        for (size_t i = 0; i < secretWord.length(); i++) {
            if (secretWord[i] == guess) {
                guessWord[i] = guess;
                found = true;
            }
        }

        if (found) {
            setColor(COLOR_GREEN); cout << "\n\tMatch Found!"; setColor(COLOR_DEFAULT);
        } else {
            setColor(COLOR_RED); cout << "\n\tIncorrect!"; setColor(COLOR_DEFAULT);
            lives--;
        }
        this_thread::sleep_for(chrono::milliseconds(800));
    }

    clearScreen();
    drawHeader(lives > 0 ? "MISSION ACCOMPLISHED" : "MISSION FAILED");
    drawHangman(lives);
    
    if (lives > 0) {
        setColor(COLOR_GREEN);
        cout << "\n\tYou survived! Word: " << secretWord << "\n";
    } else {
        setColor(COLOR_RED);
        cout << "\n\tEliminated. Word: " << secretWord << "\n";
    }
    setColor(COLOR_DEFAULT);
    pauseGame();
}