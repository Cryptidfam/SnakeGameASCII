#pragma once
using namespace std; // Use the standard namespace for convenience
#include <iostream>
#include <conio.h> // For _kbhit() and _getch()
#include <windows.h> // For Sleep()
#include <deque> 
#include <random>
#include "ConsoleUtils.h"
#include <string>

class SnakeGame
{
private:
    struct Point { // This is a struct to represent a point in 2D space, for snake and food.
        int x, y;
    };
    // Game constants
    static const int width = 30;
    static const int height = 15;
    char map[height][width]{};
    deque<Point> snake;
	Point food; // Food position
    enum Direction {
        UP,
        DOWN,
        LEFT,
        RIGHT
    };
    Direction dir;
    enum GameState {
        RUNNING,
        PAUSED,
        GAMEOVER,
        WON,
        QUIT
    };
    GameState state = RUNNING; // Game state
    string statusMessage;
    int score;

public:
    // Constructor
    SnakeGame() : score(0), dir(RIGHT) // member initialization list 
    {
        setup();
    }

    // Game methods
    void setup() {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
                    map[y][x] = '#'; // Draw the borders
                }
                else {
                    map[y][x] = ' '; // Fill the rest with spaces
                }
            }
        }
        snake.clear();
        Point start = { width / 2, height / 2 };
        snake.push_back(start);
        map[start.y][start.x] = 'O';

        dir = RIGHT;
        score = 0;

		// Calculate the position that would be immediately eaten to avoid a weird first-frame "auto-eat" at the start of the game. 
		// Simply using updateMap() before generating food, for some reason, did not work.
        Point nextPosition = { start.x + 1, start.y }; // Based on RIGHT direction
        generateFood();

        // If food ended up at the next position, regenerate it
        if (food.x == nextPosition.x && food.y == nextPosition.y) {
            // Clear that position on the map first
            map[food.y][food.x] = ' ';
            generateFood(); // Try again
        }
    }

    void generateFood() {
        vector<Point> freeSpaces;
		// Collect all free (playable) positions. This for loop being here bothers me. I should make a function for it.
        for (int y = 1; y < height - 1; ++y) {
            for (int x = 1; x < width - 1; ++x) {
                if (map[y][x] == ' ') {
                    freeSpaces.push_back({ x, y });
                }
            }
        }
        if (freeSpaces.empty()) {
            return;
        }
        static random_device rd;
        static mt19937 gen(rd());
		uniform_int_distribution<> dist(0, static_cast<int>(freeSpaces.size()) - 1); // Checks free space. 
        Point pos = freeSpaces[dist(gen)];
        food.x = pos.x;
        food.y = pos.y;
        map[food.y][food.x] = '*';
    }

	// Input handling
    void input() {
        if (_kbhit()) {
            char key = _getch();
            switch (key) {
            case 'w': if (dir != DOWN) dir = UP; break;
            case 's': if (dir != UP) dir = DOWN; break;
            case 'a': if (dir != RIGHT) dir = LEFT; break;
            case 'd': if (dir != LEFT) dir = RIGHT; break;
			case 'r': setup(); break;
            case 'p': state = PAUSED; break;
            case 'q': state = QUIT; break;
            }
        }
    }

    void draw() const {
		ConsoleUtils::clearScreen(); // Move the cursor to the top left corner.

        // Display map
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                cout << map[y][x];
            }
            cout << "\n";
        }

        // Score and controls
        string scoreText = "Score: " + to_string(score);
        cout << scoreText << string(20 - scoreText.length(), ' ') << "\n";
        cout << "Controls: WASD to move, P to pause, R to restart, Q to quit\n";

        // Display state-specific message if there is one
        if (!statusMessage.empty()) {
            cout << statusMessage << "\n";
        }

        cout.flush();
    }
    // Check for collision with wall or snake body
    bool isCollision(const Point& head) const {
        if (head.x <= 0 || head.x >= width - 1 || head.y <= 0 || head.y >= height - 1) {
            return true;
        }
        for (size_t i = 1; i < snake.size(); ++i) {
            if (head.x == snake[i].x && head.y == snake[i].y) {
                return true;
            }

        }
        return false; // No collision
    }

    void updateMap() {
        for (int y = 1; y < height - 1; y++) { // Clear the map (except borders)
            for (int x = 1; x < width - 1; x++) {
                map[y][x] = ' ';
            }
        }
		map[food.y][food.x] = '*'; // Place the food pieces.
        for (size_t i = 1; i < snake.size(); i++) {
            map[snake[i].y][snake[i].x] = 'o'; // Update the map with the snake's position.
        }
        if (!snake.empty()) {
            map[snake.front().y][snake.front().x] = 'O'; // Place snake head
        }
    }
    void logic() {
        Point head = snake.front();
        // Calculate new head position
        switch (dir) {
        case UP:    head.y--; break;
        case DOWN:  head.y++; break;
        case LEFT:  head.x--; break;
        case RIGHT: head.x++; break;
        default: break;
        }
        if (isCollision(head)) {
            state = GAMEOVER;
            return;
        }
        
		bool ate = (head.x == food.x && head.y == food.y); // Detect if snake ate food
        snake.push_front(head); // Move snake
        if (ate) {
			score++; // Increase score
            generateFood();
        }
        else {
			snake.pop_back(); // Not eating food, remove tail
        }
		updateMap(); // Update the map with the new snake position
        if (snake.size() >= static_cast<size_t>((width - 2) * (height - 2)))
        {
            state = WON;
            return;
        }
    }
    
    // Needs a better name.
    void showMessageAndWait(const string & message) {
        statusMessage = message;
        draw();
        while (true) {
            char key = _getch();
            if (key == 'q') { state = QUIT; break; }
			if (key == 'r') { setup(); state = RUNNING; statusMessage = string(50, ' '); break; } // At least messages get 'cleared' now.
        }
    }
    void run() {
        ConsoleUtils::hideCursor();
        state = RUNNING;
        while (state != QUIT) {
            input();
            if (state == GAMEOVER) showMessageAndWait("Game over. Final score: " + to_string(score));
            if (state == WON) showMessageAndWait("You win! Final score: " + to_string(score));
            if (state == PAUSED) {
                statusMessage = "Game paused. Press P to continue or Q to quit.";
                draw();
                while (true) {
                    char key = _getch();
                    if (key == 'p') { state = RUNNING; statusMessage = string(50, ' '); break; }
                    if (key == 'q') { state = QUIT; break; }
                }
            }
            if (state == RUNNING) {
                logic();
                draw();
                Sleep(225); // Control the speed of the game
            }
        }
    }

};