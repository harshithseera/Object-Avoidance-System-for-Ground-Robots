% MATLAB Visualization for x-y Scatter Plot

% Retrieve data from ThingSpeak channel
readChannelID = 2506706; % Replace with your channel ID
xFieldID = 1; % Field 1 for x-coordinate
yFieldID = 2; % Field 2 for y-coordinate
numPoints = 8000; % Number of data points to retrieve

% Read the data from ThingSpeak
[xData, timeStampsX] = thingSpeakRead(readChannelID, 'Fields', xFieldID, 'NumPoints', numPoints);
[yData, timeStampsY] = thingSpeakRead(readChannelID, 'Fields', yFieldID, 'NumPoints', numPoints);

% Create the scatter plot
scatter(xData, yData, 'filled');
title('Robot x-y Position');
xlabel('X Position');
ylabel('Y Position');
grid on;