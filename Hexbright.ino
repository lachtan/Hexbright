#include <Wire.h>
#include <Arduino.h>

#include "PowerButton.h"
#include "Acceleration.h"
#include "PowerLight.h"
#include "ThermalSensor.h"
#include "Battery.h"

// Pin assignments
#define DPIN_GLED 5

enum Mode
{
	OffMode,
	OnMode
};

byte mode;
byte nextMode;

void setup()
{
	pinMode(DPIN_PWR, INPUT);
	digitalWrite(DPIN_PWR, LOW);

	PowerButton::init();
	Accelerometer::init();
	PowerLight::init();
	ThermalSensor::init();
	Battery::init();

	pinMode(DPIN_GLED, OUTPUT);

	Serial.begin(115200);
	Wire.begin();

	Serial.println("Powered up!");

	PowerLight::off();

	mode = OffMode;
}

PowerButton powerButton;
Accelerometer accelerometer;
Acceleration acceleration;

double lastAngle = 0;
double volume = 0;

void loop()
{
	powerButton.update();

	switch (mode)
	{
		case OffMode:
			if (powerButton.justReleased())
			{
				nextMode = OnMode;
				powerButton.resetChange();
			}
			break;

		case OnMode:
			if (powerButton.justReleased())
			{
				nextMode = OffMode;
				powerButton.resetChange();
			}
			else
			{
				controllLight();
			}
			break;
	}

	if (nextMode != mode)
	{
		switch (nextMode)
		{
			case OffMode:
				Serial.println("switch to off");
				pinMode(DPIN_PWR, INPUT);
				PowerLight::off();
				break;

			case OnMode:
				Serial.println("switch to on");
				pinMode(DPIN_PWR, OUTPUT);
				volume = 100;
				break;		
		}
		mode = nextMode;
	}

	delay(50);
}

void controllLight()
{
	accelerometer.update();
	acceleration.put(accelerometer.vector());

	double angle = 180 / PI * accelerometer.vector().roll();
	double diff = lastAngle - angle;

	if (abs(diff) < 100)
	{
		volume += diff;
		volume = volume < 0 ? 0 : volume;
		volume = volume > 1000 ? 1000 : volume;

		Serial.print("volume ");
		Serial.println(volume);

		PowerLight::setLevel(volume);
	}
	lastAngle = angle;
}
