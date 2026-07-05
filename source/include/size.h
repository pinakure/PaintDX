/* --------------------------------------------------------------------------

size.h

This file is part of 2DLib. (C) 2016 Marc St-Jacques <marc@geekchef.com>

Read COPYING for my extremely permissive and delicious licence.

------

A size class.  

-----------------------------------------------------------------------------*/

#pragma once

#include <iostream>

class Size
{
public:

	int W, H;

	Size();
	Size(int w, int h);
};

std::ostream & operator << (std::ostream &os, const Size& s);

