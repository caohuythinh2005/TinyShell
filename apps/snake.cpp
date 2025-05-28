#include <iostream>
#include <vector>
#include <windows.h>
#include <conio.h>
#include <ctime>    // dùng cho time()
#include <cstdlib>  // dùng cho rand(), srand()
using namespace std;

// MACRO
#define WIDTH 40
#define HEIGHT 20
#define BODY '*'
#define APPLE 'O'

// Enum
enum class Direction
{
    up,
    right,
    down,
    left
};

// Struct
struct Point
{
    int x;
    int y;
};

// Global Variables
vector<Point> snake;
Direction direction = Direction::right;
Point apple;
int score = 0;
int speed = 300;
Point prevTail;

// Function Prototypes
void drawSnakePart(Point);
void drawSnake();
void gotoxy(int, int);
void ShowConsoleCursor(bool);
void move();
void drawBox();
bool isHitWall();
bool isBiteItself();
void drawHeadnTail();
void genApple();
bool isAteApple();
void growing();
void displayScore();
void showEndMenu();
void startGame();
void resetSnake();
void showStartMenu();

// Main function
int main()
{
    srand(static_cast<unsigned int>(time(0))); // chỉ gọi 1 lần
    showStartMenu();
    return 0;
}

// Function Definitions

void drawBox()
{
    for (int i = 0; i <= WIDTH; i++)
        cout << '=';

    gotoxy(0, HEIGHT);
    for (int i = 0; i <= WIDTH; i++)
        cout << '=';

    for (int i = 1; i < HEIGHT; i++)
    {
        gotoxy(0, i);
        cout << '|';
        gotoxy(WIDTH, i);
        cout << '|';
    }
}

bool isHitWall()
{
    return snake[0].x <= 0 || snake[0].y <= 0 || snake[0].x >= WIDTH || snake[0].y >= HEIGHT;
}

void genApple()
{
    int x = rand() % (WIDTH - 2) + 1;
    int y = rand() % (HEIGHT - 2) + 1;
    apple = { x, y };
    gotoxy(x, y);
    cout << APPLE;
}

bool isAteApple()
{
    return snake[0].x == apple.x && snake[0].y == apple.y;
}

void displayScore()
{
    gotoxy(WIDTH + 5, 2);
    cout << "Your score: " << score;
}

void showEndMenu()
{
    system("cls");
    cout << "End game!\n";
    cout << "Your score: " << score << endl;
    cout << "Do you want to play again ([y]/[n]): ";
    char option;
    cin >> option;
    option = tolower(option);
    if (option == 'y')
    {
        resetSnake();
        startGame();
    }
    else
        exit(0);
}

void startGame()
{
    system("cls");
    ShowConsoleCursor(false);

    drawBox();
    drawSnake();
    genApple();
    displayScore();

    while (true)
    {
        if (_kbhit())
        {
            char ch = tolower(_getch());
            if (ch == 'a' && direction != Direction::right) direction = Direction::left;
            else if (ch == 'w' && direction != Direction::down) direction = Direction::up;
            else if (ch == 's' && direction != Direction::up) direction = Direction::down;
            else if (ch == 'd' && direction != Direction::left) direction = Direction::right;
            else if (ch == 'q')
            {
                showEndMenu();
                break;
            }
        }

        move();
        drawHeadnTail();

        if (isAteApple())
        {
            score++;
            displayScore();
            growing();
            genApple();
        }

        if (isBiteItself() || isHitWall())
        {
            ShowConsoleCursor(true);
            showEndMenu();
            break;
        }

        Sleep(speed);
    }
}

void resetSnake()
{
    score = 0;
    direction = Direction::right;
    snake = {
        { WIDTH / 2 + 2, HEIGHT / 2 },
        { WIDTH / 2 + 1, HEIGHT / 2 },
        { WIDTH / 2, HEIGHT / 2 },
        { WIDTH / 2 - 1, HEIGHT / 2 },
        { WIDTH / 2 - 2, HEIGHT / 2 }
    };
}

void showStartMenu()
{
    system("cls");
    cout << "Welcome to Snake Game!\n";
    cout << "1. Start\n2. Quit\nYour choice: ";
    int option;
    cin >> option;

    if (option == 1)
    {
        system("cls");
        cout << "Choose your level (1 - 5): ";
        int t;
        cin >> t;
        t = max(1, min(t, 5)); // giới hạn level
        speed = 600 - t * 100;
        system("cls");
        cout << "Tip: Press 'q' to quit at any time.";
        gotoxy(0, 3);
        cout << "Ready!";
        Sleep(1000);
        for (int i = 3; i > 0; i--)
        {
            gotoxy(0, 3);
            cout << i << "   ";
            Sleep(1000);
        }
        gotoxy(0, 3);
        cout << "GO!";
        Sleep(1000);
        resetSnake();
        startGame();
    }
    else
        exit(0);
}

void drawSnakePart(Point p)
{
    gotoxy(p.x, p.y);
    cout << BODY;
}

void drawSnake()
{
    for (Point p : snake)
        drawSnakePart(p);
}

void move()
{
    prevTail = snake.back();
    for (size_t i = snake.size() - 1; i > 0; i--)
        snake[i] = snake[i - 1];

    switch (direction)
    {
    case Direction::up:    snake[0].y--; break;
    case Direction::down:  snake[0].y++; break;
    case Direction::left:  snake[0].x--; break;
    case Direction::right: snake[0].x++; break;
    }
}

void drawHeadnTail()
{
    gotoxy(snake[0].x, snake[0].y);
    cout << BODY;
    gotoxy(prevTail.x, prevTail.y);
    cout << ' ';
}

bool isBiteItself()
{
    for (size_t i = 1; i < snake.size(); i++)
        if (snake[i].x == snake[0].x && snake[i].y == snake[0].y)
            return true;
    return false;
}

void growing()
{
    snake.push_back(prevTail);
}

void gotoxy(int x, int y)
{
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void ShowConsoleCursor(bool showFlag)
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag;
    SetConsoleCursorInfo(out, &cursorInfo);
}
