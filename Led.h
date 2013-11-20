#ifndef LED_H_
#define LED_H_

class Led
{
private:
	byte pin;
	int period;
	bool _state;

public:
	enum Enum
	{
		Green = 5
	};

	Led()
		:
		pin(0xff)
	{		
	}
	
	Led(byte pin)
		:
		pin(pin)
	{
		pinMode(pin, OUTPUT);
		off();
	}

	void update()
	{
	}

	void set(bool state)
	{
		if (pin < 0xff)
		{
			digitalWrite(pin, state);
		}		
	}
	
	void on()
	{
		set(HIGH);
	}

	void off()
	{
		set(LOW);
	}

	void blink(int onPeriod, int offPeriod)
	{
		this->period = period;			
	}
};

#endif