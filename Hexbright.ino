#include <Wire.h>
#include <Arduino.h>

#include "PowerButton.h"
#include "Acceleration.h"

// Settings
#define OVERTEMP                340
// Pin assignments
#define DPIN_GLED               5
#define DPIN_PWR                8
#define DPIN_DRV_MODE           9
#define DPIN_DRV_EN             10
#define APIN_TEMP               0
#define APIN_CHARGE             3

int counter = 0;
unsigned long last = 0;

void setup()
{
	pinMode(DPIN_PWR,      INPUT);
	digitalWrite(DPIN_PWR, LOW);

	PowerButton::init();
	Accelerometer::init();

	pinMode(DPIN_GLED, OUTPUT);
	pinMode(DPIN_DRV_MODE, OUTPUT);
	pinMode(DPIN_DRV_EN, OUTPUT);
	digitalWrite(DPIN_DRV_MODE, LOW);
	digitalWrite(DPIN_DRV_EN, LOW);

	Serial.begin(115200);
	Serial.println("Powered up!");

	Serial.print(sizeof(Vector));
	Serial.println();

	//measure();
}

void measure()
{
	long a = 10;
	long b = 123;
	long c;

	unsigned long begin = micros();
	for (int i = 0; i < 10000; i++)
	{
		a += 2;
		b -= 3;
		c += a * b;
	}
	unsigned long end = micros();
	unsigned long duration = end - begin;
	
	Serial.print("Duration ");
	Serial.println(duration);
	Serial.println(c);
}

PowerButton button;
Accelerometer accelerometer;
bool lastButtonState = false;
byte tilt = 0;
Acceleration acceleration;
double roll = 0;

void loop()
{
	accelerometer.update();
	acceleration.put(accelerometer.vector());

	double _roll = acceleration.vector().roll();
	if (abs(_roll) > 0.05)
	{
		roll += _roll;

	}

	Serial.print("totalSpin=");
	Serial.print(roll);
	Serial.print(" magnitude=");
	Serial.print(acceleration.vector().magnitude());
	Serial.print(" angle=");
	Serial.print(acceleration.angleFromDown());
	Serial.print(" roll=");
	Serial.print(_roll);
	Serial.print(" down=");
	acceleration.down().print();
	Serial.print(" vector=");
	acceleration.vector().print();
	Serial.println();

	delay(100);
}

void tiltShow()
{
	if (false && tilt != accelerometer.tilt())
	{
		Serial.print("tilt=");
		Serial.print(accelerometer.tilt());
		Serial.print(" shake=");
		Serial.print(accelerometer.shaked() ? "yes" : "no");
		Serial.print(" tapped=");
		Serial.print(accelerometer.tapped() ? "yes" : "no");
		Serial.print(" orientation=");
		Serial.print(accelerometer.orientation().name());
		Serial.print(" bafro=");
		Serial.print(accelerometer.lie().name());
		Serial.println();

		tilt = accelerometer.tilt();
	}
}

void xloop()
{
	button.update();

	bool nowButtonState = button.pressed();
	if (lastButtonState != nowButtonState)
	{
		Serial.print("(");
		Serial.print(nowButtonState ? "off" : "on");
		Serial.print(" time ");
		Serial.print(button.prevDuration());
		Serial.print(") ");

		Serial.println(nowButtonState ? "ON" : "OFF");
				
		lastButtonState = nowButtonState;
	}

	if (button.isDoubleClick() && button.hasChanged())
	{
		Serial.print("DOUBLE ");
		Serial.println(millis());
		button.resetChange();
	}

	delay(10);
}
