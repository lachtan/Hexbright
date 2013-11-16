#ifndef HEXLIB_H_
#define HEXLIB_H_

#include <Arduino.h>

// Settings
#define OVERTEMP                340
// Pin assignments
#define DPIN_RLED_SW            2
#define DPIN_GLED               5
#define DPIN_PWR                8
#define DPIN_DRV_MODE           9
#define DPIN_DRV_EN             10
#define APIN_TEMP               0
#define APIN_CHARGE             3
// Modes
#define MODE_OFF                0
#define MODE_LOW                1
#define MODE_MED                2
#define MODE_HIGH               3
#define MODE_BLINKING           4
#define MODE_BLINKING_PREVIEW   5


// -----------------------------------------------------------------------------
// PowerButton
// -----------------------------------------------------------------------------

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


// -----------------------------------------------------------------------------
// Accelerometer
// -----------------------------------------------------------------------------

#include <math.h>
#include <Wire.h>

#define DPIN_ACC_INT 3

#define ACC_ADDRESS             0x4C
#define ACC_REG_XOUT            0
#define ACC_REG_YOUT            1
#define ACC_REG_ZOUT            2
#define ACC_REG_TILT            3
#define ACC_REG_INTS            6
#define ACC_REG_MODE            7

// return values for get_tilt_orientation
#define TILT_UNKNOWN 0
#define TILT_UP 1
#define TILT_DOWN 2
#define TILT_HORIZONTAL 3

#define VectorUnits double

class Vector
{
public:
	VectorUnits x;
	VectorUnits y;
	VectorUnits z;

	Vector()
		:
		x(0),
		y(0),
		z(0)
	{}

	Vector(const VectorUnits* values)
	{
		x = *(values++);
		y = *(values++);
		z = *values;
	}

	Vector(VectorUnits x, VectorUnits y, VectorUnits z)
		:
		x(x),
		y(y),
		z(z)
	{}

	const VectorUnits& operator[] (byte index) const
	{
		return *(&x + index);
	}

	Vector operator+ (const Vector& other) const
	{
		return Vector(x + other.x, y + other.y, z + other.z);
	}

	Vector& operator+= (const Vector& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	Vector operator- (const Vector& other) const
	{
		return Vector(x - other.x, y - other.y, z - other.z);
	}

	Vector operator* (double multiplier) const
	{
		return Vector(x * multiplier, y * multiplier, z * multiplier);
	}

	Vector operator/ (double divisor) const
	{
		return Vector(x / divisor, y / divisor, z / divisor);
	}

	double magnitude() const
	{
		double result = 0;
		for (byte i = 0; i < 3; i++)
		{
			result += (*this)[i] * (*this)[i];
		}
		return sqrt(result);
	}

	Vector normalize(const Vector& vector) const
	{
		return vector / vector.magnitude();
	}

	static VectorUnits dotProduct(const Vector& lhs, const Vector& rhs)
	{
		VectorUnits sum = 0;
		for (byte i = 0; i < 3; i++)
		{
			sum += lhs[i] * rhs[i];
		}
		return sum;
	}

	static Vector crossProduct(const Vector& lhs, const Vector& rhs)
	{
		return Vector(
			lhs.y * rhs.z - lhs.z * rhs.y,
			lhs.z * rhs.x - lhs.x * rhs.z,
			lhs.x * rhs.y - lhs.y * rhs.x
		);
	}

	static double spin(const Vector& down, const Vector& vector)
	{
		return atan2(down.x, down.z) - atan2(vector.x, vector.z);
	}

	static double differenceFromDown(const Vector& down)
	{
		Vector lightAxis(0, -1, 0);
		return angleDifference(dotProduct(lightAxis, down), 1.0, 1.0);
	}

	static double angleDifference(double dotProduct, double magnitude1, double magnitude2)
	{
		double tmp = dotProduct / (magnitude1 * magnitude2);
		return acos(tmp) / 3.14159;
	}

	void print() const
	{
		Serial.print("(");
		Serial.print(x);
		Serial.print(",");
		Serial.print(y);
		Serial.print(",");
		Serial.print(z);
		Serial.print(")");
	}
};

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

class Acceleration
{
private:
	Vector down;
	Vector vectors[4];
	byte first;

public:
	void put(const Vector& vector)
	{
		moveFirst();
		(*this)[0] = vector;
		findDown();
	}

private:
	void findDown()
	{
		down = (*this)[0];
		double magnitudes = down.magnitude();

		for (byte i = 1; i < 4; i++)
		{
			const Vector& vector = (*this)[i];
			down += vector;
			magnitudes += vector.magnitude();
		}
		if (magnitudes == 0)
		{
			magnitudes = 1;
		}
		down = down / magnitudes;
	}

	Vector& operator[] (byte index)
	{
		return vectors[(first + index) % 4];
	}

	void moveFirst()
	{
		first--;
		if (first < 0)
		{
			first = 3;
		}
	}
};

#endif
