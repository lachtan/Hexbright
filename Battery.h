#ifndef VOLTAGE_H_
#define VOLTAGE_H_

#define APIN_TEMP 0
#define APIN_CHARGE 3
#define APIN_BAND_GAP 14
#define OVERTEMP 340

class Battery
{
private:
	int value;

public:
	static void init()
	{
	}

	static int read()
	{
		return analogRead(APIN_CHARGE);
	}

	double volts()
	{
		// napeti ceho??
		return 1.1 * 1023 * analogRead(APIN_BAND_GAP);
	}

	static bool charging()
	{
		return read() < 128;
	}

	static bool charged()
	{
		return read() > 768;
	}

	static bool onBattery()
	{
		return !(charging() || charged());
	}
};

#endif
