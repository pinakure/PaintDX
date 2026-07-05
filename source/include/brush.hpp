#ifndef BRUSH_HPP
#define BRUSH_HPP

typedef enum E_BrushAngle {
	BA_90,
	BA_180,
	BA_270,
	BA_ANY
}BrushAngle;

class Brush {
public:
	bool transparency;
	int trans_color;
	int width;
	int height;
	bool enabled;
	void* original_bitmap;
	void* bitmap;
	void* trans_bitmap;

	Brush();
	
	void define(int x, int y, int dx, int dy);
	void draw(int x, int y);
	void drawMasked(int x, int y);
	void smooth(int x, int y);
	void shade(int x, int y);
	void drawTransparent(int x, int y);
	void preview(int x, int y, bool transparent);
	void erase(int x, int y);
	void half(bool horz, bool vert);
	void twice(bool horz, bool vert);
	void stretch(bool horz, bool vert);
	void rotate(BrushAngle angletype);
	void outline();
	void flip(bool horz);
	void bend(bool horz);
	void shear(bool horz);
	void save();
	void load();
	void dispose();
	void restore();

	~Brush();
};

#endif

