#include <iostream>
#include <string>
#include <vector>

using namespace std;

// --- Step 1: Enums ---

enum class Symbol { X, O, EMPTY };

string symbolToString(Symbol s) {
    switch (s) {
        case Symbol::X:
            return "X";
        case Symbol::O:
            return "O";
        case Symbol::EMPTY:
            return "-";
        default:
            return "?";
    }
}

enum class GameStatus { IN_PROGRESS, WINNER_X, WINNER_O, DRAW };

// --- Step 2: Data Classes ---

class Cell {
    Symbol symbol;

public:
    Cell() : symbol(Symbol::EMPTY) {}

    bool isEmpty() const { return symbol == Symbol::EMPTY; }

    void setSymbol(Symbol s) { symbol = s; }

    Symbol getSymbol() const { return symbol; }
};

class Player {
    string name;
    Symbol symbol;

public:
    Player(string name, Symbol symbol) : name(name), symbol(symbol) {}

    string getName() const { return name; }

    Symbol getSymbol() const { return symbol; }
};

// --- Step 3: Board Class ---

class Board {
    int size;
    vector<vector<Cell>> grid;

public:
    Board(int size = 3) : size(size) { grid.resize(size, vector<Cell>(size)); }

    bool makeMove(int row, int col, Symbol symbol) {
        if (row < 0 || row >= size || col < 0 || col >= size || !grid[row][col].isEmpty()) {
            return false;
        }
        grid[row][col].setSymbol(symbol);
        return true;
    }

    bool isFull() const {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (grid[i][j].isEmpty())
                    return false;
            }
        }
        return true;
    }

    void display() const {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                cout << " " << symbolToString(grid[i][j].getSymbol()) << " ";
                if (j < size - 1)
                    cout << "|";
            }
            cout << endl;
            if (i < size - 1) {
                for (int j = 0; j < size; j++) {
                    cout << "---";
                    if (j < size - 1)
                        cout << "+";
                }
                cout << endl;
            }
        }
    }

    int getSize() const { return size; }

    Cell getCell(int row, int col) const { return grid[row][col]; }
};

// --- Step 4: Interfaces ---

class WinningStrategy {
public:
    virtual bool checkWin(const Board &board, int row, int col, Symbol symbol) = 0;
    virtual ~WinningStrategy() {}
};

class GameObserver {
public:
    virtual void onGameEnd(Symbol winnerSymbol) = 0;
    virtual ~GameObserver() {}
};

// --- Step 5: Strategy Implementations ---

class RowWinStrategy : public WinningStrategy {
public:
    bool checkWin(const Board &board, int row, int col, Symbol symbol) override {
        int size = board.getSize();
        for (int j = 0; j < size; j++) {
            if (board.getCell(row, j).getSymbol() != symbol)
                return false;
        }
        return true;
    }
};

class ColWinStrategy : public WinningStrategy {
public:
    bool checkWin(const Board &board, int row, int col, Symbol symbol) override {
        int size = board.getSize();
        for (int i = 0; i < size; i++) {
            if (board.getCell(i, col).getSymbol() != symbol)
                return false;
        }
        return true;
    }
};

class DiagonalWinStrategy : public WinningStrategy {
public:
    bool checkWin(const Board &board, int row, int col, Symbol symbol) override {
        int size = board.getSize();

        // Main diagonal
        if (row == col) {
            bool win = true;
            for (int i = 0; i < size; i++) {
                if (board.getCell(i, i).getSymbol() != symbol) {
                    win = false;
                    break;
                }
            }
            if (win)
                return true;
        }

        // Anti-diagonal
        if (row + col == size - 1) {
            bool win = true;
            for (int i = 0; i < size; i++) {
                if (board.getCell(i, size - 1 - i).getSymbol() != symbol) {
                    win = false;
                    break;
                }
            }
            if (win)
                return true;
        }

        return false;
    }
};

// --- Step 6: Scoreboard Class (Observer) ---

