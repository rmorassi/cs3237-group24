/* Information for wireless network */
#define SSID "POCO F3"
#define PASSWORD "boboWifi"

/* Information for MQTT communications */
#define BROKER "some.ip.address"
#define PORT 1883
// Topics
#define DIRECTION_TOPIC "car/direction"
#define SPEED_TOPIC "car/speed"
#define CONTROL_TOPIC "car/control"
#define SETUP_TOPIC "remote/setup"

/* Information I/O */
// Pins on WeMos
#define DIR_BTN D4  // manual direction control toggle
#define CTRL_BTN D5 // start/stop toggle
#define SPD_BTN D6  // speed control send
#define LED D7      // for displaying when car is on (started)
// Pins on ADS1115
#include <ADS1115_WE.h>
#define JOYSTICK_X ADS1115_COMP_0_GND // Speed control
#define JOYSTICK_Y ADS1115_COMP_1_GND // Direction control
// Debouncing
#define DEBOUCE_TIME 200

// Connect ADS1115 pins GND, VDD, SCL, SDA to respective pins on WeMos
// Connect Joystick pins GND, +5v (to +3v), Vrx, Vry
