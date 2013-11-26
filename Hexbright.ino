#include <Wire.h>
#include <Arduino.h>

#include "Power.h"
#include "Button.h"
#include "Acceleration.h"
#include "Light.h"
#include "ThermalSensor.h"
#include "Battery.h"
#include "Led.h"

Button button;
Accelerometer accelerometer;
Acceleration acceleration;

/* ----------------------------------------------------------------------------
    mode
   ---------------------------------------------------------------------------- */

class Mode
{
protected:
	unsigned long lastTimeTemperatureCheck;
	Led greenLed;

public:
	Mode()
	{
		lastTimeTemperatureCheck = 0;
		greenLed = Led(Led::Green);
	}
	
	void run()
	{
		init();
		while (true)
		{
			update();
			if (!loop())
			{
				break;
			}
			delay(10);

		}
	}
	
	virtual void init() = 0;	

	virtual void update()
	{
		checkOverheating();
		signalizeCharging();
	}

	virtual bool loop() = 0;

	virtual void overheating()
	{}

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
		Light::setLevel(100);

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
};

/* ----------------------------------------------------------------------------
    FactoryMode
   ---------------------------------------------------------------------------- */

class FactoryMode : public Mode
{
private:
	enum Enum
	{
		Off,
		Low,
		Medium,
		High
	};

	byte mode;

public:
	virtual void init()
	{
		mode = Off;
	}
	
	virtual void update()
	{
		button.update();
	}

	virtual bool loop()
	{
		if (!(button.changed() && button.released()))
		{
			return true;
		}
		
		button.resetChanged();
		byte nextMode = (mode + 1) % 4;
		switch (nextMode)
		{
			case Off:
				Serial.println("power off");
				button.reset();
				Light::off();				
				Power::off();
				delay(100);
				return false;
				break;

			case Low:
				Power::on();
				Serial.println("light low");				
				Light::directDrive(LOW, 64);
				break;
			
			case Medium:
				Power::on();
				Serial.println("light medium");				
				Light::directDrive(LOW, 255);
				break;

			case High:
				Power::on();
				Serial.println("light high");				
				Light::directDrive(HIGH, 255);
				break;
		}
		mode = nextMode;	

		return true;
	}

	virtual void overheating()
	{
		mode = Low;
	}
};

/* ----------------------------------------------------------------------------
    RadioButtonMode
   ---------------------------------------------------------------------------- */

class RadioButtonMode : public Mode
{
private:
	enum Enum
	{
		Off,
		Tune,
		Freeze
	};

	byte mode;
	double lastAngle;
	double volume;

public:
	virtual void init()
	{
		mode = Off;
		lastAngle = 0;
		// nacti z EEPROM
		volume = 200;
	}
	
	virtual void update()
	{
		button.update();
	}

	virtual void overheating()
	{
		mode = Freeze;
		volume = 100;
	}

	virtual bool loop()
	{		
		byte nextMode = mode;

		switch (mode)
		{
			case Off:
				if (button.changed() && button.released())
				{
					Light::setLevel(volume);
					nextMode = Freeze;
				}
				break;

			case Tune:
				if (button.changed() && button.released(100))
				{
					nextMode = Freeze;
				}
				else if (button.changed() && button.doubleClicked())
				{
					nextMode = Off;
				}
				else
				{
					controllLight();
				}
				break;

			case Freeze:
				if (button.changed() && button.released(100))
				{
					nextMode = Tune;
				}
				else if (button.changed() && button.doubleClicked())
				{
					nextMode = Off;
				}
				break;
		}

		if (nextMode != mode)
		{
			button.resetChanged();
		
			switch (nextMode)
			{
				case Off:
					Serial.println("switch to off");
					Light::off();
					// uloz aktualni light level
					Power::off();
					return false;
					break;

				case Tune:
					Power::on();
					Serial.println("switch to tune");
					volume = volume < 100 ? 100 : volume;
					break;

				case Freeze:
					Power::on();
					Serial.println("switch to freeze");
					break;
			}
			mode = nextMode;
		}
		return true;
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
};


/* ----------------------------------------------------------------------------
    main
   ---------------------------------------------------------------------------- */

FactoryMode factoryMode;
RadioButtonMode radioButtonMode;
Mode* actualMode;

void setMode()
{	
	accelerometer.update();

	switch (accelerometer.orientation())
	{
		case Orientation::Up:
			Serial.println("mode radio button");
			actualMode = &radioButtonMode;
			break;
		
		default:
			Serial.println("mode factory");
			actualMode = &factoryMode;
			break;
	}
}

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

	Light::off();	
}

void loop()
{
	Power::on();
	Serial.println("select mode...");
	delay(10);
	setMode();
	Power::init();

	button.reset();
	actualMode->init();
	actualMode->run();
}

