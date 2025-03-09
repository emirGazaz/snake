#include <iostream>
#include <chrono>
#include <thread>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <random>

#define DIMY 50
#define DIMX 50
#define MAX 30

int posX = 10, posY = 25, fPosX, fPosY;
int sPosX[MAX], sPosY[MAX];
int level = 1;
bool active = 1;
char direction = 'd'; // Default movement direction

void spawnFood(char arr[][DIMY]){
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> distX(0, DIMX - 1);
    static std::uniform_int_distribution<int> distY(0, DIMY - 1);

    do {
        fPosX = distX(gen);
        fPosY = distY(gen);
    } while (arr[fPosX][fPosY] == 'S'); // Ensure food doesn't spawn inside the snake

    arr[fPosX][fPosY] = 'F';
}

void init(char arr[][DIMY]) {
    for (int i = 0; i < DIMX; i++)
        for (int k = 0; k < DIMY; k++)
            arr[i][k] = '.';

    sPosX[0] = posX;
    sPosY[0] = posY;
}

char getKey() {
    struct termios oldt, newt;
    char ch = 0;
    struct timeval tv = {0L, 0L};
    fd_set fds;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0)
        read(STDIN_FILENO, &ch, 1);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void checkCollision(char arr[][DIMY]){
    
    for(int i=1;i<level;i++)
        if(sPosX[0]==sPosX[i] && sPosY[i]==sPosY[0])
        {
            active=0;
            return;
        }    

}

void move(char arr[][DIMY]) {
    // Clear last position of the tail
    arr[sPosX[level - 1]][sPosY[level - 1]] = '.';

    // Shift snake body forward
    for (int i = level - 1; i > 0; i--) {
        sPosX[i] = sPosX[i - 1];
        sPosY[i] = sPosY[i - 1];
    }

    // Move head based on direction
    if (direction == 'w') posX--;
    else if (direction == 's') posX++;
    else if (direction == 'a') posY--;
    else if (direction == 'd') posY++;

    // Prevent out-of-bounds movement (wrap around)
    if (posX < 0) posX = DIMX - 1;
    if (posX >= DIMX) posX = 0;
    if (posY < 0) posY = DIMY - 1;
    if (posY >= DIMY) posY = 0;

    // Update new head position
    sPosX[0] = posX;
    sPosY[0] = posY;

    // Check if food is eaten
    if (posX == fPosX && posY == fPosY) {
        if (level < MAX) level++; // Grow only up to MAX size
        spawnFood(arr);
    }

    // Update board with the snake
    for (int i = 0; i < level; i++)
        arr[sPosX[i]][sPosY[i]] = 'S';

    checkCollision(arr);
}

void render(char arr[][DIMY]) {
    while (active) {
        system("clear");

        // Print board
        for (int i = 0; i < DIMX; i++) {
            for (int k = 0; k < DIMY; k++)
                std::cout << arr[i][k] << " ";
            std::cout << std::endl;
        }

        // Read input without waiting
        char key = getKey();
        switch(key){
            case 'w': if (direction!='s') direction=key; break;
            case 's': if (direction!='w') direction=key; break;
            case 'a': if (direction!='d') direction=key; break;
            case 'd': if (direction!='a') direction=key; break;
        }

        move(arr);
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Smooth movement
    }

    std::cout << "Game Over" << std::endl;
}



int main() {
    char dims[DIMX][DIMY];
    init(dims);
    spawnFood(dims);
    render(dims);
    return 0;
}
