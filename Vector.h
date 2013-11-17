#ifndef VECTOR_H_
#define VECTOR_H_

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

	Vector normalize() const
	{
		double _magnitude = magnitude();
		if (_magnitude < 0.001)
		{
			return *this;
		}
		else
		{
			return *this / _magnitude;
		}
	}

	double roll() const
	{
		return atan2(x, z);
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

	static double angle(const Vector& lhs, const Vector& rhs)
	{
		double size = lhs.magnitude() * rhs.magnitude();
		if (size < 0.0001)
		{
			return 0;
		}
		return acos(dotProduct(lhs, rhs) / size);
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

#endif
