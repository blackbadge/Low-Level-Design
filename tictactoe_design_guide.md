# LLD Learning Guide: Design a Tic Tac Toe Game in C++

A step-by-step walkthrough of how to approach a Low Level Design problem from scratch — from reading requirements to writing clean, extensible code.

---

## Step 1: Clarify Requirements

Before writing a single line of code, ask: **what does this system actually need to do?**

Divide into two categories:

### Functional Requirements (What the system does)
- Played on a **3×3 grid**
- Two players take **alternate turns**, identified by `X` and `O`
- System **detects wins** (row, column, diagonal)
- System **detects a draw** (board full, no winner)
- System **rejects invalid moves** (occupied cell, out-of-bounds)
- System maintains a **scoreboard** across multiple games
- Moves can be **hardcoded in a demo** (no real-time input needed for LLD interviews)

### Non-Functional Requirements (How the system should be built)
- Follow **OOP principles** — clear responsibilities, separation of concerns
- **Modular and extensible** — should be easy to add AI, larger boards, move history
- Game logic should be **testable and maintainable**

> **Interview tip**: Always ask these questions first. It shows you think like a system designer, not just a programmer.

---

## Step 2: Identify Core Entities

Think of entities as the **nouns** in your requirements. What "things" exist in this system?

| Entity | Type | Purpose |
|---|---|---|
| `Symbol` | Enum | Fixed values: `X`, `O`, `EMPTY` |
| `GameStatus` | Enum | States: `IN_PROGRESS`, `WINNER_X`, `WINNER_O`, `DRAW` |
| `Cell` | Data Class | Holds one grid position's symbol |
| `Player` | Data Class | Holds player name and symbol |
| `Board` | Core Class | Manages the 3×3 grid |
| `Game` | Core Class | Orchestrates turn-by-turn gameplay |
| `Scoreboard` | Core Class | Tracks wins/draws across multiple games |
| `TicTacToeSystem` | Facade/Singleton | Entry point that ties everything together |

### Why Enums First?
Enums define the **vocabulary** of your system. Defining them first ensures all other classes speak the same language and prevents invalid states at compile time.

---

## Step 3: Define Class Attributes and Behaviour

For each entity, define **what it knows** (attributes) and **what it can do** (methods).

### `Cell`
```cpp
class Cell {
    Symbol symbol; // starts as EMPTY
public:
    bool isEmpty();
    void setSymbol(Symbol s);
    Symbol getSymbol();
};
```
- **Mutable**: starts `EMPTY`, updates when a move is made.
- `isEmpty()` is cleaner than checking `getSymbol() == Symbol::EMPTY` everywhere.

### `Player`
```cpp
class Player {
    string name;
    Symbol symbol;
public:
    Player(string name, Symbol symbol);
    string getName();
    Symbol getSymbol();
};
```
- **Immutable**: once created, name and symbol never change. This prevents accidental state corruption.

### `Board`
```cpp
class Board {
    vector<vector<Cell>> grid;
public:
    bool makeMove(int row, int col, Symbol symbol);
    bool isFull();
    void display();
    Cell getCell(int row, int col);
};
```
- Owns the grid — **composition** (Board HAS-A grid of Cells).

### `Game`
```cpp
class Game {
    Board board;
    vector<Player> players;
    int currentPlayerIndex;
    GameStatus status;
    vector<WinningStrategy*> strategies;
    vector<GameObserver*> observers;
public:
    void makeMove(int row, int col);
    GameStatus getStatus();
};
```
- Orchestrates the game loop, validates moves, checks win conditions, notifies observers.

---

## Step 4: Design Relationships Between Classes

Understanding relationships between classes is crucial for LLD.

### Composition (Strong Ownership — "HAS-A")
- `Game` **owns** a `Board` → if Game is destroyed, Board is too
- `Board` **owns** a grid of `Cell`s

### Association (Weak Reference — "USES-A")
- `Game` **uses** `Player` objects (Players exist independently of a single Game)
- `Game` **uses** `WinningStrategy` implementations

### Implementation (Interface Contract)
- `WinningStrategy` is an **interface** — `RowWinStrategy`, `ColWinStrategy`, `DiagonalWinStrategy` implement it
- `GameObserver` is an **interface** — `Scoreboard` implements it

---

## Step 5: Identify and Apply Design Patterns

This is where LLD separates from basic OOP. Design patterns are **proven solutions to recurring problems**.

### 🔁 Strategy Pattern — Win Detection

