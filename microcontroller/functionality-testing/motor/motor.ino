#include "constants.h"

void setup()
{
	// Set up Serial communications
	Serial.begin(115200);
	Serial.println();

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

void loop()
{
	directionControl();
	delay(1000);
	speedControl();
	delay(1000);
}

// This function lets you control spinning direction of motors
void directionControl()
{
	// Set motors to maximum speed
	// For PWM maximum possible values are 0 to 255
	analogWrite(L_SPE, 255);
	analogWrite(R_SPE, 255);

	// Turn on motor A & B
	Serial.println("Full Speed - Forwards");
	digitalWrite(L_FOR, HIGH);
	digitalWrite(L_REV, LOW);
	digitalWrite(R_FOR, HIGH);
	digitalWrite(R_REV, LOW);
	delay(2000);

	// Now change motor directions
	Serial.println("Full Speed - Backwards");
	digitalWrite(L_FOR, LOW);
	digitalWrite(L_REV, HIGH);
	digitalWrite(R_FOR, LOW);
	digitalWrite(R_REV, HIGH);
	delay(2000);

	// Turn off motors
	Serial.println("STOP");
	digitalWrite(L_FOR, LOW);
	digitalWrite(L_REV, LOW);
	digitalWrite(R_FOR, LOW);
	digitalWrite(R_REV, LOW);
}

// This function lets you control speed of the motors
void speedControl()
{
	// Turn on motors
	digitalWrite(L_FOR, HIGH);
	digitalWrite(L_REV, LOW);
	digitalWrite(R_FOR, HIGH);
	digitalWrite(R_REV, LOW);

	// Accelerate from zero to maximum speed
	Serial.println("Accelerating - Forwards");
	for (int i = 0; i < 256; i++)
	{
		analogWrite(L_SPE, i);
		analogWrite(R_SPE, i);
		delay(20);
	}

	// Decelerate from maximum speed to zero
	Serial.println("Decelerating - Forwards");
	for (int i = 255; i >= 0; --i)
	{
		analogWrite(L_SPE, i);
		analogWrite(R_SPE, i);
		delay(20);
	}

	// Turn on motors
	digitalWrite(L_FOR, LOW);
	digitalWrite(L_REV, HIGH);
	digitalWrite(R_FOR, LOW);
	digitalWrite(R_REV, HIGH);

	// Accelerate from zero to maximum speed
	Serial.println("Accelerating - Backwards");
	for (int i = 0; i < 256; i++)
	{
		analogWrite(L_SPE, i);
		analogWrite(R_SPE, i);
		delay(20);
	}

	// Decelerate from maximum speed to zero
	Serial.println("Decelerating - Backwards");
	for (int i = 255; i >= 0; --i)
	{
		analogWrite(L_SPE, i);
		analogWrite(R_SPE, i);
		delay(20);
	}

	// Now turn off motors
	Serial.println("STOP");
	digitalWrite(L_FOR, LOW);
	digitalWrite(L_REV, LOW);
	digitalWrite(R_FOR, LOW);
	digitalWrite(R_REV, LOW);
}
