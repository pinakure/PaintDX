#include "brush.hpp"
#include <allegro.h>

extern BITMAP *picture;
extern BITMAP *ovram;
extern int pasive_color;
extern void GPURemap(BITMAP *bmp, int sx, int sy, int dx, int dy);
extern void GPUReplaceColor(BITMAP *bmp, int o, int r);

Brush::Brush() {
	enabled = false;
	width = 0;
	height = 0;
	bitmap = nullptr;
	trans_bitmap = nullptr;
	trans_color = pasive_color;
	transparency = false;
}

void Brush::preview(int x, int y, bool transparent) {
	if (!enabled)return;
	blit((BITMAP*)bitmap,(BITMAP*)trans_bitmap, 0, 0, 0, 0, width, height);
	GPUReplaceColor((BITMAP*)trans_bitmap, pasive_color, makecol(255, 0, 255));
	masked_blit((BITMAP*)trans_bitmap, ovram, 0, 0, x, y, width, height);
	GPURemap((BITMAP*)ovram, x, y, x + width, y + height);
}

void Brush::erase(int x, int y) {
	rectfill(picture, x, y, x+width, y+height, pasive_color);
}

void Brush::draw(int x, int y) {
	blit((BITMAP*)bitmap, picture, 0, 0, x, y, width, height);
}

void Brush::drawMasked(int x, int y) {
	//draw with active_color where brush color is not pasive_color
}

void Brush::smooth(int x, int y) {
	// Smooth points where brush color is not pasive color
}
void Brush::shade(int x, int y) {
	// Shade points where brush color is not pasive color
}

void Brush::drawTransparent(int x, int y) {
	if (!enabled)return;
	blit((BITMAP*)bitmap, (BITMAP*)trans_bitmap, 0, 0, 0, 0, width, height);
	GPUReplaceColor((BITMAP*)trans_bitmap, pasive_color, makecol(255, 0, 255));
	masked_blit((BITMAP*)trans_bitmap, picture, 0, 0, x, y, width, height);
}

void Brush::define(int x, int y, int dx, int dy) {
	if (bitmap)dispose();
	width = dx - x;
	height = dy - y;
	bitmap = create_bitmap(width, height);
	trans_bitmap = create_bitmap(width, height);
	blit(picture, (BITMAP*)bitmap, x, y, 0, 0, width, height);
	enabled = true;
}

static void panic() {
	allegro_message("ERROR: Out of memory!");
	exit(-1);
}

void Brush::half(bool horz, bool vert) {
	if (!bitmap)return;
	int nwidth = horz ? width / 2 : width;
	int nheight = vert ? height / 2 : height;
	BITMAP *n = create_bitmap(nwidth, nheight);
	if (!n)panic();
	stretch_blit((BITMAP*)bitmap, n, 0, 0, width, height, 0, 0, nwidth, nheight);
	destroy_bitmap((BITMAP*)bitmap);
	destroy_bitmap((BITMAP*)trans_bitmap);
	bitmap = n;
	trans_bitmap = create_bitmap(n->w, n->h);
	if (!trans_bitmap)panic();
	width = nwidth;
	height = nheight;
}

void Brush::twice(bool horz, bool vert) {
	if (!bitmap)return;
	int nwidth = horz ? width * 2 : width;
	int nheight = vert ? height * 2 : height;
	BITMAP *n = create_bitmap(nwidth, nheight);
	if (!n)panic();
	stretch_blit((BITMAP*)bitmap, n, 0, 0, width, height, 0, 0, nwidth, nheight);
	destroy_bitmap((BITMAP*)bitmap);
	destroy_bitmap((BITMAP*)trans_bitmap);
	bitmap = n;
	trans_bitmap = create_bitmap(n->w, n->h);
	if (!trans_bitmap)panic();
	width = nwidth;
	height = nheight;
}

void Brush::rotate(BrushAngle angletype) {
	if (!bitmap)return;

	int sw;
	switch (angletype) {
		case BA_90:		sw = width; width = height; height = sw; break;
		case BA_270:	sw = width; width = height; height = sw; break;
		default:
		case BA_ANY:
			break;
	};

	BITMAP *n = create_bitmap(width, height);
	if (!n)panic();
	
	switch (angletype) {
		case BA_90:		rotate_sprite(n, (BITMAP*)bitmap, width / 2, height / 2, itofix(90)); break;
		case BA_180:	rotate_sprite(n, (BITMAP*)bitmap, width / 2, height / 2, itofix(180)); break;
		case BA_270:	rotate_sprite(n, (BITMAP*)bitmap, width / 2, height / 2, itofix(270)); break;
		case BA_ANY:
			// TODO: open arbitrary rotation dialog
			break;
	}
	destroy_bitmap((BITMAP*)bitmap);
	destroy_bitmap((BITMAP*)trans_bitmap);
	bitmap = n;
	trans_bitmap = create_bitmap(n->w, n->h);
	if (!trans_bitmap)panic();
}

void Brush::flip(bool horz) {
	if (!bitmap)return;
	BITMAP *n = create_bitmap(width, height);
	if (!n)panic();
	if(horz)draw_sprite_h_flip(n, (BITMAP*)bitmap, 0, 0);
	else draw_sprite_v_flip(n, (BITMAP*)bitmap, 0, 0);
	destroy_bitmap((BITMAP*)bitmap);
	destroy_bitmap((BITMAP*)trans_bitmap);
	bitmap = n;
	trans_bitmap = create_bitmap(n->w, n->h);
	if (!trans_bitmap)panic(); 
}

// These methods require mouse interactivity
void Brush::bend(bool horz) {
// horz = true:
//      ___         ____
//		| |  =>    /  /
//		---		  ----
// horz = false:
//      ___       --_
//		| |  =>   |  |
//		---		  --_|
}

void Brush::shear(bool horz) {
// horz = true:
//      ___       ___
//		| |  =>  (  (
//		---		  ---
// horz = false:
//      ___      _-_
//		| |  =>  |-|
//		---		 _-_
}

void Brush::stretch(bool horz, bool vert) {

}

// These methods require both load and save brush dialogs
void Brush::save() {

}

void Brush::load() {

}

void Brush::restore() {
	// not implemented, use original_bitmap attribute to implement this as is it yet disponible
}

// This method requires a MarchingSquares algorithm implementation
void Brush::outline() {

}

void Brush::dispose() {
	if (!enabled)return;
	enabled = false;
	width = 0;
	height = 0;
	if (bitmap)destroy_bitmap((BITMAP*)bitmap);
	if (trans_bitmap)destroy_bitmap((BITMAP*)trans_bitmap);
	bitmap = nullptr;
	trans_bitmap = nullptr;
}

Brush::~Brush() {
	//dispose();
}

