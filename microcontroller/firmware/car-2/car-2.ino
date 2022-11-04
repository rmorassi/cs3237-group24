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
float direction;
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
void updateDynamics(float *varToUpdate, float newValue, float minimum, float maximum, char *varName)
{
    if ((newValue >= minimum) && (newValue <= maximum))
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
        Serial.print(minimum);
        Serial.print(" to ");
        Serial.print(maximum);
        Serial.println(" inclusive)!! Stopping the car as a result...");
        clearToGo = false;
    }
}

/* Update the state of the car depending on the MQTT message that arrived */
void messageMux(int messageSize)
{
    // Get topic and message
    String topic = mqttClient.messageTopic();
    String message = String();
    for (int i = 0; i < messageSize; i++)
    { // Read the entire message
        message += (char)mqttClient.read();
    }

    // Execute specific function depending on topic
    if (topic == DIRECTION_TOPIC)
    {

        float newDirection = message.toFloat();
        updateDynamics(&direction, newDirection, -1.0, 1.0, "Direction Raw");
    }
    else if (topic == SPEED_TOPIC)
    {

        float newSpeed = message.toFloat();
        updateDynamics(&speed, newSpeed, 0.0, 1.0, "Speed Raw");
    }
    else if (topic == CONTROL_TOPIC)
    {

        Serial.print("Control command received: ");
        if (message == "start")
        {
            direction = 0.0; // still
            speed = 1;       // full throttle
            clearToGo = true;
            Serial.println("starting...");
        }
        else if (message == "stop")
        {
            direction = 0.0; // still
            speed = 1;       // full throttle
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
    // Process the state and actually control the car
    controlCar();
}

/* Control the car based on the status variables of the car */
void controlCar()
{
    // Control the car
    if (clearToGo)
    {
        // Set speed (PWM)
        analogWrite(L_SPE, speed * 255);
        analogWrite(R_SPE, speed * 255);

        // Set wheel directions
        if (direction <= -0.5)
        { // Left movement
            digitalWrite(L_FOR, LOW);
            digitalWrite(L_REV, HIGH);
            digitalWrite(R_FOR, HIGH);
            digitalWrite(R_REV, LOW);
        }
        else if (direction >= 0.5)
        { // Right movement
            digitalWrite(L_FOR, HIGH);
            digitalWrite(L_REV, LOW);
            digitalWrite(R_FOR, LOW);
            digitalWrite(R_REV, HIGH);
        }
        else
        { // Stay still
            digitalWrite(L_FOR, LOW);
            digitalWrite(L_REV, LOW);
            digitalWrite(R_FOR, LOW);
            digitalWrite(R_REV, LOW);
        }
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

void setup()
{
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

    // Set up Serial communications
    Serial.begin(115200);
    Serial.println();

    // Set up Wireless network
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Attempting to connect to WPA network...");
        Serial.print("SSID: ");
        Serial.println(SSID);

        WiFi.mode(WIFI_STA);        // Set WiFi mode
        WiFi.begin(SSID, PASSWORD); // Connect to network

        Serial.print("Waiting for result of connection attempt: ");
        switch (WiFi.waitForConnectResult())
        {
        case WL_IDLE_STATUS:
            Serial.println("Wi-Fi is in process of changing between statuses");
            break;
        case WL_NO_SSID_AVAIL:
            Serial.println("Configured SSID cannot be reached");
            break;
        case WL_CONNECTED:
            Serial.println("Successful connection is established");
            break;
        case WL_CONNECT_FAILED:
            Serial.println("Connection failed");
            break;
        case WL_WRONG_PASSWORD:
            Serial.println("Password is incorrect");
            break;
        case WL_DISCONNECTED:
            Serial.println("Module is not configured in station mode");
            break;
        default:
            Serial.println("Some unknown status was returned...");
            break;
        }
    }
    Serial.print("Connected to WiFi. IP Address: ");
    Serial.println(WiFi.localIP());

    // Set up Wireless communications (MQTT)
    Serial.print("Attempting to connect to the MQTT broker: ");
    Serial.println(BROKER);
    while (!mqttClient.connect(BROKER, PORT))
    {
        Serial.print("MQTT connection failed! Error code: ");
        Serial.println(mqttClient.connectError());
        Serial.println("Attempting connection again...");
    }
    Serial.println("You're connected to the MQTT broker!");
    // Setup subscribers
    mqttClient.onMessage(messageMux);
    mqttClient.subscribe(DIRECTION_TOPIC);
    mqttClient.subscribe(SPEED_TOPIC);
    mqttClient.subscribe(CONTROL_TOPIC);

    // Setup vehicle dynamics
    direction = 0.0;   // still
    speed = 1;         // full throttle
    clearToGo = false; // not clear to start

    // Confirm setup done
    mqttClient.beginMessage(SETUP_TOPIC);
    mqttClient.print("SETUP DONE!");
    mqttClient.endMessage();
}

void loop()
{
    mqttClient.poll();
}
