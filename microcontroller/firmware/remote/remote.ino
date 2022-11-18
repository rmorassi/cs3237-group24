/* Include libraries needed */
#include <ESP8266WiFi.h>
#include <ArduinoMqttClient.h>
#include <ADS1115_WE.h>
#include <Wire.h>

/* ADS1115 */
#define I2C_ADDRESS 0x48
ADS1115_WE adc;

/* MQTT */
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

/* Constants */
#include "constants.h"

/* Controller information */
// true when the remote is publishing manual direction control (with joystick)
bool manualDirection;
// true when car has started
String lastControl;
// Store the speed of the car
float speed;
// Store the direction the car is instructed to face
float direction;

/* Debouncing */
volatile unsigned long time_dir_btn, time_ctrl_btn, time_spd_btn;

/* Restart the microcontroller if something went terribly wrong */
void restartESP()
{
    Serial.println("Restarting ESP...");
    delay(2000);
    ESP.restart();
}

/* Update the state depending on the MQTT message that arrived */
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
    if (topic == CONTROL_TOPIC)
    {
        Serial.print("Control command received: ");
        if (message == "start")
        {
            digitalWrite(LED, HIGH);
            Serial.println("Start. Turning LED On.");
            lastControl = message;
        }
        else if (message == "stop")
        {
            digitalWrite(LED, LOW);
            Serial.println("Stop. Turning LED Off.");
            lastControl = message;
        }
        else
        {
            Serial.print("message contents not recognised (");
            Serial.print(message);
            Serial.println(")!");
        }
    }
    else
    {
        Serial.print("Received a message from topic: ");
        Serial.print(topic);
        Serial.println(" but messageMux does not cover this topic.");
    }
}

float readChannel(ADS1115_MUX channel)
{
    adc.setCompareChannels(channel);
    float percentage = adc.getResult_V() / 3.3;
    return percentage;
}

ICACHE_RAM_ATTR void manualDirectionToggle()
{
    // Debounce
    unsigned long currTime = millis();
    if (currTime - time_dir_btn < DEBOUCE_TIME)
        return;
    time_dir_btn = currTime;

    manualDirection = !manualDirection;
    Serial.println("Manual direction toggled");
};

ICACHE_RAM_ATTR void controlToggle()
{
    // Debounce
    unsigned long currTime = millis();
    if (currTime - time_ctrl_btn < DEBOUCE_TIME)
        return;
    time_ctrl_btn = currTime;

    lastControl = (lastControl == "stop") ? "start" : "stop";

    mqttClient.beginMessage(CONTROL_TOPIC);
    mqttClient.print(lastControl);
    mqttClient.endMessage();

    Serial.print("Control command sent: ");
    Serial.println(lastControl);
};

ICACHE_RAM_ATTR void speedPressed()
{
    // Debounce
    unsigned long currTime = millis();
    if (currTime - time_spd_btn < DEBOUCE_TIME)
        return;
    time_spd_btn = currTime;

    float speed = readChannel(JOYSTICK_Y);
    speed = (speed < 0) ? 0 : ((speed > 1) ? 1 : speed); // should vary between 0 and 1
    speed = ((float)(int)(speed * 100)) / 100;           // round to two decimal places

    mqttClient.beginMessage(SPEED_TOPIC);
    mqttClient.print(speed);
    mqttClient.endMessage();

    Serial.print("Speed command sent: ");
    Serial.println(speed);
};

void setup()
{
    // Setup I/O
    // Set all the motor control pins to outputs
    pinMode(DIR_BTN, INPUT_PULLUP);
    pinMode(CTRL_BTN, INPUT_PULLUP);
    pinMode(SPD_BTN, INPUT_PULLUP);
    pinMode(LED, OUTPUT);
    // Turn off outputs
    digitalWrite(LED, LOW);
    // Setup interrupts
    attachInterrupt(digitalPinToInterrupt(DIR_BTN), manualDirectionToggle, FALLING);
    attachInterrupt(digitalPinToInterrupt(CTRL_BTN), controlToggle, FALLING);
    attachInterrupt(digitalPinToInterrupt(SPD_BTN), speedPressed, FALLING);
    // Debouncing
    time_dir_btn = millis();
    time_ctrl_btn = millis();
    time_spd_btn = millis();

    // Set up Serial communications
    Serial.begin(115200);
    Serial.println();

    // Set up A2C and ADS1115
    adc = ADS1115_WE(I2C_ADDRESS);
    Wire.begin();
    while (!adc.init())
    {
        Serial.println("ADS1115 not connected! Retrying in 3 seconds...");
        delay(3000);
    }
    adc.setVoltageRange_mV(ADS1115_RANGE_6144); // 3v (each side since +/-)
    adc.setMeasureMode(ADS1115_CONTINUOUS);
    Serial.println("A2C and ADS1115 configured");

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
        delay(3000);
        Serial.println("Attempting connection again...");
    }
    Serial.println("You're connected to the MQTT broker!");
    // Setup subscribers
    mqttClient.onMessage(messageMux);
    mqttClient.subscribe(CONTROL_TOPIC);

    // Setup controller info
    manualDirection = false;
    lastControl = "NOT SET";
    speed = 1;
    direction = 0;

    // Confirm setup done
    mqttClient.beginMessage(SETUP_TOPIC);
    mqttClient.print("SETUP DONE!");
    mqttClient.endMessage();
}

void loop()
{
    mqttClient.poll();

    // Manual direction
    if (manualDirection)
    {
        float currDir = readChannel(JOYSTICK_X);
        currDir = (currDir - 0.5) * 2;
        currDir = (currDir < -1) ? -1 : ((currDir > 1) ? 1 : currDir); // should vary between -1 and 1
        currDir = ((float)(int)(currDir * 100)) / 100;                 // round to two decimal places

        if (currDir != direction)
        {
            direction = currDir;

            mqttClient.beginMessage(DIRECTION_TOPIC);
            mqttClient.print(direction);
            mqttClient.endMessage();

            Serial.print("DIRECTION: ");
            Serial.print(direction);
            Serial.print(". ");
        }
    }
}
