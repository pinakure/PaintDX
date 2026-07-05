/* --------------------------------------------------------------------------

rect.h

This file is part of 2DLib. (C) 2016 Marc St-Jacques <marc@geekchef.com>

Read COPYING for my extremely permissive and delicious licence.

------

A rectangle class.

-----------------------------------------------------------------------------*/

#include "rect.h"

Rect::Rect()
{

}

Rect::Rect(const Point& p, const Size& s)
	: left(p.X)
	, top(p.Y)
	, right(p.X + s.W - 1)
	, bottom(p.Y + s.H - 1)
{

}

Rect::Rect(const Point &p, const Point &q)
	: left(p.X)
	, top(p.Y)
	, right(q.X)
	, bottom(q.Y)
{

}

void Rect::Grow(int g)
{
	left -= g;
	right += g;
	top -= g;
	bottom += g;
}

void Rect::Shrink(int s)
{
	left += s;
	right -= s;
	top += s;
	bottom -= s;
}

void Rect::Normalize()
{
	int l = left;
	left = 0;
	right -= l;

	int t = top;
	top = 0;
	bottom -= t;
}

Rect Rect::GetNormal() const
{
	Rect n = *this;
	n.Normalize();

	return n;
}

int Rect::GetWidth() const
{
	return right - left + 1;
}

int Rect::GetHeight() const
{
	return bottom - top + 1;
}

Point Rect::GetTopLeft() const
{
	return Point(left, top);
}

Point Rect::GetTopRight() const
{
	return Point(right, top);
}

Point Rect::GetBottomLeft() const
{
	return Point(left, bottom);
}

Point Rect::GetBottomRight() const
{
	return Point(right, bottom);
}

std::vector<Rect> Rect::SplitHorizontally(int y)
{
	Rect r1 = *this, r2 = *this;

	r1.bottom = r1.top + y - 1;
	r2.top += y;

	return{ r1, r2 };
}

std::vector<Rect> Rect::SplitVertically(int x)
{
	Rect r1 = *this, r2 = *this;

	r1.right = r1.left + x - 1;
	r2.left += x;

	return{ r1, r2 };
}

bool Rect::operator == (const Rect& r) const
{
	return (r.left == left && r.top == top && r.right == right && r.bottom == bottom);
}

bool Rect::operator < (const Rect& r) const
{
	return (right - left < r.right - r.left || bottom - top < r.bottom - r.top);
}

Rect& Rect::operator += (const Point& p)
{
	left += p.X;
	right += p.X;

	top += p.Y;
	bottom += p.Y;

	return *this;
}

Rect Rect::operator + (const Point& p) const
{
	Rect r = *this;
	r += p;
	return r;
}

void Rect::Limit(Size &size)
{
	if (left < 0)
		left = 0;

	if (top < 0)
		top = 0;

	if (right >= size.W)
		right = size.W - 1;

	if (bottom >= size.H)
		bottom = size.H - 1;
}

std::ostream& operator << (std::ostream &os, const Rect &r)
{
	os << "<left=" << r.left << ", top=" << r.top << ", right=" << r.right << ", bottom=" << r.bottom << ">";
	return os;
}
