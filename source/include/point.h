/* --------------------------------------------------------------------------

point.h

This file is part of 2DLib. (C) 2016 Marc St-Jacques <marc@geekchef.com>

Read COPYING for my extremely permissive and delicious licence.

------

A point class.

-----------------------------------------------------------------------------*/
#pragma once

#include <ostream>

class Point
{
public:

	int X, Y;

	Point();
	Point(int x, int y);

	bool operator == (const Point& p) const;
	bool operator != (const Point& p) const;
	Point & operator += (const Point& p);
	Point operator + (const Point& p) const;
	Point & operator -= (const Point& p);
	Point operator - (const Point& p) const;

	Point MaxRight() const;
	Point MaxDown() const;

	static const Point Origin;
	static const Point OutOfBounds;
};

std::ostream & operator << (std::ostream &os, const Point &p);
