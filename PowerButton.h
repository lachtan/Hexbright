#ifndef POWERBUTTON_H_
#define POWERBUTTON_H_

#include <Arduino.h>

class PowerButton
{
	private:
	static const int LastTimeCount = 3;

	unsigned int time[LastTimeCount];
	bool lastState;
	unsigned long lastTime;
	bool _changed;

	public:
	PowerButton();
	static void init();
	void update();
	bool pressed();
	bool released();
	bool justReleased();
	bool justPressed();
	unsigned int duration();
	unsigned int prevDuration();
	bool isDoubleClick();
	void resetChange();
	bool changed();
	static bool read();
};

#define DPIN_RLED_SW 2

PowerButton::PowerButton()
{
	time[0] = 0xffff;
	time[1] = 0xffff;
	time[2] = 0xffff;
	
	lastState = false;
	lastTime = millis();
	_changed = false;
}

void PowerButton::init()
{
	pinMode(DPIN_RLED_SW, INPUT);
}

void PowerButton::update()
{
	unsigned long now = millis();

	if (now - lastTime < 10)
	{
		return;
	}

	bool actualState = read();
	if (actualState != lastState)
	{
		time[2] = time[1];
		time[1] = time[0];
		time[0] = now - lastTime;
		
		lastState = actualState;
		lastTime = now;
		_changed = true;
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

bool PowerButton::pressed()
{
	return lastState;
}

bool PowerButton::released()
{
	return !lastState;
}

bool PowerButton::justReleased()
{
	if (changed() && released())
	{
		resetChange();
		return true;
	}
	return false;
}

bool PowerButton::justPressed()
{
	if (changed() && pressed())
	{
		resetChange();
		return true;
	}
	return false;
}

unsigned int PowerButton::duration()
{
	return millis() - lastTime;
}

unsigned int PowerButton::prevDuration()
{
	return time[0];
}

void PowerButton::resetChange()
{
	_changed = false;
}

bool PowerButton::changed()
{
	return _changed;
}

#endif
