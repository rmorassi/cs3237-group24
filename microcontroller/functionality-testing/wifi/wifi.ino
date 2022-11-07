#include <ESP8266WiFi.h>

#define SSID "POCO F3"
#define PASSWORD "boboWifi"

void setup()
{
    Serial.begin(115200);
    Serial.println();
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
}

void loop()
{
}
