#include <WiFi.h>
#include <ThingSpeak.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include <ArduinoJson.h>

#define TRIG_BACK 26
#define ECHO_BACK 14
#define TRIG_FRONT 33
#define ECHO_FRONT 35
#define TRIG_LEFT 25
#define ECHO_LEFT 27
#define TRIG_RIGHT 32
#define ECHO_RIGHT 34

#define MAX_DISTANCE 200
#define MIN_SPACE 20
#define DETECT 20

#define TURNING_SPEED 40
#define MOVING_SPEED_LEFT 150
#define MOVING_SPEED_RIGHT 10

struct motor {
    int ForwardPin;
    int BackwardPin;
    int EnablePin;
};
typedef struct motor motor;

motor leftMotor;
motor rightMotor;

void initialize_motors() {
    leftMotor.ForwardPin = 22;
    leftMotor.BackwardPin = 12;
    leftMotor.EnablePin = 23;

    rightMotor.ForwardPin = 21;
    rightMotor.BackwardPin = 15;
    rightMotor.EnablePin = 19;
}

void stopLeft() {
    for (int i = 255; i > 0; i--) {
        analogWrite(leftMotor.EnablePin, i);
        delay(10);
    }
    digitalWrite(leftMotor.ForwardPin, LOW);
    digitalWrite(leftMotor.BackwardPin, LOW);
}

void stopRight() {
    for (int i = 255; i > 0; i--) {
        analogWrite(rightMotor.EnablePin, i);
        delay(10);
    }
    digitalWrite(rightMotor.ForwardPin, LOW);
    digitalWrite(rightMotor.BackwardPin, LOW);
}

float robotX = 0.0;
float robotY = 0.0;

void calculateAndPublishCoordinates(int distanceFront, int distanceLeft, int distanceRight) {
    float frontX = distanceFront != MAX_DISTANCE ? robotX + distanceFront : -1;
    float frontY = distanceFront != MAX_DISTANCE ? robotY : -1;

    float leftX = distanceLeft != MAX_DISTANCE ? robotX : -1;
    float leftY = distanceLeft != MAX_DISTANCE ? robotY + distanceLeft : -1;

    float rightX = distanceRight != MAX_DISTANCE ? robotX : -1;
    float rightY = distanceRight != MAX_DISTANCE ? robotY - distanceRight : -1;

    mqttPublish(channelID, "field3=" + String(frontX));
    mqttPublish(channelID, "field4=" + String(frontY));
    mqttPublish(channelID, "field3=" + String(leftX));
    mqttPublish(channelID, "field4=" + String(leftY));
    mqttPublish(channelID, "field3=" + String(rightX));
    mqttPublish(channelID, "field4=" + String(rightY));

    Serial.print("Obstacle Front: (");
    Serial.print(frontX);
    Serial.print(", ");
    Serial.print(frontY);
    Serial.println(")");

    Serial.print("Obstacle Left: (");
    Serial.print(leftX);
    Serial.print(", ");
    Serial.print(leftY);
    Serial.println(")");

    Serial.print("Obstacle Right: (");
    Serial.print(rightX);
    Serial.print(", ");
    Serial.print(rightY);
    Serial.println(")");
}

void forward() {
    digitalWrite(leftMotor.ForwardPin, HIGH);
    digitalWrite(leftMotor.BackwardPin, LOW);
    analogWrite(leftMotor.EnablePin, MOVING_SPEED_LEFT);

    digitalWrite(rightMotor.ForwardPin, HIGH);
    digitalWrite(rightMotor.BackwardPin, LOW);
    analogWrite(rightMotor.EnablePin, MOVING_SPEED_RIGHT);
}

