// SnakeGameASCII.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
using namespace std; // Use the standard namespace for convenience
#include <iostream>
#include <conio.h> // For _kbhit() and _getch()
#include <windows.h> // For Sleep()
#include <deque> 
#include <ctime> // For srand() and rand()

struct Point {
	int x, y;
};
// Game constants
const int width = 30;
const int height = 15;

// Game variables
deque<Point> snake;
int foodX, foodY;
char map[height][width];
enum Direction {
	STOP,
	UP,
	DOWN,
	LEFT,
	RIGHT
};
Direction dir = STOP;

void setup();
void draw();
bool input();
void logic();
void static hideCursor();

int main()
{
	setup();
	hideCursor(); // Hide the console cursor
	while (input()) {
		draw();
		logic(); // Update the game logic
		Sleep(125); // Control game speed
	}
	return 0;
}

void setup() {
	srand(time(0)); // Seed the random number generator 'Warning: 'argument': conversion from 'time_t' to 'unsigned int', possible loss of data'
	Point start = { width / 2, height / 2 };
	snake.push_back(start);
	foodX = rand() % width;
	foodY = rand() % height;
	dir = RIGHT; // Set the initial direction of the snake
}

bool input() {
	if (_kbhit()) {
		char key = _getch();
		switch (key) {
		case 'w': if (dir != DOWN) dir = UP; break;
		case 's': if (dir != UP) dir = DOWN; break;
		case 'a': if (dir != RIGHT) dir = LEFT; break;
		case 'd': if (dir != LEFT) dir = RIGHT; break;
		case 'q': return false;
		}
	}
	return true; // Return true to continue the game loop
}

void hideCursor() {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(hOut, &cursorInfo);
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(hOut, &cursorInfo);
}


void static clearScreen() {
	COORD cursorPosition{};
	cursorPosition.X = 0;
	cursorPosition.Y = 0;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
}

// This could be improved by turning it into seperate functions.
void draw() {
	clearScreen(); // Clear the console screen
	// Clear the map
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			map[y][x] = ' '; // Default to empty
		}
	}
	// Draw walls
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
				map[y][x] = '#';
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
	cout.flush(); // Flush the output buffer
}

// Handle movement and collisions
void logic() {
	Point head = snake.front();
	switch (dir) {
	case UP:    head.y--; break;
	case DOWN:  head.y++; break;
	case LEFT:  head.x--; break;
	case RIGHT: head.x++; break;
	default: break;
	}

	// Check wall or self collision
	if (head.x <= 0 || head.x >= width - 1 || head.y <= 0 || head.y >= height - 1) {
		cout << "Game Over (wall)!\n"; exit(0);
	}


	for (auto& segment : snake) {
		if (segment.x == head.x && segment.y == head.y) {
			cout << "Game Over (self)!\n"; exit(0);
		}
	}

	snake.push_front(head); // move head forward

	if (head.x == foodX && head.y == foodY) {
		foodX = rand() % (width - 2) + 1; // To do: Use srand() to make this more random
		foodY = rand() % (height - 2) + 1;
	}
	else {
		snake.pop_back(); // remove tail if not eating
	}
}
