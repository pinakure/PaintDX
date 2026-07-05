/* --------------------------------------------------------------------------

rect.h

This file is part of 2DLib. (C) 2016 Marc St-Jacques <marc@geekchef.com>

Read COPYING for my extremely permissive and delicious licence.

------

A rectangle class.

-----------------------------------------------------------------------------*/

#pragma once

#include "point.h"
#include "size.h"
#include <vector>

class Rect
{

public:

	int left;
	int top;
	int right;
	int bottom;

	Rect();
	Rect(const Point& p, const Size& s);
	Rect(const Point &p, const Point &q);

	void Grow(int g);
	void Shrink(int s);

	void Normalize();
	Rect GetNormal() const;

	int GetWidth() const;
	int GetHeight() const;

	Point GetTopLeft() const;
	Point GetTopRight() const;
	Point GetBottomLeft() const;
	Point GetBottomRight() const;

	std::vector<Rect> SplitHorizontally(int y);
	std::vector<Rect> SplitVertically(int x);

	bool operator == (const Rect& r) const;
	bool operator < (const Rect& r) const;
	Rect& operator += (const Point& p);
	Rect operator + (const Point& p) const;

protected:

	void Limit(Size &size);
};

std::ostream& operator << (std::ostream& os, const Rect& r);
