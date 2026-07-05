/* --------------------------------------------------------------------------

point.h

This file is part of 2DLib. (C) 2016 Marc St-Jacques <marc@geekchef.com>

Read COPYING for my extremely permissive and delicious licence.

------

A point class.

-----------------------------------------------------------------------------*/

#include "point.h"
#include <limits>

const Point Point::Origin(0, 0);
const Point Point::OutOfBounds(-1, -1);

Point::Point()
	: X(0)
	, Y(0)
{

}

Point::Point(int x, int y)
	: X(x)
	, Y(y)
{

}

bool Point::operator == (const Point& p) const
{
	return (X == p.X && Y == p.Y);
}

bool Point::operator != (const Point& p) const
{
	return (X != p.X || Y != p.Y);
}

Point& Point::operator += (const Point& p)
{
	X += p.X;
	Y += p.Y;

	return *this;
}

Point Point::operator + (const Point& p) const
{
	Point t = *this;
	t += p;
	return t;
}

Point& Point::operator -= (const Point& p)
{
	X -= p.X;
	Y -= p.Y;

	return *this;
}

Point Point::operator - (const Point& p) const
{
	Point t = *this;
	t -= p;
	return t;
}

Point Point::MaxRight() const
{
	return Point(std::numeric_limits<int>::max(), Y);
}

Point Point::MaxDown() const
{
	return Point(X, std::numeric_limits<int>::max());
}

std::ostream & operator << (std::ostream &os, const Point &p)
{
	os << "<" << p.X << ", " << p.Y  << ">";
	return os;
}