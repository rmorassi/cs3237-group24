#include "constants.h"

void setup() {
	// Set all the motor control pins to outputs
	pinMode(L_SPE, OUTPUT);
	pinMode(R_SPE, OUTPUT);
	pinMode(L_FOR, OUTPUT);
	pinMode(L_REV, OUTPUT);
	pinMode(R_FOR, OUTPUT);
	pinMode(R_REV, OUTPUT);
	
	// Turn off motors - Initial state
	digitalWrite(L_FOR, LOW);
	digitalWrite(L_REV, LOW);
	digitalWrite(R_FOR, LOW);
	digitalWrite(R_REV, LOW);
}

void loop() {
	directionControl();
	delay(1000);
	speedControl();
	delay(1000);
}

// This function lets you control spinning direction of motors
void directionControl() {
	// Set motors to maximum speed
	// For PWM maximum possible values are 0 to 255
	analogWrite(L_SPE, 255);
	analogWrite(R_SPE, 255);

	// Turn on motor A & B
	digitalWrite(L_FOR, HIGH);
	digitalWrite(L_REV, LOW);
	digitalWrite(R_FOR, HIGH);
	digitalWrite(R_REV, LOW);
	delay(2000);
	
	// Now change motor directions
	digitalWrite(L_FOR, LOW);
	digitalWrite(L_REV, HIGH);
	digitalWrite(R_FOR, LOW);
	digitalWrite(R_REV, HIGH);
	delay(2000);
	
	// Turn off motors
	digitalWrite(L_FOR, LOW);
	digitalWrite(L_REV, LOW);
	digitalWrite(R_FOR, LOW);
	digitalWrite(R_REV, LOW);
}

// This function lets you control speed of the motors
void speedControl() {
	// Turn on motors
	digitalWrite(L_FOR, LOW);
	digitalWrite(L_REV, HIGH);
	digitalWrite(R_FOR, LOW);
	digitalWrite(R_REV, HIGH);
	
	// Accelerate from zero to maximum speed
	for (int i = 0; i < 256; i++) {
		analogWrite(L_SPE, i);
		analogWrite(R_SPE, i);
		delay(20);
	}
	
	// Decelerate from maximum speed to zero
	for (int i = 255; i >= 0; --i) {
		analogWrite(L_SPE, i);
		analogWrite(R_SPE, i);
		delay(20);
	}
	
	// Now turn off motors
	digitalWrite(L_FOR, LOW);
	digitalWrite(L_REV, LOW);
	digitalWrite(R_FOR, LOW);
	digitalWrite(R_REV, LOW);
}