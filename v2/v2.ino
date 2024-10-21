// Code for two motors, for robot that moves only back and front
#define TRIG_FRONT 12
#define ECHO_FRONT 13
#define TRIG_BACK 14
#define ECHO_BACK 15

#define OUTPUT 1
#define INPUT 0
#define HIGH 1
#define LOW 0
#define MAX_DISTANCE 200
#define DETECT 10

struct motor{
    int EncoderPinA;
    int EncoderPinB;
    int ForwardPin;
    int BackwardPin;
    int EnablePin;
    int EncoderValue;
};
typedef struct motor motor;

motor leftMotor;
motor rightMotor;

int dir = 1;

void update_encoder_left() {
    if(digitalRead(leftMotor.EncoderPinA) > digitalRead(leftMotor.EncoderPinB)) {
        leftMotor.EncoderValue++;
    } else {
        leftMotor.EncoderValue--;
    }
}
void update_encoder_right() {
    if(digitalRead(rightMotor.EncoderPinA) > digitalRead(rightMotor.EncoderPinB)) {
        rightMotor.EncoderValue++;
    } else {
        rightMotor.EncoderValue--;
    }
}

void initialize_motors() {
    leftMotor.EncoderPinA = 2;
    leftMotor.EncoderPinB = 4;
    leftMotor.ForwardPin = 16;
    leftMotor.BackwardPin = 7;
    leftMotor.EnablePin = 17;
    leftMotor.EncoderValue = 0;
    attachInterrupt(digitalPinToInterrupt(leftMotor.EncoderPinA), update_encoder_left, RISING);

    rightMotor.EncoderPinA = 5;
    rightMotor.EncoderPinB = 18;
    rightMotor.ForwardPin = 9;
    rightMotor.BackwardPin = 10;
    rightMotor.EnablePin = 11;
    rightMotor.EncoderValue = 0;
    attachInterrupt(digitalPinToInterrupt(rightMotor.EncoderPinA), update_encoder_right, RISING);
}

void forward() {
    digitalWrite(leftMotor.ForwardPin, HIGH);
    digitalWrite(leftMotor.BackwardPin, LOW);
    digitalWrite(rightMotor.ForwardPin, HIGH);
    digitalWrite(rightMotor.BackwardPin, LOW);
    analogWrite(leftMotor.EnablePin, 255);
    analogWrite(rightMotor.EnablePin, 255);
}

void backward() {
    digitalWrite(leftMotor.ForwardPin, LOW);
    digitalWrite(leftMotor.BackwardPin, HIGH);
    digitalWrite(rightMotor.ForwardPin, LOW);
    digitalWrite(rightMotor.BackwardPin, HIGH);
    analogWrite(leftMotor.EnablePin, 255);
    analogWrite(rightMotor.EnablePin, 255);
}

void stopMotors() {
    digitalWrite(leftMotor.ForwardPin, LOW);
    digitalWrite(leftMotor.BackwardPin, LOW);
    digitalWrite(rightMotor.ForwardPin, LOW);
    digitalWrite(rightMotor.BackwardPin, LOW);
    analogWrite(leftMotor.EnablePin, 0);
    analogWrite(rightMotor.EnablePin, 0);
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

void setup() {
    Serial.begin(9600);
    initialize_motors();
    pinMode(leftMotor.ForwardPin, OUTPUT);
    pinMode(leftMotor.BackwardPin, OUTPUT);
    pinMode(leftMotor.EnablePin, OUTPUT);
    pinMode(rightMotor.ForwardPin, OUTPUT);
    pinMode(rightMotor.BackwardPin, OUTPUT);
    pinMode(rightMotor.EnablePin, OUTPUT);
    pinMode(leftMotor.EncoderPinA, INPUT);
    pinMode(leftMotor.EncoderPinB, INPUT);
    pinMode(rightMotor.EncoderPinA, INPUT);
    pinMode(rightMotor.EncoderPinB, INPUT);

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
