/*
All constants defined in constants.h .
This program sends MQTT messages every INTERVAL, and prints out any receiving messages.
*/

/* Include libraries needed */
#include <ESP8266WiFi.h>
#include <ArduinoMqttClient.h>

/* MQTT */
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

/* Constants */
#include "constants.h"

/* Miscellaneous */
unsigned long previousMillis = 0;
int count = 0;

void restartESP() {
  Serial.println("Restarting ESP...");
  delay(2000);
  ESP.restart();
}

void setup() {

  // Set up Serial communications
  Serial.begin(115200);
  Serial.println();

  // Set up Wireless network
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Connect Failed!");
    restartESP();
  }
  Serial.print("Connected to Wifi. IP Address: ");
  Serial.println(WiFi.localIP());

  // Set up Wireless communications (MQTT)
  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(BROKER);
  if (!mqttClient.connect(BROKER, PORT)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    restartESP();
  }
  Serial.println("You're connected to the MQTT broker!");
  // Setup subscribers
  mqttClient.onMessage(onMqttMessage);
  mqttClient.subscribe(RECEIVE_TOPIC);
}

void loop() {
  // call poll() regularly to allow the library to receive MQTT messages and
  // send MQTT keep alives which avoids being disconnected by the broker
  mqttClient.poll();

  // to avoid having delays in loop, we'll use the strategy from BlinkWithoutDelay
  // see: File -> Examples -> 02.Digital -> BlinkWithoutDelay for more info
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= INTERVAL) {
    // save the last time a message was sent
    previousMillis = currentMillis;

    Serial.print("Sending message to topic: ");
    Serial.println(SEND_TOPIC);
    Serial.print("hello ");
    Serial.println(count);

    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(SEND_TOPIC);
    mqttClient.print("hello ");
    mqttClient.print(count);
    mqttClient.endMessage();

    Serial.println();

    count++;
  }
}

void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
  Serial.println("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    Serial.print((char)mqttClient.read());
  }
  Serial.println();

  Serial.println();
}
