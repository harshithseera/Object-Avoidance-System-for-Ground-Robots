#include <WiFi.h>
#include <ThingSpeak.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include <ArduinoJson.h>

// Definitions
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

// Ensure correct credentials to connect to your WiFi Network.
char ssid[] = "seera";
char pass[] = "afwd3291";

// Ensure that the credentials here allow you to publish and subscribe to the ThingSpeak channel.
#define channelID 2506706 //should not be a string, just an int
const char mqttUserName[] = "CSodJDMlGCIHKA8LJB8JES0"; 
const char clientID[] = "CSodJDMlGCIHKA8LJB8JES0";
const char mqttPass[] = "uAbYatwOcDfR28N+7S4Wnp10";

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

// Connect to WiFi.
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
  // put your setup code here, to run once:
  Serial.begin( 115200 );
  // Delay to allow serial monitor to come up.
  delay(3000);
  // Connect to Wi-Fi network.
  connectWifi();
  // Configure the MQTT client
  mqttClient.setServer( server, mqttPort ); 
  // Set the MQTT message handler function.
  mqttClient.setCallback( mqttSubscriptionCallback );
  // Set the buffer to handle the returned JSON. NOTE: A buffer overflow of the message buffer will result in your callback not being invoked.
  mqttClient.setBufferSize( 2048 );

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
  // Reconnect to WiFi if it gets disconnected.
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
      mqttPublish( channelID, (String("field1=")+String(dir)));
  }
  if(dir == -1 && distanceBack < DETECT) {
      stopMotors();
      forward();
      // delay(1000);
      dir = 1;
      mqttPublish( channelID, (String("field1=")+String(dir)));
  }

  delay(1000);
  /**unsigned long currentTimestamp = millis();
  if (currentTimestamp - lastHourTimestamp >= 3600000) { // 3600000 milliseconds = 1 hour
    // Publish the number of entries and exits for the current hour to fields 3 and 4
    mqttPublish( channelID, (String("field2=")+String(entriesThisHour)));
    mqttPublish( channelID, (String("field3=")+String(exitsThisHour)));
    // Reset the counters and update the timestamp
    entriesThisHour = 0;
    exitsThisHour = 0;
    lastHourTimestamp = currentTimestamp;
  }
  delay(1000);**/
}
