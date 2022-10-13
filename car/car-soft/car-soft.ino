/* Include libraries needed */
#include <ESP8266WiFi.h>
#include <ArduinoWebsockets.h>

/* Information for wireless network */
#define SSID "Network Name"
#define PASSWORD "Netw0rk_passphrasE"

/* Input/Output */
#define LED1 1
#define LED2 2

/* WebSocket */
using namespace websockets;
WebsocketsClient client;
#define WEB_SOCKET_ADDRESS "some.address"
#define WEB_SOCKET_PORT 80

void setup() {
  // Miscellaneous
  pinMode(LED_BUILTIN, OUTPUT);

  // Set up motor output
  pinMode(LED1, OUTPUT); // Simulate motor control for now by editing the brightness of LED
  analogWrite(LED1, 0);
  pinMode(LED2, OUTPUT);
  analogWrite(LED2, 0);

  // Set up Serial communications
  Serial.begin(9600);
  Serial.println();

  // Set up Wireless network
  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Connect Failed! Rebooting...");
    delay(1000);
    ESP.restart();
  }
  Serial.print("Connected to Wifi. IP Address: ");
  Serial.println(WiFi.localIP());

  // Set up Wireless communications (WebSocket)
  // Setup Callbacks
  client.onMessage(onMessageCallback);
  client.onEvent(onEventsCallback);
  // Connect to server
  if (!client.connect(WEB_SOCKET_ADDRESS, WEB_SOCKET_PORT, "/")) {
    Serial.println("WebSocket Connect Failed! Rebooting...");
    delay(1000);
    ESP.restart();
  }
  // Send a message
  client.send("Hi Server!");
  // Send a ping
  client.ping();
}

void onMessageCallback(WebsocketsMessage message) {
  Serial.print("Got Message: ");
  Serial.println(message.data());
}

void onEventsCallback(WebsocketsEvent event, String data) {
  if(event == WebsocketsEvent::ConnectionOpened) {
    Serial.println("Connnection Opened");
  } else if(event == WebsocketsEvent::ConnectionClosed) {
    Serial.println("Connnection Closed");
  } else if(event == WebsocketsEvent::GotPing) {
    Serial.println("Got a Ping!");
  } else if(event == WebsocketsEvent::GotPong) {
    Serial.println("Got a Pong!");
  } else {
    Serial.println("Got some event.")
  }
}

// Control the car based on the desired direction of travel.
// Direction can range from -1 to 1 indicating the 180 degrees in front of the car. 0 being straight in front.
void controlCar(int direction) {
  int leftSide = (direction < 0) ? (-direction * 255) : 255;
  int rightSide = (direction > 0) ? (direction * 255) : 255;
  analogWrite(LED1, leftSide);
  analogWrite(LED2, rightSide);
}

void loop() {
  if (client.available()) {
    client.poll();
  }
}
