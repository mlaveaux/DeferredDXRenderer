#ifndef GLOBAL_H
#define GLOBAL_H

struct UVector2
{
	unsigned int x;
	unsigned int y;

	UVector2(){}

	UVector2(unsigned int X, unsigned int Y)
	{
		x = X;
		y = Y;
	}

	UVector2& operator += (const UVector2& Vector)
	{
		x += Vector.x;
	    y += Vector.y;	

		return *this;
	}

	UVector2 operator + (const UVector2& Vector)
	{
		UVector2 _Vector;

		_Vector.x = x + Vector.x;
		_Vector.y = y + Vector.y;

		return _Vector;
	}
};

struct IVector2
{
	int x;
	int y;

	IVector2(){}

	IVector2(int X, int Y)
	{
		x = X;
		y = Y;
	}
};
/*
struct Exception
{
	char* Name;
	unsigned long Error;

	Exception(const char* FunctionName, unsigned long ErrorCode)
	{
		Name  = FunctionName;
		Error = ErrorCode;
	}
};*/

#endif