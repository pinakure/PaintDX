#ifndef GRADIENT_HPP
#define GRADIENT_HPP

#include "enum.h"

#include <vector>

class Gradient {
public:
	int							spatter;
	int							cycle_speed;
	bool						forward;
	int							index_start;
	int							index_end;
	std::vector<ColorStop>		stops;

	void preset() {
		spatter = 0;
		cycle_speed = 0;
		forward = true;
		index_start = 0;
		index_end = 0;
		stops.clear();
	}

	Gradient() {
		preset();
	}

	Gradient(const Gradient &o) {
		preset();
		spatter = o.spatter;
		cycle_speed = o.cycle_speed;
		forward = o.forward;
		index_start = o.index_start;
		index_end = o.index_end;
		calculateStops();
	}

	Gradient(int begincolor, int endcolor, int speed = 0, bool backwards = false, int spat = 0) {
		preset();
		spatter = spat;
		forward = !backwards;
		cycle_speed = speed;
		index_start = begincolor;
		index_end = endcolor;
		calculateStops();
	}

	void draw(void *bmp, int sx, int sy, int dx, int dy);
	void calculateStops();
	void generate(int start, int end);
};

#endif