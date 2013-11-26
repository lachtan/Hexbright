#ifndef POWERBUTTON_H_
#define POWERBUTTON_H_

#include <Arduino.h>

#define DPIN_RLED_SW 2

class Button
{
private:
	static const int LastTimeCount = 3;

	unsigned int time[LastTimeCount];
	bool lastState;
	unsigned long lastTime;
	bool _changed;

public:
	Button()
	{
		reset();
	}

	static void init()
	{
		pinMode(DPIN_RLED_SW, INPUT);
	}

	void reset()
	{
		time[0] = 0xffff;
		time[1] = 0xffff;
		time[2] = 0xffff;
		
		lastState = false;
		lastTime = millis();
		_changed = false;
	}

	void update()
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
			_changed = true;
		}
		else
		{
			// predpokladam ze si kazdy vycte hodnotu okamzite po update()
			// hm ale co ruzne casy...
			
			//_changed = false;
		}
	}

	bool doubleClicked()
	{
		return !lastState && time[0] < 120 && time[1] < 100 && time[2] < 120;
	}

	bool read()
	{
		bool value;
	
		pinMode(DPIN_RLED_SW, INPUT);
		delayMicroseconds(50);
		value = digitalRead(DPIN_RLED_SW);
		pinMode(DPIN_RLED_SW, OUTPUT);

		return value;
	}

	bool changed()
	{
		return _changed;
	}

	void resetChanged()
	{
		_changed = false;
	}

	bool pressed()
	{
		return lastState;
	}

	bool pressed(int minDuration)
	{
		return pressed() && duration() >= minDuration;
	}

	bool released()
	{
		return !lastState;
	}

	bool released(int minDuration)
	{
		return released() && duration() >= minDuration;
	}

	unsigned int duration()
	{
		return millis() - lastTime;
	}

	unsigned int prevDuration()
	{
		return time[0];
	}
};

#endif
