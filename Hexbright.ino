#include <math.h>
#include <Wire.h>

#include "hexlib.h"

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
Accelerometer acc;
bool lastButtonState = false;
byte tilt = 0;

void loop()
{
	acc.update();
	acc.vector().print();
	Serial.println();

	if (false && tilt != acc.tilt())
	{		
		Serial.print("tilt=");
		Serial.print(acc.tilt());
		Serial.print(" shake=");
		Serial.print(acc.shaked() ? "yes" : "no");
		Serial.print(" tapped=");
		Serial.print(acc.tapped() ? "yes" : "no");
		Serial.print(" orientation=");
		Serial.print(acc.orientation().name());
		Serial.print(" bafro=");
		Serial.print(acc.lie().name());
		Serial.println();

		tilt = acc.tilt();
	}
	delay(1000);
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
