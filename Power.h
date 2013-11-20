#ifndef POWER_H_
#define POWER_H_

#define DPIN_PWR 8

class Power
{
public:
	static void init()
	{
		// po teto akci je k dispozici nejaky cas, nez se baterka zas vypne
		pinMode(DPIN_PWR, INPUT);
		digitalWrite(DPIN_PWR, LOW);
	}

	static void on()
	{
		pinMode(DPIN_PWR, OUTPUT);
		digitalWrite(DPIN_PWR, HIGH);
	}

	static void off()
	{
		pinMode(DPIN_PWR, OUTPUT);
		digitalWrite(DPIN_PWR, LOW);
	}
};

#endif