# Low Level Design (LLD) — C++

A collection of Low Level Design problems implemented in C++, following OOP principles and common design patterns.

## Design Patterns Covered

- **Strategy Pattern** — Pluggable behavior (e.g., win detection logic)
- **Observer Pattern** — Event-driven updates (e.g., scoreboard)
- **Singleton Pattern** — Single shared entry point

## Projects

| Project | Description | Patterns Used |
|---|---|---|
| [Tic Tac Toe](./tictactoe.cpp) | 2-player terminal game on a 3×3 grid | Strategy, Observer, Singleton |

## How to Compile & Run

```bash
clang++ tictactoe.cpp -std=c++17 -o tictactoe && ./tictactoe
```
