#include <Wire.h>
#include <Arduino.h>

#include "Power.h"
#include "Button.h"
#include "Acceleration.h"
#include "Light.h"
#include "ThermalSensor.h"
#include "Battery.h"
#include "Led.h"

class Mode
{
public:
	enum Enum
	{
		Off,
		Tune,
		Freeze
	};
};

byte mode;
byte nextMode;

Button button;
Accelerometer accelerometer;
Acceleration acceleration;

double lastAngle;
double volume;
unsigned long lastTimeTemperatureCheck;

Led greenLed;

void setup()
{
	Power::init();
	Button::init();
	Accelerometer::init();
	Light::init();
	ThermalSensor::init();
	Battery::init();	

	Serial.begin(115200);
	Wire.begin();

	Serial.println("Powered up!");

	Light::off();

	mode = Mode::Off;
	lastAngle = 0;
	volume = 200;

	greenLed = Led(Led::Green);
}

void loop()
{
	button.update();
	checkOverheating();
	signalizeCharging();

	switch (mode)
	{
		case Mode::Off:
			if (button.changed() && button.released())
			{				
				Light::setLevel(volume);
				nextMode = Mode::Freeze;
			}
			break;

		case Mode::Tune:
			if (button.changed() && button.released(100))
			{
				nextMode = Mode::Freeze;
			}
			else if (button.changed() && button.doubleClicked())
			{
				nextMode = Mode::Off;
			}
			else
			{
				controllLight();
			}
			break;

		case Mode::Freeze:
			if (button.changed() && button.released(100))
			{
				nextMode = Mode::Tune;
			}
			else if (button.changed() && button.doubleClicked())
			{
				nextMode = Mode::Off;
			}
			break;
	}

	if (nextMode != mode)
	{
		button.resetChanged();
		pinMode(DPIN_PWR, OUTPUT); // proc???
		
		switch (nextMode)
		{
			case Mode::Off:
				Serial.println("switch to off");
				Light::off();
				Power::off();
				break;

			case Mode::Tune:
				Power::on();
				Serial.println("switch to tune");
				volume = volume < 100 ? 100 : volume;
				break;

			case Mode::Freeze:
				Power::on();
				Serial.println("switch to freeze");
				break;
		}
		mode = nextMode;
	}

	delay(10);
}

void checkOverheating()
{
	unsigned long now = millis();

	if (now - lastTimeTemperatureCheck < 1000)
	{
		return;
	}

	if (!ThermalSensor::overheating())
	{
		return;
	}

	Serial.println("overheating");
	Light::quickBlink(400);
	volume = 100;
	Light::setLevel(volume);
	mode = Mode::Freeze;
	nextMode = mode;

	lastTimeTemperatureCheck = now;
}

void signalizeCharging()
{
	if (Battery::charging())
	{
		greenLed.blink(50, 500);
	}
	else
	{
		greenLed.off	();
	}
}

void controllLight()
{
	accelerometer.update();
	acceleration.put(accelerometer.vector());

	double angle = 180 / PI * accelerometer.vector().roll();
	double diff = lastAngle - angle;

	if (diff > 180)
	{
		diff -= 360;
	}
	if (diff < -180)
	{
		diff += 360;
	}
	
	volume += diff;
	volume = volume < 0 ? 0 : volume;
	volume = volume > 1000 ? 1000 : volume;

	Serial.print("volume ");
	Serial.println(volume);

	Light::setLevel(volume);

	lastAngle = angle;
}
