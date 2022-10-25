/* Include libraries needed */
#include <ESP8266WiFi.h>
#include <ArduinoMqttClient.h>

/* MQTT */
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

/* Constants */
#include "constants.h"

/* Vehicle dynamics */
// Between -1 and 1
float directionRaw;
// Between -1 and 1
float directionGain;
// Between 0 and 1
float speed;
// true when the car can safely proceed
bool clearToGo;

/* Restart the microcontroller if something went terribly wrong */
void restartESP()
{
  Serial.println("Restarting ESP...");
  delay(2000);
  ESP.restart();
}

/* Update vehicle dynamics float and check if value lies within specified range. Stop the car if it lies outside */
void updateDynamics(float *varToUpdate, float newValue, float min, float max, char *varName)
{
  if ((newValue >= min) && (newValue <= max))
  {
    Serial.print(varName);
    Serial.print(" received: ");
    Serial.println(newValue);
    *varToUpdate = newValue;
  }
  else
  {
    Serial.print(varName);
    Serial.print(" received is outside range (from ");
    Serial.print(min);
    Serial.print(" to ");
    Serial.print(max);
    Serial.println(" inclusive)!! Stopping the car as a result...");
    clearToGo = false;
  }
}

/* Do specific actions depending on the topic of the message that arrived */
void messageMux(int messageSize)
{
  // Get topic and message
  String topic = mqttClient.messageTopic();
  char message[messageSize];
  for (int i = 0; i < messageSize; i++)
  { // Read the entire message
    message[messageSize] = (char)mqttClient.read();
  }

  // Execute specific function depending on topic
  if (topic == DIRECTION_RAW_TOPIC)
  {

    float newDirectionRaw = atof(message);
    updateDynamics(&directionRaw, newDirectionRaw, -1.0, 1.0, "Direction Raw");
  }
  else if (topic == DIRECTION_GAIN_TOPIC)
  {

    float newDirectionGain = atof(message);
    updateDynamics(&directionGain, newDirectionGain, -1.0, 1.0, "Direction Gain");
  }
  else if (topic == SPEED_TOPIC)
  {

    float newSpeed = atof(message);
    updateDynamics(&speed, newSpeed, 0.0, 1.0, "Speed Raw");
  }
  else if (topic == CONTROL_TOPIC)
  {

    Serial.print("Control command received: ");
    if (message == "start")
    {
      clearToGo = true;
      Serial.println("starting...");
    }
    else if (message == "stop")
    {
      clearToGo = false;
      Serial.println("stopping...");
    }
    else
    {
      clearToGo = false;
      Serial.print("message contents not recognised (");
      Serial.print(message);
      Serial.println(")! stopping anyways...");
    }
  }
  else
  {

    Serial.print("Received a message from topic: ");
    Serial.print(topic);
    Serial.println(" but messageMux does not cover this topic.");
  }
}

void setup()
{
  // Set up Serial communications
  Serial.begin(115200);
  Serial.println();

  // Set up Wireless network
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("WiFi Connect Failed!");
    restartESP();
  }
  Serial.print("Connected to Wifi. IP Address: ");
  Serial.println(WiFi.localIP());

  // Set up Wireless communications (MQTT)
  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(BROKER);
  if (!mqttClient.connect(BROKER, PORT))
  {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    restartESP();
  }
  Serial.println("You're connected to the MQTT broker!");
  // Setup subscribers
  mqttClient.onMessage(messageMux);
  mqttClient.subscribe(DIRECTION_RAW_TOPIC);
  mqttClient.subscribe(DIRECTION_GAIN_TOPIC);
  mqttClient.subscribe(SPEED_TOPIC);
  mqttClient.subscribe(CONTROL_TOPIC);

  // Setup I/O
  // Set all the motor control pins to outputs
  pinMode(L_SPE, OUTPUT);
  pinMode(R_SPE, OUTPUT);
  pinMode(L_FOR, OUTPUT);
  pinMode(L_REV, OUTPUT);
  pinMode(R_FOR, OUTPUT);
  pinMode(R_REV, OUTPUT);
  // Turn off motors - Initial state
  analogWrite(L_SPE, 0);
  analogWrite(R_SPE, 0);
  digitalWrite(L_FOR, LOW);
  digitalWrite(L_REV, LOW);
  digitalWrite(R_FOR, LOW);
  digitalWrite(R_REV, LOW);

  // Setup vehicle dynamics
  directionRaw = 0.0;  // straight
  directionGain = 0.0; // no gain
  speed = 0.5;         // half throttle
  clearToGo = false;   // not clear to start
}

void loop()
{
  mqttClient.poll();

  // Control the car
  if (clearToGo)
  {
    // Calculate final direction
    float directionFinal = directionRaw + (directionRaw * directionGain * DIRECTION_GAIN_MULTIPLIER);
    directionFinal = (directionFinal < -1) ? -1 : ((directionFinal > 1) ? 1 : directionFinal);

    // Calculate pwm for each side
    int leftPWM = (directionFinal < 0) ? 255 * speed * abs(directionFinal) : 255 * speed;
    int rightPWM = (directionFinal > 0) ? 255 * speed * directionFinal : 255 * speed;

    // Set PWM
    analogWrite(L_SPE, leftPWM);
    analogWrite(R_SPE, rightPWM);

    // Forward movement
    digitalWrite(L_FOR, HIGH);
    digitalWrite(L_REV, LOW);
    digitalWrite(R_FOR, HIGH);
    digitalWrite(R_REV, LOW);
  }
  else
  {
    // Stop the car
    analogWrite(L_SPE, 0);
    analogWrite(R_SPE, 0);
    digitalWrite(L_FOR, LOW);
    digitalWrite(L_REV, LOW);
    digitalWrite(R_FOR, LOW);
    digitalWrite(R_REV, LOW);
  }
}
