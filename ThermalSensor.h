#ifndef THERMALSENSOR_H_
#define THERMALSENSOR_H_

#include <Arduino.h>

// http://ww1.microchip.com/downloads/en/devicedoc/21942a.pdf

#define APIN_TEMP 0
#define TEMP_OVERHEATING 340

class ThermalSensor
{
public:
	static void init()
	{		
	}

	static int read()
	{
		return analogRead(APIN_TEMP);
	}

	static double celsius()
	{
		return read() * ((40.05-0)/(275-153)) - 50;
	}

	static bool overheating()
	{
	    return read() > TEMP_OVERHEATING;
	}
};

#endif
