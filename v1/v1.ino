#include <AFMotor.h>

#define SPEED 255

#define TRIG_FRONT 13
#define ECHO_BACK 9
#define TRIG_BACK 2
#define ECHO_FRONT 10

#define MAX_DISTANCE 200
#define DETECT 10

// Instantiate four motor objects, one for each motor
AF_DCMotor motor1(1);
AF_DCMotor motor2(2);
AF_DCMotor motor3(3);
AF_DCMotor motor4(4);

int dir = 1;

void setup() {
    Serial.begin(9600);
    pinMode(TRIG_FRONT, OUTPUT);
    pinMode(ECHO_FRONT, INPUT);
    pinMode(TRIG_BACK, OUTPUT);
    pinMode(ECHO_BACK, INPUT);

    stopMotors();
    forward();
}

void loop() {
    int distanceFront = getDistance(TRIG_FRONT, ECHO_FRONT);
    int distanceBack = getDistance(TRIG_BACK, ECHO_BACK);

    Serial.print("Front: ");
    Serial.print(distanceFront);
    Serial.print(" Back: ");
    Serial.println(distanceBack);
    Serial.println();
    
    if(dir == 1 && distanceFront < DETECT) {
        stopMotors();
        backward();
        delay(1000);
        dir = -1;
    }
    if(dir == -1 && distanceBack < DETECT) {
        stopMotors();
        forward();
        delay(1000);
        dir = 1;
    }
}

int getDistance(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    int distance = duration * 0.034 / 2;
    return (distance == 0) ? MAX_DISTANCE : distance;
}

void forward() {
    motor1.run(FORWARD);
    motor2.run(FORWARD);
    motor3.run(FORWARD);
    motor4.run(FORWARD);

    motor1.setSpeed(SPEED);
    motor2.setSpeed(SPEED);
    motor3.setSpeed(SPEED);
    motor4.setSpeed(SPEED);

    Serial.println("Forward");
}

void backward() {
    motor1.run(BACKWARD);
    motor2.run(BACKWARD);
    motor3.run(BACKWARD);
    motor4.run(BACKWARD);

    motor1.setSpeed(SPEED);
    motor2.setSpeed(SPEED);
    motor3.setSpeed(SPEED);
    motor4.setSpeed(SPEED);

    Serial.println("Backward");
}

void stopMotors() {
    motor1.run(RELEASE);
    motor2.run(RELEASE);
    motor3.run(RELEASE);
    motor4.run(RELEASE);
}