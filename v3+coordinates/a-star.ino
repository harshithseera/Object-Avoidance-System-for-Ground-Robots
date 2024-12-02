#include <math.h>

// Grid dimensions
#define GRID_ROWS 20
#define GRID_COLS 20
#define OBSTACLE -1
#define FREE 1
#define UNEXPLORED 0

// Movement directions
int dx[] = {-1, 1, 0, 0}; // Up, Down
int dy[] = {0, 0, -1, 1}; // Left, Right

// Robot's starting position
int startX = GRID_ROWS / 2, startY = GRID_COLS / 2;

// Define sensors pins (update as per your setup)
const int frontSensor = 34; 
const int leftSensor = 35;
const int rightSensor = 32;
const int backSensor = 33;

// Define movement functions (placeholders; replace with actual motor driver code)
void moveForward() { /* motor driver code */ }
void moveBackward() { /* motor driver code */ }
void moveLeft() { /* motor driver code */ }
void moveRight() { /* motor driver code */ }
void stopMovement() { /* motor driver code */ }

// Grid and cost structures
int grid[GRID_ROWS][GRID_COLS];
bool closedList[GRID_ROWS][GRID_COLS];

struct Node {
    int x, y, g, h, f;
    Node *parent;
};

// Simple priority queue implementation
#define MAX_NODES (GRID_ROWS * GRID_COLS)
Node *openList[MAX_NODES];
int openListSize = 0;

void pqPush(Node *node) {
    openList[openListSize++] = node;
    for (int i = openListSize - 1; i > 0 && openList[i]->f < openList[i - 1]->f; i--) {
        Node *temp = openList[i];
        openList[i] = openList[i - 1];
        openList[i - 1] = temp;
    }
}

Node *pqPop() {
    return openList[--openListSize];
}

bool isValid(int x, int y) {
    return x >= 0 && y >= 0 && x < GRID_ROWS && y < GRID_COLS && grid[x][y] != OBSTACLE;
}

int heuristic(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2); // Manhattan distance
}

void initializeGrid() {
    for (int i = 0; i < GRID_ROWS; i++) {
        for (int j = 0; j < GRID_COLS; j++) {
            grid[i][j] = UNEXPLORED;
            closedList[i][j] = false;
        }
    }
    grid[startX][startY] = FREE;
}

bool aStar(int startX, int startY, int targetX, int targetY) {
    openListSize = 0;

    Node *startNode = new Node{startX, startY, 0, heuristic(startX, startY, targetX, targetY), 0, nullptr};
    startNode->f = startNode->g + startNode->h;
    pqPush(startNode);

    while (openListSize > 0) {
        Node *current = pqPop();

        if (current->x == targetX && current->y == targetY) {
            // Reconstruct path
            Node *temp = current;
            while (temp != nullptr) {
                Serial.printf("(%d, %d) <- ", temp->x, temp->y);
                temp = temp->parent;
            }
            Serial.println("Start");
            return true;
        }

        closedList[current->x][current->y] = true;

        for (int i = 0; i < 4; i++) {
            int nx = current->x + dx[i];
            int ny = current->y + dy[i];

            if (isValid(nx, ny) && !closedList[nx][ny]) {
                int g = current->g + 1;
                int h = heuristic(nx, ny, targetX, targetY);
                int f = g + h;

                Node *neighbor = new Node{nx, ny, g, h, f, current};
                pqPush(neighbor);
            }
        }
    }

    Serial.println("Path not found.");
    return false;
}

void scanEnvironment(int robotX, int robotY) {
    int front = analogRead(frontSensor);
    int left = analogRead(leftSensor);
    int right = analogRead(rightSensor);
    int back = analogRead(backSensor);

    if (front < 20) grid[robotX - 1][robotY] = OBSTACLE;
    else grid[robotX - 1][robotY] = FREE;

    if (left < 20) grid[robotX][robotY - 1] = OBSTACLE;
    else grid[robotX][robotY - 1] = FREE;

    if (right < 20) grid[robotX][robotY + 1] = OBSTACLE;
    else grid[robotX][robotY + 1] = FREE;

    if (back < 20) grid[robotX + 1][robotY] = OBSTACLE;
    else grid[robotX + 1][robotY] = FREE;
}

void setup() {
    Serial.begin(115200);
    initializeGrid();

    int targetX = 10, targetY = 10; // Example target

    scanEnvironment(startX, startY);

    if (aStar(startX, startY, targetX, targetY)) {
        Serial.println("Exploration successful.");
    } else {
        Serial.println("Exploration failed.");
    }
}

void loop() {
    // Exploration logic or dynamic path planning updates here
}
