// Code for two motors, for robot that moves only back and front
#define TRIG_FRONT 25
#define ECHO_FRONT 26
#define TRIG_BACK 32
#define ECHO_BACK 33

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

void IRAM_ATTR update_encoder_left() {
    if(digitalRead(leftMotor.EncoderPinA) > digitalRead(leftMotor.EncoderPinB)) {
        leftMotor.EncoderValue++;
    } else {
        leftMotor.EncoderValue--;
    }
}
void IRAM_ATTR update_encoder_right() {
    if(digitalRead(rightMotor.EncoderPinA) > digitalRead(rightMotor.EncoderPinB)) {
        rightMotor.EncoderValue++;
    } else {
        rightMotor.EncoderValue--;
    }
}

void initialize_motors() {
    leftMotor.EncoderPinA = 16; //C1
    leftMotor.EncoderPinB = 4; //C2
    leftMotor.ForwardPin = 22;
    leftMotor.BackwardPin = 17;
    leftMotor.EnablePin = 23;
    leftMotor.EncoderValue = 0;
    attachInterrupt(digitalPinToInterrupt(leftMotor.EncoderPinA), update_encoder_left, RISING);

    rightMotor.EncoderPinA = 18;
    rightMotor.EncoderPinB = 5;
    rightMotor.ForwardPin = 21;
    rightMotor.BackwardPin = 15;
    rightMotor.EnablePin = 19;
    rightMotor.EncoderValue = 0;
    attachInterrupt(digitalPinToInterrupt(rightMotor.EncoderPinA), update_encoder_right, RISING);
}

void forward() {
    digitalWrite(rightMotor.ForwardPin, HIGH);
    digitalWrite(rightMotor.BackwardPin, LOW);
    analogWrite(rightMotor.EnablePin, 255);
    digitalWrite(leftMotor.ForwardPin, HIGH);
    digitalWrite(leftMotor.BackwardPin, LOW);
    analogWrite(leftMotor.EnablePin, 255);
}

void backward() {
    digitalWrite(rightMotor.ForwardPin, LOW);
    digitalWrite(rightMotor.BackwardPin, HIGH);
    analogWrite(rightMotor.EnablePin, 255);
    digitalWrite(leftMotor.ForwardPin, LOW);
    digitalWrite(leftMotor.BackwardPin, HIGH);
    analogWrite(leftMotor.EnablePin, 255);
}

void stopMotors() {
    for(int i = 255; i > 0; i--) {
        analogWrite(leftMotor.EnablePin, i);
        analogWrite(rightMotor.EnablePin, i);
        delay(10);
    }
    digitalWrite(leftMotor.ForwardPin, LOW);
    digitalWrite(leftMotor.BackwardPin, LOW);
    digitalWrite(rightMotor.ForwardPin, LOW);
    digitalWrite(rightMotor.BackwardPin, LOW);
    // analogWrite(leftMotor.EnablePin, 0);
    // analogWrite(rightMotor.EnablePin, 0);
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
    // return distance;
}

void setup() {
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

    Serial.begin(115200);

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
        // delay(1000);
        dir = -1;
    }
    if(dir == -1 && distanceBack < DETECT) {
        stopMotors();
        forward();
        // delay(1000);
        dir = 1;
    }

    delay(1000);
}
