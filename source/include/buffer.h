/* --------------------------------------------------------------------------

buffer.h

This file is part of 2DLib. (C) 2016 Marc St-Jacques <marc@geekchef.com>

Read COPYING for my extremely permissive and delicious licence.

------

Buffer class in which we manipulate the color data.

-----------------------------------------------------------------------------*/

#pragma once

#include <string>
#include <vector>
#include "color.h"
#include "rect.h"
#include <string>

class PNG_Exception
{
	std::string strError;

public:

	PNG_Exception(const std::string &fn, const std::string &err)
	{
		char buffer[1024];
		sprintf_s(buffer, err.c_str(), fn.c_str());
		strError = buffer;
	}

	~PNG_Exception() { }
	std::string GetError() { return strError; }
};


class Buffer
{
protected:

	std::vector<Color> colors;
	Size size;

	void LimitPoint(Point &p);
	void LimitRect(Rect &r);

	bool LoadFromTGA(const std::string &filename);
	bool SaveAsTGA(const std::string &filename, bool with_alpha);
	bool LoadFromPNG(const std::string &filename);
	bool SaveAsPNG(const std::string &filename, bool with_alpha);

public:

	enum ScanDirection { HORZ, VERT };
	enum ScanState { MUST_FIND, MUST_ONLY_FIND };

	Buffer();
	Buffer(const Size& s, const Color& c);
	void Reset(const Size& s, const Color& c);
	void Reset(const Color& c);
	bool Save(const std::string &filename, bool with_alpha = true);
	bool Load(const std::string &filename, bool with_alpha = true);

	void Sanitize();

	void Set(const Point &p, const Color& c);
	const Color& Get(const Point &p) const;

	void DrawHorizontalLine(const Point &p, const Point &q, const Color& c);
	void DrawVerticalLine(const Point& start, const Point& end, const Color& c);

	void DrawRect(const Rect& r, const Color &c);
	void FillRect(const Rect& r, const Color& c);
	bool Scan(const Point &start, const Point &end, ScanDirection dir, ScanState s, const Color &c, Point& hit);
	Rect IsolateRect(const Rect& r, const Color& avoid);
	bool IsRectEmpty(const Rect& r, const Color& empty = RGBA::NoAlpha);

	void CopyLineFromBuffer(int dst, int src, int size, const Buffer& from);
	void CopyRectFromBuffer(const Rect& dst, const Rect& src, const Buffer& from);

	inline Size GetSize() const
	{
		return size;
	}

	void FullAlpha(const Color& c, float t = 1.f);

	void GetData(std::vector<unsigned char> &data, size_t size) const;

	void Grayscale();

	Color Average();
};