**Problem**: There are 3 types of win checks (row, col, diagonal). If we add more (e.g., four-corners), we'd keep modifying the `Game` class — violating the **Open/Closed Principle**.

**Solution**: Extract each win-check into its own class implementing a common `WinningStrategy` interface.

```cpp
class WinningStrategy {
public:
    virtual bool checkWin(Board& board, int row, int col, Symbol symbol) = 0;
};

class RowWinStrategy : public WinningStrategy { ... };
class ColWinStrategy : public WinningStrategy { ... };
class DiagonalWinStrategy : public WinningStrategy { ... };
```

`Game` holds a `vector<WinningStrategy*>` and iterates through them. Adding a new rule = adding a new class. **No changes to Game.**

---

### 👁️ Observer Pattern — Scoreboard Updates

**Problem**: When a game ends, the `Scoreboard` needs to update. But `Game` shouldn't be tightly coupled to `Scoreboard`. What if we also want a `Logger` or an `Analytics` service to react?

**Solution**: `Game` notifies all registered **observers** when a game ends. Each observer handles the event independently.

```cpp
class GameObserver {
public:
    virtual void onGameEnd(Game& game) = 0;
};

class Scoreboard : public GameObserver {
    void onGameEnd(Game& game) override { /* update scores */ }
};
```

`Game` maintains a `vector<GameObserver*>` and calls `onGameEnd()` for all. Adding a logger = add a new observer. **No changes to Game.**

---

### 🔒 Singleton Pattern — TicTacToeSystem

**Problem**: The entry point (`TicTacToeSystem`) should be globally accessible but only one instance should exist.

**Solution**: Singleton pattern. The constructor is private; a static method returns the single instance.

```cpp
class TicTacToeSystem {
    static TicTacToeSystem* instance;
    TicTacToeSystem() {}  // private
public:
    static TicTacToeSystem* getInstance();
    void startGame(Player p1, Player p2);
};
```

> **When to use Singleton**: Only when you genuinely need one shared instance (e.g., a global registry, config manager). Don't overuse it.

---

## Step 6: Draw a Class Diagram (Mental or on Paper)

Before coding, sketch:
- Boxes for each class with their attributes and methods
- Arrows showing composition (`♦──`), association (`──>`), and inheritance / implementation (`◁──`)

```
TicTacToeSystem (Singleton)
    └── creates Game
            ├── HAS-A Board
            │       └── HAS-A Cell[][]
            ├── HAS-A Player[]
            ├── USES WinningStrategy[] (Strategy Pattern)
            └── NOTIFIES GameObserver[] (Observer Pattern)
                            └── Scoreboard implements GameObserver
```

---

## Step 7: Write the Code — Bottom Up

Always code in **dependency order** — classes that have no dependencies first.

| Order | Class | Reason |
|---|---|---|
| 1 | Enums (`Symbol`, `GameStatus`) | No dependencies |
| 2 | `Cell`, `Player` | Depend only on enums |
| 3 | `Board` | Depends on `Cell` |
| 4 | Interfaces (`WinningStrategy`, `GameObserver`) | Contracts only |
| 5 | Strategy implementations | Depend on `Board`, interface |
| 6 | `Scoreboard` | Implements `GameObserver` |
| 7 | `Game` | Depends on all above |
| 8 | `TicTacToeSystem` | Depends on `Game` |
| 9 | `main()` / Demo | Wires everything together |

---

## Step 8: Extensibility — Think About "What Ifs"

A good LLD is **easy to extend without modifying existing code**.

| Extension | What Changes |
|---|---|
| Variable board size (4×4, 5×5) | `Board` constructor takes size parameter |
| New win condition (four corners) | Add a new `WinningStrategy` class |
| AI opponent | Add a new `Player` subclass `AIPlayer` |
| Move history / Undo | Add a `MoveHistory` class with a stack |
| Multiple observers (logger, analytics) | Add new `GameObserver` implementations |

---

## Summary: The LLD Process

```
1. Clarify Requirements  →  Functional + Non-functional
2. Identify Entities     →  Enums, Data Classes, Core Classes
3. Define Classes        →  Attributes + Methods per class
4. Design Relationships  →  Composition, Association, Interfaces
5. Apply Design Patterns →  Strategy, Observer, Singleton
6. Draw Class Diagram    →  Visualize structure before coding
7. Write Code (Bottom-Up)→  Dependencies first
8. Think Extensibility   →  Open/Closed Principle
```
