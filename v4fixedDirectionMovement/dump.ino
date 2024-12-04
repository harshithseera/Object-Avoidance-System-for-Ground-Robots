/* Code for getting information about surrounding cells from a given cell */
void Gather() {
    // Measure distances in all directions
    int distances[4];
    distances[0] = getDistance(TRIG_FRONT, ECHO_FRONT); // Front
    distances[1] = getDistance(TRIG_BACK, ECHO_BACK);   // Back
    distances[2] = getDistance(TRIG_LEFT, ECHO_LEFT);   // Left
    distances[3] = getDistance(TRIG_RIGHT, ECHO_RIGHT); // Right

    // Directions: 0 - Front, 1 - Back, 2 - Left, 3 - Right
    for (int dir = 0; dir < 4; dir++) {
        int distance = distances[dir];
        int maxCells = distance / CELL_SIZE;
        if (maxCells > MAX_CELLS) maxCells = MAX_CELLS;

        int dx = 0, dy = 0;
        int sensorOrientation = (orientation + dir * 90) % 360;

        switch (sensorOrientation) {
            case RIGHT: dx = 1; dy = 0; break;
            case UP:    dx = 0; dy = 1; break;
            case LEFT:  dx = -1; dy = 0; break;
            case DOWN:  dx = 0; dy = -1; break;
        }

        int gridX = mapToGrid(x_position);
        int gridY = mapToGrid(y_position);

        for (int i = 1; i <= maxCells; i++) {
            int nx = gridX + dx * i;
            int ny = gridY + dy * i;

            // Check grid bounds
            if (nx < 0 || nx >= GRID_SIZE || ny < 0 || ny >= GRID_SIZE) break;

            // If an obstacle is detected
            if (i * CELL_SIZE >= distance) {
                grid[nx][ny] = OBSTACLE;
                break;
            } else {
                // Mark as free path
                if (grid[nx][ny] == UNTRAVELLED) {
                    grid[nx][ny] = FREEPATH;
                }
            }
        }
    }

    // Mark current cell as visited
    int currentGridX = mapToGrid(x_position);
    int currentGridY = mapToGrid(y_position);
    grid[currentGridX][currentGridY] = FREEPATH;
}

/*Code to find closest path to unvisited cells given information about surrounding cells atleast via BFS*/
vector<pair<int, int>> FindPoi() {
    int startX = mapToGrid(x_position);
    int startY = mapToGrid(y_position);

    bool visited[GRID_SIZE][GRID_SIZE] = {false};

    struct Node {
        int x, y;
        vector<pair<int, int>> path;
    };

    queue<Node> q;
    q.push({startX, startY, {}});
    visited[startX][startY] = true;

    while (!q.empty()) {
        Node current = q.front();
        q.pop();

        // If the cell is unvisited, return the path to it
        if (grid[current.x][current.y] == UNTRAVELLED) {
            return current.path;
        }

        // Explore neighboring cells
        const vector<pair<int, int>> directions = {{-1,0},{1,0},{0,-1},{0,1}};
        for (const auto& dir : directions) {
            int nx = current.x + dir.first;
            int ny = current.y + dir.second;

            // Check bounds and if cell is traversable
            if (nx >= 0 && nx < GRID_SIZE && ny >= 0 && ny < GRID_SIZE &&
                !visited[nx][ny] && grid[nx][ny] != OBSTACLE) {
                visited[nx][ny] = true;
                Node nextNode = {nx, ny, current.path};
                nextNode.path.push_back({nx, ny});
                q.push(nextNode);
            }
        }
    }

    // No unvisited cell found
    return {};
}

/* Code to actually follow a given path */
void moveOneCell() {
    forward();
    int targetEncoderValue = leftMotor.EncoderValue + ENCODER_COUNTS_PER_CELL;

    while (leftMotor.EncoderValue < targetEncoderValue) {
        int distanceFront = getDistance(TRIG_FRONT, ECHO_FRONT);
        if (distanceFront < DETECT) {
            stopMotors();
            Gather(); // Update grid with the obstacle
            return;   // Exit the function to replan
        }
        delay(10);
    }
    stopMotors();
    updatePosition(1);
    Gather();
}

void FollowPath(const vector<pair<int, int>>& path) {
    for (const auto& point : path) {
        int targetX = point.first;
        int targetY = point.second;

        int deltaX = targetX - mapToGrid(x_position);
        int deltaY = targetY - mapToGrid(y_position);

        int requiredOrientation;

        if (deltaX == 1 && deltaY == 0) {
            requiredOrientation = RIGHT;
        } else if (deltaX == -1 && deltaY == 0) {
            requiredOrientation = LEFT;
        } else if (deltaX == 0 && deltaY == 1) {
            requiredOrientation = UP;
        } else if (deltaX == 0 && deltaY == -1) {
            requiredOrientation = DOWN;
        } else {
            Serial.println("Invalid path movement");
            continue;
        }

        // Rotate to required orientation
        while (orientation != requiredOrientation) {
            int diff = (requiredOrientation - orientation + 360) % 360;
            if (diff == 90) {
                turnLeft();
            } else if (diff == 270) {
                turnRight();
            } else if (diff == 180) {
                turnLeft();
                turnLeft();
            }
        }

        // Move forward one cell
        moveOneCell();
    }
}

/* Template loop, can be restructured to do more this just does the bare minimum of finding info, going to a point of interest, finding more info and so on */
void loop() {
    // Reconnect to WiFi and MQTT if disconnected
    if (WiFi.status() != WL_CONNECTED) {
        connectWifi();
    }

    if (!mqttClient.connected()) {
        mqttConnect(); 
        mqttSubscribe(channelID);
    }

    mqttClient.loop(); 

    // Gather sensor data and update the grid
    Gather();

    // Find path to nearest unvisited cell
    vector<pair<int, int>> path = FindPoi();

    if (path.empty()) {
        // Exploration complete
        Serial.println("Exploration complete");
        stopMotors();
        while (true); // Stop the robot
    } else {
        // Follow the path
        FollowPath(path);
    }

    delay(100); // Small delay to prevent overwhelming the loop
}
