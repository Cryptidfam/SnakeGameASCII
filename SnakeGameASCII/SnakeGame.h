#pragma once
using namespace std; // Use the standard namespace for convenience
#include <iostream>
#include <conio.h> // For _kbhit() and _getch()
#include <windows.h> // For Sleep()
#include <deque> 
#include <random>
#include "ConsoleUtils.h"

// The ConsoleUtils.h file is assumed to be in the same directory as this file
// To do: 1. Let the main loop handle termination. 2. Let the map array initialize once.

class SnakeGame
{
private:
    // Game data (previously globals)
    struct Point {
        int x, y;
    };
    // Game constants
    static const int width = 30;
    static const int height = 15;
    deque<Point> snake;
    int foodX, foodY;
    enum Direction {
        STOP,
        UP,
        DOWN,
        LEFT,
        RIGHT
    };
    Direction dir;
    int score;

public:
    // Constructor
    SnakeGame() : score(0), dir(STOP) // member initialization list 
    {setup();}

    // Game methods
    void setup() {
        Point start = { width / 2, height / 2 };
        snake.push_back(start);
        generateFood();
        dir = RIGHT;
    }

    void generateFood() {
        static random_device rd;
        static mt19937 gen(rd());
        static uniform_int_distribution<> disX(1, width - 2);
        static uniform_int_distribution<> disY(1, height - 2);
		do {
			{
				foodX = disX(gen);
				foodY = disY(gen);
			}
		} while (std::any_of(snake.begin(), snake.end(), [this](const Point& p) { return p.x == foodX && p.y == foodY; }));
    }

    // The input function both handles key presses and contains game logic (pausing), which violates separation of concerns.
    bool input() {
        if (_kbhit()) {
            char key = _getch();
            switch (key) {
            case 'w': if (dir != DOWN) dir = UP; break;
            case 's': if (dir != UP) dir = DOWN; break;
            case 'a': if (dir != RIGHT) dir = LEFT; break;
            case 'd': if (dir != LEFT) dir = RIGHT; break;
			case 'p': // Pause the game
                cout << "Game Paused. Press 'p' again to continue...\n";
                while (_getch() != 'p'); // Keep reading keys until 'p' is pressed
				break;
            case 'q': return false; // Kind of useless because any key closes the game.
            }
        }
        return true; // Return true to continue the game loop
    }

    // The draw() method creates a temporary 2D array in each call, maybe it would be better to update only changed cells?
    void draw() {
        char map[height][width]{};
        ConsoleUtils::clearScreen(); // Clear the console screen
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
				map[y][x] = ' '; // Draw empty space
            }
        }
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
					map[y][x] = '#'; // Draw the borders
                }
            }
        }
        map[foodY][foodX] = '*'; // Draw food
        for (auto& s : snake) {
            map[s.y][s.x] = 'O'; // Draw snake
        }
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                cout << map[y][x];	// Print the map
            }
            cout << "\n";
        }
        cout << "Score: " << score << "\n"; // Print the score
        cout.flush(); // Flush the output buffer, to laymans, this means to make sure everything is printed to the console
    }

	bool isCollision(const Point& head)
	{
        if (head.x <= 0 || head.x >= width - 1 || head.y <= 0 || head.y >= height - 1) {
            return true;
        }
        for (size_t i = 1; i < snake.size(); ++i) {
            if (snake[i].x == head.x && snake[i].y == head.y) return true;
        }
		return false;
	}
    void logic()
    {
        Point head = snake.front();
        switch (dir) {
        case UP:    head.y--; break;
        case DOWN:  head.y++; break;
        case LEFT:  head.x--; break;
        case RIGHT: head.x++; break;
        default: break;
        }

        if (isCollision(head)) {
            cout << "Game Over!\n";
            exit(0);
        }
        snake.push_front(head); // move head forward

        if (head.x == foodX && head.y == foodY) 
        {
            score++;
            generateFood(); // Generate new food
        }
        else {
            snake.pop_back(); // remove tail (last element from a vector) if not eating
        }
    }
    void run() {
        ConsoleUtils::hideCursor();
        while (input()) {
            draw();
            logic();
			Sleep(125);
        }
    }
};

