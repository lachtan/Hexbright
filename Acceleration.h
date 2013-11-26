#ifndef ACCELERATION_H_
#define ACCELERATION_H_

#include <Arduino.h>
#include <math.h>
#include <Wire.h>
#include "Vector.h"

#define DPIN_ACC_INT 3
#define ACC_ADDRESS 0x4C
#define ACC_REG_XOUT 0
#define ACC_REG_YOUT 1
#define ACC_REG_ZOUT 2
#define ACC_REG_TILT 3
#define ACC_REG_INTS 6
#define ACC_REG_MODE 7

/* ----------------------------------------------------------------------------
    Orientation
   ---------------------------------------------------------------------------- */

class Orientation
{
	private:
	byte value;

	public:
	enum Enum
	{
		Unknwon = 0, Left, Right, Down, Up, Back, Front
	};

	Orientation(Enum value)
	{
		this->value = value;
	}

	bool horizontal() const
	{
		return value == Left || value == Right;
	}

	bool vertical() const
	{
		return value == Up || value == Down;
	}

	operator byte () const
	{
		return value;
	}

	const char* name() const
	{
		switch (value)
		{
			case Left:
			return "left";
			
			case Right:
			return "right";
			
			case Down:
			return "down";
			
			case Up:
			return "up";
			
			case Back:
			return "back";

			case Front:
			return "front";

			default:
			return "unknown";
		}
	}
};


/* ----------------------------------------------------------------------------
    Accelerometer
   ---------------------------------------------------------------------------- */

class Accelerometer
{
private:
	Vector _vector;
	byte _tilt;

public:
	Accelerometer()
	:
	_tilt(0)
	{}

	static void init()
	{
		// Configure accelerometer
		byte config[] = {
			ACC_REG_INTS,  // First register (see next line)
			0xE4,  // Interrupts: shakes, taps
			0x00,  // Mode: not enabled yet
			0x00,  // Sample rate: 120 Hz
			0x0F,  // Tap threshold
			0x10   // Tap debounce samples
		};
		Wire.beginTransmission(ACC_ADDRESS);
		Wire.write(config, sizeof(config));
		Wire.endTransmission();

		// Enable accelerometer
		byte enable[] = {ACC_REG_MODE, 0x01};  // Mode: active!
		Wire.beginTransmission(ACC_ADDRESS);
		Wire.write(enable, sizeof(enable));
		Wire.endTransmission();
	}

	void update()
	{
		VectorUnits values[3];

		while (true)
		{
			Wire.beginTransmission(ACC_ADDRESS);
			Wire.write(ACC_REG_XOUT);
			Wire.endTransmission(false);
			Wire.requestFrom(ACC_ADDRESS, 4);

			for (byte i = 0; i < 4; i++)
			{
				if (!Wire.available())
				{
					continue;
				}

				char value = Wire.read();
				
				if (value & 0x40)
				{
					// Indicates failed read; redo!
					continue;
				}
				if (i == 3)
				{
					_tilt = value;
				}
				else
				{
					if (value & 0x20)
					{
						// Sign-extend
						value |= 0xC0;
					}
					values[i] = value / 21.3;
				}
			}
			break;
		}
		
		_vector = Vector(values);
	}

	byte tilt() const
	{
		return _tilt;
	}

	bool tapped() const
	{
		return _tilt & 0x20;
	}

	bool shaked() const
	{
		return _tilt & 0x80;
	}

	Orientation lie() const
	{
		if (_tilt & 0x01)
		{
			return Orientation::Back;
		}
		else if (_tilt & 0x02)
		{
			return Orientation::Front;
		}
		else
		{
			return Orientation::Unknwon;
		}
	}

	Orientation orientation() const
	{
		byte pola = (_tilt >> 2) & 7;
		switch (pola)
		{
			case 1:
				return Orientation::Up;

			case 2:
				return Orientation::Down;

			case 5:
				return Orientation::Right;

			case 6:
				return Orientation::Left;

			default:
				return Orientation::Unknwon;
		}
	}

	const Vector& vector() const
	{
		return _vector;
	}
};


/* ----------------------------------------------------------------------------
    Acceleration
   ---------------------------------------------------------------------------- */

#define AccelerationMaxHistory 4

class Acceleration
{
private:
	Vector _down;
	Vector vectors[AccelerationMaxHistory];
	char firstVectorIndex;

public:
	Acceleration()
		:
		firstVectorIndex(0)
	{
	}
	
	void put(const Vector& vector)
	{
		shiftFirst();
		vectors[firstVectorIndex] = vector;
		findDown();
	}

	const Vector& vector() const
	{
		return vectors[firstVectorIndex];
	}

	const Vector& history(byte index) const
	{
		return vectors[(firstVectorIndex + index) % AccelerationMaxHistory];
	}

	const Vector& down() const
	{
		return _down;
	}

	double angleFromDown() const
	{
		Vector lightAxis(0, -1, 0);
		return Vector::angle(lightAxis, down()) * 180 / 3.14159;
	}

private:
	void findDown()
	{
		Vector down;

		for (byte i = 0; i < AccelerationMaxHistory; i++)
		{
			down += history(i);
		}
		_down = down.normalize();
	}

	void shiftFirst()
	{
		firstVectorIndex--;
		if (firstVectorIndex < 0)
		{
			firstVectorIndex = AccelerationMaxHistory - 1;
		}
	}
};

#endif