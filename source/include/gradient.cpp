#include "PaintDX.hpp"
#include "gradient.hpp"
#include <allegro.h>

extern void updatePalette();
extern BITMAP *pattern[32];

ColorStop createStop(int dither, int fg, int bg, int i = 0, int size = 1) {
	ColorStop cs;
	cs.dither_pattern = dither;
	cs.color = fg;
	cs.alt_color = bg;
	cs.position = float(i) / float(size);
	return cs;
}

void Gradient::calculateStops() {

	int begin = index_start;
	int size = (index_end + 1) - index_start;

	if (!forward) begin = index_end;
	
	stops.clear();

	for (int i = 0; i < size; i++) {
		ColorStop cs;

		int fg = begin + (forward ? i : -i);
		int bg = (i < size - 1) ? begin + (forward ? (i + 1) : -(i + 1)) : begin + (forward ? i : -i);

		cs = createStop(0, fg, bg, i + 1, size);

		stops.push_back(cs);

		int w = i;

		if (spatter > 7)spatter = 7;

		// Add dithering stops
		switch (spatter) {
		default:
		case 0: continue;

		case 1: w *= 2; stops.push_back(createStop(1, fg, bg));
			continue;

		case 2: w *= 3;
			stops.push_back(createStop(2, fg, bg));
			stops.push_back(createStop(1, fg, bg));
			stops.push_back(createStop(2, bg, fg));
			continue;

		case 3:	w *= 4;
			stops.push_back(createStop(3, fg, bg));
			stops.push_back(createStop(2, fg, bg));
			stops.push_back(createStop(1, fg, bg));
			stops.push_back(createStop(2, bg, fg));
			stops.push_back(createStop(3, bg, fg));
			continue;

		case 4:	w *= 5;
			stops.push_back(createStop(4, fg, bg));
			stops.push_back(createStop(3, fg, bg));
			stops.push_back(createStop(2, fg, bg));
			stops.push_back(createStop(1, fg, bg));
			stops.push_back(createStop(2, bg, fg));
			stops.push_back(createStop(3, bg, fg));
			stops.push_back(createStop(4, bg, fg));
			continue;

		case 5:	w *= 6;
			stops.push_back(createStop(5, fg, bg));
			stops.push_back(createStop(4, fg, bg));
			stops.push_back(createStop(3, fg, bg));
			stops.push_back(createStop(2, fg, bg));
			stops.push_back(createStop(1, fg, bg));
			stops.push_back(createStop(2, bg, fg));
			stops.push_back(createStop(3, bg, fg));
			stops.push_back(createStop(4, bg, fg));
			stops.push_back(createStop(5, bg, fg));
			continue;

		case 6:	w *= 7;
			stops.push_back(createStop(6, fg, bg));
			stops.push_back(createStop(5, fg, bg));
			stops.push_back(createStop(4, fg, bg));
			stops.push_back(createStop(3, fg, bg));
			stops.push_back(createStop(2, fg, bg));
			stops.push_back(createStop(1, fg, bg));
			stops.push_back(createStop(2, bg, fg));
			stops.push_back(createStop(3, bg, fg));
			stops.push_back(createStop(4, bg, fg));
			stops.push_back(createStop(5, bg, fg));
			stops.push_back(createStop(6, bg, fg));
			continue;

		case 7:	w *= 8;
			stops.push_back(createStop(7, fg, bg));
			stops.push_back(createStop(6, fg, bg));
			stops.push_back(createStop(5, fg, bg));
			stops.push_back(createStop(4, fg, bg));
			stops.push_back(createStop(3, fg, bg));
			stops.push_back(createStop(2, fg, bg));
			stops.push_back(createStop(1, fg, bg));
			stops.push_back(createStop(2, bg, fg));
			stops.push_back(createStop(3, bg, fg));
			stops.push_back(createStop(4, bg, fg));
			stops.push_back(createStop(5, bg, fg));
			stops.push_back(createStop(6, bg, fg));
			stops.push_back(createStop(7, bg, fg));
			continue;
		}
	}

	size = stops.size();
	int i = 0;
	int color = 0;
	for (ColorStop &cs : stops) {
		cs.position = float(i + 2) / float(size + 1);
		color = cs.color;
		i++;
	}
}

void Gradient::generate(int start, int end) {
	int size = (end + 1) - start;

	int ir, ig, ib;
	int fr, fg, fb;
	PaintDX::getRGB(start, ir, ig, ib);
	PaintDX::getRGB(end, fr, fg, fb);
	float delta_r = float(fr - ir) / float(size);
	float delta_g = float(fg - ig) / float(size);
	float delta_b = float(fb - ib) / float(size);
	float r = float(ir);
	float g = float(ig);
	float b = float(ib);

	for (int i = start; i < end; i++) {
		PaintDX::setRGB(i, int(r), int(g), int(b), false);
		r += delta_r;
		g += delta_g;
		b += delta_b;
	}

	index_start = start;
	index_end = end;

	calculateStops();

	PaintDX::redraw();
	updatePalette();
}

void Gradient::draw(void *bmp, int sx, int sy, int dx, int dy) {
	BITMAP *b = (BITMAP*)bmp;
	float w = float(dx - sx);
	float h = float(dy - sy);

	drawing_mode(DRAW_MODE_SOLID, 0, 0, 0);
	int memo_x = sx;

	for (ColorStop cs : stops) {
		float x = sx + (w * cs.position);
		if (cs.dither_pattern<1)rectfill(b, memo_x, 0, x, sy + h, PaintDX::palette[cs.color]);
		else {
			rectfill(b, memo_x, 0, x, sy + h, PaintDX::palette[cs.alt_color]);
			drawing_mode(DRAW_MODE_MASKED_PATTERN, pattern[22 + cs.dither_pattern], 0, 0);
			rectfill(b, memo_x, 0, x, sy + h, PaintDX::palette[cs.color]);
			drawing_mode(DRAW_MODE_SOLID, 0, 0, 0);
		}
		memo_x = x;
	}
	if (stops.size())
		rectfill(b, memo_x, 0, sx + w, sy + h, PaintDX::palette[stops[stops.size() - 1].color]);
}

