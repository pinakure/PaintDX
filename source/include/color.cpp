/* --------------------------------------------------------------------------

color.cpp

This file is part of 2DLib. (C) 2016 Marc St-Jacques <marc@geekchef.com>

Read COPYING for my extremely permissive and delicious licence.

------

A Color data type based on glm with constants.

-----------------------------------------------------------------------------*/
#include "color.h"

void RGBA::FromBGRA(Color &dst, const unsigned char *src, size_t size)
{
	dst.b = (float)src[0] / 255.f;
	dst.g = (float)src[1] / 255.f;
	dst.r = (float)src[2] / 255.f;
	dst.a = (size == 4) ? (float)src[3] / 255.f : 0.f;
}

void RGBA::FromRGBA(Color &dst, const unsigned char *src, size_t size)
{
	dst.r = (float)src[0] / 255.f;
	dst.g = (float)src[1] / 255.f;
	dst.b = (float)src[2] / 255.f;
	dst.a = (size == 4) ? (float)src[3] / 255.f : 0.f;
}

void RGBA::ToBGRA(unsigned char *dst, size_t size, const Color& src)
{
	dst[0] = (unsigned char)(src.b * 255.f);
	dst[1] = (unsigned char)(src.g * 255.f);
	dst[2] = (unsigned char)(src.r * 255.f);

	if (size == 4)
		dst[3] = (unsigned char)(src.a * 255.f);
}

void RGBA::ToRGBA(unsigned char *dst, size_t size, const Color& src)
{
	dst[0] = (unsigned char)(src.r * 255.f);
	dst[1] = (unsigned char)(src.g * 255.f);
	dst[2] = (unsigned char)(src.b * 255.f);

	if (size == 4)
		dst[3] = (unsigned char)(src.a * 255.f);
}

std::ostream & operator << (std::ostream &os, const Color &c)
{
	os << "<r=" << c.r << ", g=" << c.g << ", b=" << c.b << ", a=" << c.a << ">";
	return os;
}
