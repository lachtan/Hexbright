#ifndef POWERLIGHT_H_
#define POWERLIGHT_H_

#define DPIN_PWR                8
#define DPIN_DRV_MODE           9
#define DPIN_DRV_EN             10

#define OFF_LEVEL -1

// http://www.candlepowerforums.com/vb/newreply.php?p=3889844

class Light
{
public:
	static void init()
	{
		pinMode(DPIN_DRV_MODE, OUTPUT);
		pinMode(DPIN_DRV_EN, OUTPUT);
		digitalWrite(DPIN_DRV_MODE, LOW);
		digitalWrite(DPIN_DRV_EN, LOW);
	}

	static void off()
	{
		digitalWrite(DPIN_PWR, LOW);
		digitalWrite(DPIN_DRV_MODE, LOW);
		digitalWrite(DPIN_DRV_EN, LOW);
	}

	static void lowMode()
	{
		digitalWrite(DPIN_DRV_MODE, LOW);
	}

	static void highMode()
	{
		digitalWrite(DPIN_DRV_MODE, LOW);
	}

	static void directDrive(bool mode, byte value)
	{
		digitalWrite(DPIN_DRV_MODE, mode);
		analogWrite(DPIN_DRV_EN, value);
	}

	// <0, 1000>
	static void setLevel(unsigned long level)
	{
		digitalWrite(DPIN_PWR, HIGH);
		if (level == 0)
		{
			// lowest possible power, but cpu still running (DPIN_PWR still high)
			digitalWrite(DPIN_DRV_MODE, LOW);
			analogWrite(DPIN_DRV_EN, 0);
		}
		else
		{
			byte value;
			if (level <= 500)
			{
				digitalWrite(DPIN_DRV_MODE, LOW);
				value = (byte)(.000000633*(level*level*level)+.000632*(level*level)+.0285*level+3.98);
			}
			else
			{
				level -= 500;
				digitalWrite(DPIN_DRV_MODE, HIGH);
				value = (byte)(.00000052*(level*level*level)+.000365*(level*level)+.108*level+44.8);
			}
			analogWrite(DPIN_DRV_EN, value);
		}
	}

	static void quickBlink(unsigned int level)
	{
	    for (int i = 0; i < 6; i++)
	    {
			setLevel(0);
			delay(50);
			setLevel(level);
			delay(50);
	    }
		setLevel(0);
	}
};

#endif
