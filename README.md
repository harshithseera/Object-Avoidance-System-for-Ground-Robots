# Documentation
To run the code, open the file using Arduino IDE and upload the `.ino` file to the ESP32 module. Running should begin automatically. 

## Folder and File Descriptions
a. Data: Contains `.csv` files for sample plotting on ThingSpeak to show MATLAB and graph functionality
b. MATLAB: Contains the matlab code for plotting the scatterplots on ThingSpeak
c. v1: Contains our first, very basic implementation of front and back sensors and motor movement without N20 encoders
d. v2: Contains our first, very basic implementation of front and back movement with N20 encoders
e. v3: Contains our first code for two motors where the robot moves front and back, based on ultrasonic sesnsor readings
f. v3+coordinates: 
  - a-star.ino: Contains rudimentary implementation of a-star algorithm
  - coordinates.ino: Contains rudimentary implementation of coordinate system
  - coordinates+thingspeak.ino: rudimentary implementation of coordinate system and publishing data to ThingSpeak
  - final_v3.ino: v3/v3.ino but with turning added
g. v4fixedDirectionMovement:
  - dump.ino: Code to perform BFS and discover best path for travel
  - fixedDistanceMovement.ino: Assume fixed distance traversed when moving forwards/backwards, update coordinates, implement a local grid system, and publish to ThingSpeak accordingly
h. v5fixedAndPathFinding:
  - final2.ino: Merged dump.ino functions with fixedDistanceMovement.ino
i. v6OnlyForward:
  - Motor was not working and the encoder was not working so we could only move forward and right. `forward.ino`  was a basic implementation proof of concept version of code to operate using only front and right turns
  - withCoordinatesAndThingspeak/file.ino: added coordinates and thingspeak to this
j. eval.ino: File created during eval to show temporary robot movement and ThingSpeak/coordinate values
k. index.html and style.css were to create our dashboard website
