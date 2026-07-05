/* --------------------------------------------------------------------------

size.h

This file is part of 2DLib. (C) 2016 Marc St-Jacques <marc@geekchef.com>

Read COPYING for my extremely permissive and delicious licence.

------

A size class.

-----------------------------------------------------------------------------*/

#include "size.h"

Size::Size()
	: W(0)
	, H(0)
{

}

Size::Size(int w, int h)
	: W(w)
	, H(h)
{

}

std::ostream & operator << (std::ostream &os, const Size& s)
{
	os << "<width=" << s.W << ", height=" << s.H << ">";
	return os;
}