void stopMotors() {
    stopLeft();
    stopRight();
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

void turnLeft() {
    stopLeft(); // Stop left motor
    digitalWrite(rightMotor.ForwardPin, HIGH);
    digitalWrite(rightMotor.BackwardPin, LOW);
    analogWrite(rightMotor.EnablePin, TURNING_SPEED);

    delay(1000); // Adjust delay for turning
    stopRight(); // Stop right motor after turn
}

void turnRight() {
    stopRight(); // Stop right motor
    digitalWrite(leftMotor.ForwardPin, HIGH);
    digitalWrite(leftMotor.BackwardPin, LOW);
    analogWrite(leftMotor.EnablePin, TURNING_SPEED);

    delay(1000); // Adjust delay for turning
    stopLeft(); // Stop left motor after turn
}

char ssid[] = "Disha";
char pass[] = "svkl2978";

#define channelID 2749278 //should not be a string, just an int
const char mqttUserName[] = "BjQdLBkbKR4PGy4IIywhJAU"; 
const char clientID[] = "BjQdLBkbKR4PGy4IIywhJAU";
const char mqttPass[] = "Fxgqhx4QvdcrjxFmazTlKanN";

const char * PROGMEM thingspeak_ca_cert = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n" \
"ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL\n" \
"MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n" \
"LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug\n" \
"RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm\n" \
"+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW\n" \
"PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM\n" \
"xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB\n" \
"Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3\n" \
"hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg\n" \
"EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF\n" \
"MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA\n" \
"FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec\n" \
"nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z\n" \
"eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF\n" \
"hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2\n" \
"Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe\n" \
"vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep\n" \
"+OkuE6N36B9K\n" \
"-----END CERTIFICATE-----\n";

#define mqttPort 1883
WiFiClient client;

const char* server = "mqtt3.thingspeak.com";
const char* server1 = "api.thingspeak.com";
const char* apiKey = "50QMQKRPSTGYJ1UM";
const int fieldNum = 1;
int status = WL_IDLE_STATUS; 
long lastPublishMillis = 0;
int connectionDelay = 1;
int updateInterval = 15;
PubSubClient mqttClient( client );


// Function to handle messages from MQTT subscription.
void mqttSubscriptionCallback( char* topic, byte* payload, unsigned int length ) {
  // Print the details of the message that was received to the serial monitor.
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// Subscribe to ThingSpeak channel for updates.
void mqttSubscribe( long subChannelID ){
  String myTopic = "channels/"+String( subChannelID )+"/subscribe";
  mqttClient.subscribe(myTopic.c_str());
}

// Publish messages to a ThingSpeak channel.
void mqttPublish(long pubChannelID, String message) {
  String topicString ="channels/" + String( pubChannelID ) + "/publish";
  mqttClient.publish( topicString.c_str(), message.c_str() );
}

void connectWifi()
{
  Serial.print( "Connecting to Wi-Fi..." );
  // Loop until WiFi connection is successful
  while ( WiFi.status() != WL_CONNECTED ) {
    WiFi.begin( ssid, pass );
    delay( connectionDelay*1000 );
    Serial.print( WiFi.status() ); 
  }
  Serial.println( "Connected to Wi-Fi." );
}

// Connect to MQTT server.
void mqttConnect() {
  // Loop until connected.
  while ( !mqttClient.connected() )
  {
    // Connect to the MQTT broker.
    if ( mqttClient.connect( clientID, mqttUserName, mqttPass ) ) {
      Serial.print( "MQTT to " );
      Serial.print( server );
      Serial.print (" at port ");
      Serial.print( mqttPort );
      Serial.println( " successful." );
    } else {
      Serial.print( "MQTT connection failed, rc = " );
      // See https://pubsubclient.knolleary.net/api.html#state for the failure code explanation.
      Serial.print( mqttClient.state() );
      Serial.println( " Will try again in a few seconds" );
      delay( connectionDelay*1000 );
    }
  }
}
String get_feild1(String s) {
    String result = "";
    for (int i = s.length() - 1; i >= 0; i--) {
        if (s[i] == ':') {
            for (int j = i + 1; j < s.length(); j++) {
                if (s[j] == '"') {
                    for (int k = j + 1; k < s.length(); k++) {
                        if (s[k] == '"') {
                            return result;
                        } else {
                            result += s[k];
                        }
                    }
                }
            }
        }
    }
    return "";
}
String parse_at_curly_braces(String s) {
    String result = "";
    bool is_curly_braces = false;
    for (int i = 0; i < s.length(); i++) {
        if (s[i] == '{') {
            is_curly_braces = true;
        } else if (s[i] == '}') {
            is_curly_braces = false;
        } else if (is_curly_braces) {
            result += s[i];
        }
    }
    return '{' + result + '}';
}
String fetchLatestEntry() {
  if (!client.connect(server1, 80)) {
    Serial.println("Connection failed");
  }
  
  // Make HTTP request to ThingSpeak API
  String url = "/channels/2506706/fields/" + String(fieldNum) + "/last.json?api_key=" + apiKey;
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + server1 + "\r\n" +
               "Connection: close\r\n\r\n");
  
  delay(1000);
  
  // Read response
  String response = "";
  while (client.available()) {
    char c = client.read();
    response += c;
  }
  String s = (get_feild1(parse_at_curly_braces(response)));
  return s;

}

int convert(String s) 
{ 
    int i = 0, sign = 1, num = 0;
    if (s[0] == '-') {
        sign = -1;
        i++;
    }
    while (s[i] != '\0') {
        num = num * 10 + (s[i] - '0');
        i++;
    }
    num *= sign;

    return num;
} 


void setup() {
    initialize_motors();
    pinMode(leftMotor.ForwardPin, OUTPUT);
    pinMode(leftMotor.BackwardPin, OUTPUT);
    pinMode(leftMotor.EnablePin, OUTPUT);
    pinMode(rightMotor.ForwardPin, OUTPUT);
    pinMode(rightMotor.BackwardPin, OUTPUT);
    pinMode(rightMotor.EnablePin, OUTPUT);

    pinMode(TRIG_FRONT, OUTPUT);
    pinMode(ECHO_FRONT, INPUT);
    pinMode(TRIG_BACK, OUTPUT);
    pinMode(ECHO_BACK, INPUT);
    pinMode(TRIG_LEFT, OUTPUT);
    pinMode(ECHO_LEFT, INPUT);
    pinMode(TRIG_RIGHT, OUTPUT);
    pinMode(ECHO_RIGHT, INPUT);

    connectWifi();
    // Configure the MQTT client
    mqttClient.setServer( server, mqttPort ); 
    // Set the MQTT message handler function.
    mqttClient.setCallback( mqttSubscriptionCallback );
    // Set the buffer to handle the returned JSON. NOTE: A buffer overflow of the message buffer will result in your callback not being invoked.
    mqttClient.setBufferSize( 2048 );

    Serial.begin(115200);

    stopMotors();

    int distanceFront = getDistance(TRIG_FRONT, ECHO_FRONT);
    mqttPublish(channelID, "field5=" + String(distanceFront));
    int distanceLeft = getDistance(TRIG_LEFT, ECHO_LEFT);
    mqttPublish(channelID, "field6=" + String(distanceLeft));
    int distanceRight = getDistance(TRIG_RIGHT, ECHO_RIGHT);
    mqttPublish(channelID, "field7=" + String(distanceRight));
    calculateAndPublishCoordinates(distanceFront, distanceLeft, distanceRight);
    forward();
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        connectWifi();
    }

    // Connect if MQTT client is not connected and resubscribe to channel updates.
    if (!mqttClient.connected()) {
        mqttConnect(); 
        mqttSubscribe( channelID );
    }

    // Call the loop to maintain connection to the server.
    mqttClient.loop(); 

    int distanceFront = getDistance(TRIG_FRONT, ECHO_FRONT);
    mqttPublish(channelID, "field5=" + String(distanceFront));
    int distanceLeft = getDistance(TRIG_LEFT, ECHO_LEFT);
    mqttPublish(channelID, "field6=" + String(distanceLeft));
    int distanceRight = getDistance(TRIG_RIGHT, ECHO_RIGHT);
    mqttPublish(channelID, "field7=" + String(distanceRight));

    Serial.print("Front: ");
    Serial.print(distanceFront);
    Serial.print(" Left: ");
    Serial.print(distanceLeft);
    Serial.print(" Right: ");
    Serial.println(distanceRight);

    if (distanceFront < DETECT) {
        if (distanceLeft < MIN_SPACE && distanceRight < MIN_SPACE) {
            Serial.println("No way forward, stopping motors.");
            stopMotors();
            while (1) {} // Stuck, stop program
        } else if (distanceLeft < MIN_SPACE) {
            Serial.println("Front and Left obstructed, turning right.");
            turnRight();
        } else if (distanceRight < MIN_SPACE) {
            Serial.println("Front and Right obstructed, turning left.");
            turnLeft();
        } else {
            Serial.println("Only front obstructed, turning left.");
            turnLeft();
        }
    } else {
        forward();
    }

    delay(500); // Delay for sensor stabilization
}