% MATLAB Visualization for x-y Scatter Plot

% Retrieve data from ThingSpeak channel
readChannelID = 2506706; % Replace with your channel ID
xFieldID = 1; % Field 1 for x-coordinate
yFieldID = 2; % Field 2 for y-coordinate
numPoints = 8000; % Number of data points to retrieve

% Read the data from ThingSpeak
[xData, timeStampsX] = thingSpeakRead(readChannelID, 'Fields', xFieldID, 'NumPoints', numPoints);
[yData, timeStampsY] = thingSpeakRead(readChannelID, 'Fields', yFieldID, 'NumPoints', numPoints);

obstacleXFieldID = 3;
obstacleYFieldID = 4;

[xData2, timeStampsX2] = thingSpeakRead(readChannelID, 'Fields', obstacleXFieldID, 'NumPoints', numPoints);
[yData2, timeStampsY2] = thingSpeakRead(readChannelID, 'Fields', obstacleYFieldID, 'NumPoints', numPoints);

% Create the scatter plot for the first dataset (in blue)
scatter(xData, yData, 36, 'b', 'filled'); % 'b' for blue color
hold on; % Hold the current plot to overlay the second dataset

% Create the scatter plot for the second dataset (in red)
scatter(xData2, yData2, 36, 'r', 'filled'); % 'r' for red color

% Add title and labels
title('Robot and Obstacle Position Map');
xlabel('X Position');
ylabel('Y Position');
grid on;

% Add legend to differentiate between the two data sets
legend('Free Explored Paths', 'Obstacles');
hold off; % Release the hold on the current plot