class Scoreboard : public GameObserver {
    int xWins = 0;
    int oWins = 0;
    int draws = 0;

public:
    void onGameEnd(Symbol winnerSymbol) override {
        if (winnerSymbol == Symbol::X)
            xWins++;
        else if (winnerSymbol == Symbol::O)
            oWins++;
        else
            draws++;
        display();
    }

    void display() const {
        cout << "\n--- SCOREBOARD ---" << endl;
        cout << "X Wins: " << xWins << " | O Wins: " << oWins << " | Draws: " << draws << endl;
        cout << "------------------\n" << endl;
    }
};

// --- Step 7: Game Class ---

class Game {
    Board board;
    vector<Player> players;
    int currentPlayerIndex;
    GameStatus status;
    vector<WinningStrategy *> strategies;
    vector<GameObserver *> observers;

    void notifyObservers(Symbol winner) {
        for (auto observer : observers) {
            observer->onGameEnd(winner);
        }
    }

public:
    Game(Board b, vector<Player> p)
        : board(b), players(p), currentPlayerIndex(0), status(GameStatus::IN_PROGRESS) {}

    void addStrategy(WinningStrategy *s) { strategies.push_back(s); }
    void addObserver(GameObserver *o) { observers.push_back(o); }

    bool makeMove(int row, int col) {
        if (status != GameStatus::IN_PROGRESS)
            return false;

        Player &currentPlayer = players[currentPlayerIndex];
        if (board.makeMove(row, col, currentPlayer.getSymbol())) {

            // Check win
            for (auto strategy : strategies) {
                if (strategy->checkWin(board, row, col, currentPlayer.getSymbol())) {
                    status = (currentPlayer.getSymbol() == Symbol::X) ? GameStatus::WINNER_X
                                                                      : GameStatus::WINNER_O;
                    notifyObservers(currentPlayer.getSymbol());
                    return true;
                }
            }

            // Check draw
            if (board.isFull()) {
                status = GameStatus::DRAW;
                notifyObservers(Symbol::EMPTY);
                return true;
            }

            // Next turn
            currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
            return true;
        }
        return false;
    }

    GameStatus getStatus() const { return status; }
    void displayBoard() const { board.display(); }
    Player getCurrentPlayer() const { return players[currentPlayerIndex]; }
};

// --- Step 8: TicTacToeSystem (Facade) ---

class TicTacToeSystem {
    Scoreboard scoreboard;
    RowWinStrategy rowStrategy;
    ColWinStrategy colStrategy;
    DiagonalWinStrategy diagStrategy;

public:
    void playDemoGame() {
        Player p1("Alice", Symbol::X);
        Player p2("Bob", Symbol::O);

        Board board(3);
        Game game(board, {p1, p2});

        game.addStrategy(&rowStrategy);
        game.addStrategy(&colStrategy);
        game.addStrategy(&diagStrategy);
        game.addObserver(&scoreboard);

        // Simulating moves for a win by X
        int moves[5][2] = {{0, 0}, {1, 0}, {0, 1}, {1, 1}, {0, 2}};

        for (int i = 0; i < 5; i++) {
            cout << game.getCurrentPlayer().getName() << " moves to (" << moves[i][0] << ", "
                 << moves[i][1] << ")" << endl;
            game.makeMove(moves[i][0], moves[i][1]);
            game.displayBoard();
            cout << endl;

            if (game.getStatus() != GameStatus::IN_PROGRESS)
                break;
        }

        if (game.getStatus() == GameStatus::WINNER_X)
            cout << "Winner: Alice (X)!" << endl;
        else if (game.getStatus() == GameStatus::WINNER_O)
            cout << "Winner: Bob (O)!" << endl;
        else if (game.getStatus() == GameStatus::DRAW)
            cout << "Game Draw!" << endl;
    }
};

// --- Step 9: Main (Demo) ---

int main() {
    TicTacToeSystem system;
    system.playDemoGame();
    return 0;
}
