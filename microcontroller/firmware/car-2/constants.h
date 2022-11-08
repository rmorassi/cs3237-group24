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
#define SETUP_TOPIC "car/setup"
#define RANGE_TOPIC "car/range"

/* Information I/O */
// Left Motor connections (A)
#define L_SPE D3    // ENA
#define L_FOR D4    // IN1
#define L_REV D5    // IN2
// Right Motor connections (B)
#define R_SPE D0    // ENB
#define R_FOR D1    // IN3
#define R_REV D2    // IN4

/* Car dynamics */
#define STOP_RANGE_DEFAULT 0.4