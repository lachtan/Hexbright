#ifndef POWERBUTTON_H_
#define POWERBUTTON_H_

class PowerButton
{
	private:
	static const int LastTimeCount = 3;

	unsigned int time[LastTimeCount];
	bool lastState;
	unsigned long lastTime;
	bool changed;

	public:
	PowerButton();
	static void init();
	void update();
	inline bool pressed();
	inline bool released();
	inline unsigned int duration();
	inline unsigned int prevDuration();
	bool isDoubleClick();
	inline void resetChange();
	inline bool hasChanged();
	static bool read();
};

bool PowerButton::pressed()
{
	return lastState;
}

bool PowerButton::released()
{
	return !lastState;
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
	changed = false;
}

bool PowerButton::hasChanged()
{
	return changed;
}

#endif
