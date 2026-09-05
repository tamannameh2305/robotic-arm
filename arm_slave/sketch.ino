#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "broker.hivemq.com";
const int   mqtt_port   = 1883;
// MUST exactly match the topic in your master sketch
const char* topic = "gloveArm_yourname_2026/angles";

WiFiClient espClient;
PubSubClient client(espClient);

// thumb, index, middle, ring, pinky, wrist, elbow
Servo servos[7];
int servoPins[7] = {13, 12, 14, 27, 26, 25, 33};

void callback(char* topicRecv, byte* payload, unsigned int length) {
  StaticJsonDocument<256> doc;
  DeserializationError err = deserializeJson(doc, payload, length);
  if (err) {
    Serial.print("JSON parse failed: ");
    Serial.println(err.c_str());
    return;
  }

  JsonArray angles = doc["a"];
  if (angles.size() != 7) {
    Serial.println("Unexpected payload size");
    return;
  }

  for (int i = 0; i < 7; i++) {
    int angle = angles[i];
    angle = constrain(angle, 0, 180);
    servos[i].write(angle);
  }
}

void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker...");
    if (client.connect("ArmSlave")) {
      Serial.println("connected");
      client.subscribe(topic);
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  for (int i = 0; i < 7; i++) {
    servos[i].setPeriodHertz(50);
    servos[i].attach(servoPins[i], 500, 2400);
  }

  connectWiFi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnectMQTT();
  client.loop();
}