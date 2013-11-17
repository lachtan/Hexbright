#include <Arduino.h>
#include "PowerButton.h"

#define DPIN_RLED_SW 2

PowerButton::PowerButton()
{
	time[0] = 0xffff;
	time[1] = 0xffff;
	time[2] = 0xffff;
		
	lastState = false;
	lastTime = millis();
	changed = false;
}

void PowerButton::init()
{
	pinMode(DPIN_RLED_SW, INPUT);
}

void PowerButton::update()
{
	bool actualState = read();
	if (actualState != lastState)
	{
		unsigned long now = millis();
			
		time[2] = time[1];
		time[1] = time[0];
		time[0] = now - lastTime;
			
		lastState = actualState;
		lastTime = now;
		changed = true;
	}
}

bool PowerButton::isDoubleClick()
{
	return !lastState && time[0] < 120 && time[1] < 100 && time[2] < 120;
}

bool PowerButton::read()
{
	bool value;
		
	pinMode(DPIN_RLED_SW, INPUT);
	delayMicroseconds(50);
	value = digitalRead(DPIN_RLED_SW);
	pinMode(DPIN_RLED_SW, OUTPUT);

	return value;
}

