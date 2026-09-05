#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "broker.hivemq.com";
const int   mqtt_port   = 1883;
const char* topic = "gloveArm_yourname_2026/angles";

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_MPU6050 mpu;

// thumb, index, middle, ring, pinky, wrist
const int sensorPins[6] = {32, 33, 34, 35, 36, 39};
int rawMin[6] = {0, 0, 0, 0, 0, 0};
int rawMax[6] = {4095, 4095, 4095, 4095, 4095, 4095};

unsigned long lastPublish = 0;
const unsigned long publishInterval = 100;

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
    if (client.connect("GloveMaster")) {
      Serial.println("MQTT connected");
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  for (int i = 0; i < 6; i++) pinMode(sensorPins[i], INPUT);

  if (!mpu.begin()) {
    Serial.println("MPU6050 not found!");
  } else {
    Serial.println("MPU6050 ready");
  }

  connectWiFi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) reconnectMQTT();
  client.loop();

  unsigned long now = millis();
  if (now - lastPublish >= publishInterval) {
    lastPublish = now;

    StaticJsonDocument<256> doc;
    JsonArray angles = doc.createNestedArray("a");

    for (int i = 0; i < 6; i++) {
      int raw = analogRead(sensorPins[i]);
      int angle = map(raw, rawMin[i], rawMax[i], 0, 180);
      angle = constrain(angle, 0, 180);
      angles.add(angle);
      Serial.print("S"); Serial.print(i); Serial.print(":"); Serial.print(angle); Serial.print("  ");
    }

    // ---- Elbow angle from MPU6050 tilt ----
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    float elbowAngle = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / PI;
    elbowAngle = abs(elbowAngle);
    elbowAngle = constrain(elbowAngle, 0, 180);
    angles.add((int)elbowAngle);
    Serial.print("Elbow:"); Serial.println((int)elbowAngle);

    char buffer[256];
    size_t len = serializeJson(doc, buffer);
    client.publish(topic, buffer, len);
  }
}