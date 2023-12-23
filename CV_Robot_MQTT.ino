#include <ESP8266WiFi.h> // file modified 
#include <PubSubClient.h>

const char* ssid = "SiAymen";
const char* password = "12345678";
const char* mqtt_server = "91.121.93.94";  // test.mosquitto.org

WiFiClient espClient;
PubSubClient client(espClient);

const int leftFrontPin = D1;    // Replace with the actual GPIO pins you are using on ESP-12E
const int leftBackPin = D2;     // Replace with the actual GPIO pins you are using on ESP-12E
const int left_pwmpin = D0;     // Replace with the actual GPIO pins you are using on ESP-12E

const int rightFrontPin = D7;   // Replace with the actual GPIO pins you are using on ESP-12E
const int rightBackPin = D6;    // Replace with the actual GPIO pins you are using on ESP-12E
const int right_pwmpin = D8;    // Replace with the actual GPIO pins you are using on ESP-12E

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
}

Serial.println("");
if (WiFi.status() == WL_CONNECTED) {
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
} else {
  Serial.println("WiFi connection failed!");
}
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  int speed = 0;
  int mode = 0;

  // Process the received message
  char* message = (char*)payload;

  // Find the index of the comma
  char* commaPos = strchr(message, ',');
  if (commaPos != NULL) {
    // Convert the substring before the comma to speed
    speed = atoi(message);

    // Convert the substring after the comma to mode
    mode = atoi(commaPos + 1);
  }

  // Print received values for verification
  Serial.print("Received data: Speed=");
  Serial.print(speed);
  Serial.print(", mode=");
  Serial.println(mode);

  // Use speed and mode to control your motors
  setMotors(mode, speed);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");

      client.subscribe("device/reception");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(leftFrontPin, OUTPUT);
  pinMode(leftBackPin, OUTPUT);
  pinMode(rightFrontPin, OUTPUT);
  pinMode(rightBackPin, OUTPUT);
  pinMode(left_pwmpin, OUTPUT);
  pinMode(right_pwmpin, OUTPUT);

  digitalWrite(leftFrontPin, LOW);
  digitalWrite(leftBackPin, LOW);
  digitalWrite(rightFrontPin, LOW);
  digitalWrite(rightBackPin, LOW);

  analogWrite(left_pwmpin, 0);
  analogWrite(right_pwmpin, 0);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void setMotors(int mode, int Speed) {
  setDirection(mode);
  analogWrite(left_pwmpin, abs(Speed));
  analogWrite(right_pwmpin, abs(Speed));
}

void setDirection(int mode) {
  if (mode == 1) { // front
    digitalWrite(leftFrontPin, HIGH);
    digitalWrite(leftBackPin, LOW);
    digitalWrite(rightFrontPin, HIGH);
    digitalWrite(rightBackPin, LOW);
  } else if (mode == 2) { // back
    digitalWrite(leftFrontPin, LOW);
    digitalWrite(leftBackPin, HIGH);
    digitalWrite(rightFrontPin, LOW);
    digitalWrite(rightBackPin, HIGH);
  } else if (mode == 3) { // left
    digitalWrite(leftFrontPin, LOW);
    digitalWrite(leftBackPin, HIGH);
    digitalWrite(rightFrontPin, HIGH);
    digitalWrite(rightBackPin, LOW);
  } else if (mode == 4) { // right
    digitalWrite(leftFrontPin, HIGH);
    digitalWrite(leftBackPin, LOW);
    digitalWrite(rightFrontPin, LOW);
    digitalWrite(rightBackPin, HIGH);
  } else if (mode == 0) { // stop
    digitalWrite(leftFrontPin, LOW);
    digitalWrite(leftBackPin, LOW);
    digitalWrite(rightFrontPin, LOW);
    digitalWrite(rightBackPin, LOW);
    analogWrite(left_pwmpin, 0);
    analogWrite(right_pwmpin, 0);
  }
}    