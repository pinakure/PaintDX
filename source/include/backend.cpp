#include <list>
#include "paintdx.hpp"
#include <allegro.h>
#include <winalleg.h>
#include <vector>
#include <deque>
#include "enum.h"
#include <string>
#include "tools.hpp"

BITMAP *vram = nullptr;
BITMAP *picture = nullptr;

int VWIDTH = 320;
int VHEIGHT = 240;

class DXPainting {
public:
	Gradient gradients[16];
	std::string name;
	int palette[256];
	int width;
	int height;
	int current_frame;
	std::vector<BITMAP*> frames;
	std::vector<int> speeds;
	DXPainting();
	void selectFrame(int index);
	void updateFrame();
	void updatePicture();
	void updatePalette();
	void updateGradients();
	void addFrame(int frame_speed);
	void copyFrame(BITMAP *picture_to_copy, int frame_speed);
	void deleteFrame(int index);
	void create(int w, int h, int frame_count, int default_speed);
	bool load(std::string filename);
	bool save();
	bool saveBitmap(std::string filename);
	~DXPainting();
};

DXPainting::DXPainting() {
	width = 0;
	height = 0;
	memset(palette, 0, sizeof(int) * 256);
	current_frame = -1;
	for (int i = 0; i < 16; i++) {
		gradients[i].index_start = (i * 16);
		gradients[i].index_end = (i * 16) + 16;
	}
	name = "";
}

void DXPainting::updateFrame() {
	blit(picture, frames[current_frame], 0, 0, 0, 0, picture->w, picture->h);
}

void DXPainting::updatePalette() {
	memcpy(PaintDX::palette, palette, sizeof(int) * 256);
}

void DXPainting::updatePicture() {
	blit(frames[current_frame], picture, 0, 0, 0, 0, picture->w, picture->h);
}

void DXPainting::updateGradients() {
	for (int i = 0; i < 16; i++) {
		PaintDX::gradients[i] = gradients[i];
	}
}

void DXPainting::selectFrame(int index) {
	if (index >= (int)frames.size())return;
	
	if (current_frame > -1) {
		updateFrame();// Safeward current frame before switching
	}

	current_frame = index;
	BITMAP *b = frames[index];
	updatePicture();
}

void DXPainting::create(int w, int h, int frame_count, int defaultspeed) {
	width = w;
	height = h;
	
	for (int i = 0; i < frame_count; i++) {
		addFrame(defaultspeed);
	}
	current_frame = 0;
	
	// don't forget palette and gradient setup!
	selectFrame(0);
}

bool DXPainting::load(std::string filename) {
	FILE *fp;
	fopen_s(&fp, filename.c_str(), "r");
	if (!fp) {
		allegro_message("ERROR: File Not Found");
		return false;
	}

	name = filename;

	int framecount;
	// deserialize properties
	fscanf_s(fp, "[%d,%d,%d]\n", &width, &height, &framecount);

	create(width, height, framecount, 60);
	
	// serialize palette
	fscanf_s(fp, "[%d,", &palette[0]);
	for (int i = 1; i < 256; i++) {
		fscanf_s(fp, "%d,", &palette[i]);
	}
	fscanf_s(fp, "-1]\n");

	// serialize gradients
	for (int i = 0; i < 16; i++) {
		int forward;
		fscanf_s(fp, "[%d,%d,%d,%d,%d]\n", &gradients[i].index_start, &gradients[i].index_end, &gradients[i].cycle_speed, &gradients[i].spatter, &forward);
		gradients[i].forward = (bool)forward;
	}

	// serialize frame data (repeat for each frame)
	for (int i = 0; i < framecount; i++) {
		int speed = 0;
		fscanf_s(fp, "[%d,", &speed);
		addFrame(speed);
		BITMAP *b = frames[i];
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				fscanf_s(fp, "%d,", &((long*)(b->line[y]))[x]);				
			}
		}
		fscanf_s(fp, "-1]\n");
	}
	fclose(fp);

	std::string caption = std::string("PaintDX - " + name);	
	SetWindowTextA(PaintDX::handle, caption.c_str());
	return true;
}

bool DXPainting::saveBitmap(std::string filename) {
	return save_bitmap(name.c_str(), vram, nullptr);
}

bool DXPainting::save() {
	if (name.substr(name.find_last_of(".") + 1) != "pdx") {
		if (name.substr(name.find_last_of(".") + 1) != "PDX") {
			name += ".pdx";
		}
	}	

	FILE *fp;
	fopen_s(&fp, name.c_str(), "w");
	if (!fp) {
		allegro_message("ERROR: Cannot write output file");
		return false;
	}
		
	// serialize properties
	fprintf_s(fp, "[%d,%d,%d]\n", width, height, (int)frames.size());
	
	// serialize palette
	fprintf_s(fp, "[%d,", palette[0]); 
	for (int i = 1; i < 256; i++) {
		fprintf_s(fp, "%d,", palette[i]);
	}
	fprintf_s(fp, "-1]\n");
	
	// serialize gradients
	for (int i = 0; i < 16; i++) {
		fprintf_s(fp, "[%d,%d,%d,%d,%d]\n", gradients[i].index_start, gradients[i].index_end, gradients[i].cycle_speed, gradients[i].spatter, (int)gradients[i].forward);
	}

	// serialize frame data (repeat for each frame)
	for (int i = 0; i < (int)frames.size(); i++) {
		BITMAP *b = frames[i];
		fprintf_s(fp, "[%d,", speeds[i]);
		int c = ((long*)(b->line[0]))[0]; 
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				c = ((long*)(b->line[y]))[x];
				fprintf_s(fp, "%d,",c);
			}
		}
		fprintf_s(fp, "-1]\n");
	}
	fclose(fp);

	std::string caption = std::string("PaintDX - " + name);
	SetWindowTextA(PaintDX::handle, caption.c_str());
	return true;
}

void DXPainting::addFrame(int frame_speed) {
	BITMAP *b = create_bitmap(width, height);
	if (!b) {
		allegro_message("Out of memory");
		exit(-1);
	}
	clear_to_color(b, 15);
	frames.push_back(b);
	speeds.push_back(frame_speed);
}

void DXPainting::copyFrame(BITMAP *frame_to_copy, int frame_speed) {
	BITMAP *b = create_bitmap(frame_to_copy->w, frame_to_copy->h);
	if (!b) {
		allegro_message("Out of memory");
		exit(-1);
	}
	blit(frame_to_copy, b, 0, 0, 0, 0, b->w, b->h);
	frames.push_back(b);
	speeds.push_back(frame_speed);
}

void DXPainting::deleteFrame(int index) {
	if (index >= (int)frames.size())return;
	BITMAP *b = frames[index];
	frames.erase(std::find(frames.begin(), frames.end(), frames[index]));
	destroy_bitmap(b);
	if (current_frame >= index)current_frame--;
	if (current_frame < 0)current_frame=0;
	selectFrame(current_frame);
}

DXPainting::~DXPainting() {
	width = 0;
	height = 0;
	memset(palette, 0, sizeof(int) * 256);
	while(frames.size()){
		deleteFrame(0);
	}
	frames.clear();
	speeds.clear();
}


DXPainting *PaintDX::dxpainting = nullptr;

std::string infobar_text;

extern void* menu_help;
extern void* menu_picture;
extern void* menu_brush;
extern void* menu_techniques;
extern void* menu_stencil;
extern void* menu_font;
extern void* menu_misc;
extern void* menu_preferences;
extern int current_gradient;
extern void GPUSetColor(int);


BrushType	active_brush = BT_POINT_1;
int 		active_tool = (int)T_DASH;//default
bool		menu_tool = false;

void(*brush_point_callback[T_COUNT])(BITMAP*, int, int, int);

int			oldwidth = 0;
int			oldheight = 0;

int			pasive_color = 15;
int			active_color = 0;
int			active_subtool[22];
int			zoom = 1;
float		scale_x;
float		scale_y;
float		window_x;
float		window_y;
float		avail_w;
float		avail_h;
float		window_w;
float		window_h;

// Working



std::deque<BITMAP *>	undo;
BITMAP *backup= nullptr;
BITMAP *ovram = nullptr;
BITMAP *offram = nullptr;
BITMAP *spare = nullptr;
BITMAP *ui_vram = nullptr;
BITMAP *buttons = nullptr;
BITMAP *brushes = nullptr;
BITMAP *dashmodes = nullptr;
BITMAP *brush = nullptr;
BITMAP *gradients = nullptr;
BITMAP *sprays= nullptr;
BITMAP *patterns = nullptr;
BITMAP *pattern[32];
BITMAP *oddeven = nullptr;

Menu tool_menus[22];

int white = 0;
int pink  = 0;

HWND oldhnd;

int PaintDX::palette[256];

int last_x = -1;
int last_y = -1;
int click_x	= 0;
int click_y = 0;
int unclick_x = 0;
int unclick_y = 0;

void panic() {
	allegro_message("Out of memory!");
	exit(-1);
}

void GPUSyncPainting() {
	PaintDX::dxpainting->updateFrame();
	memcpy(PaintDX::dxpainting->palette, PaintDX::palette, sizeof(int) * 256);	//copy palette
	for (int i = 0; i < 16; i++) {
		PaintDX::dxpainting->gradients[i] = PaintDX::gradients[i];
	}
}

void GPUUndo() {
	for (int i = 0; i < 2; i++) {
		BITMAP *u = undo.back();
		undo.erase(undo.end() - 1);
		blit(u, picture, 0, 0, 0, 0, u->w, u->h);
		clear_to_color(u, 15);
		undo.insert(undo.begin(), 1, u);
	}
	PaintDX::redraw();
	GPUSyncPainting();
}

void GPUSaveState() {
	// Make undo
	BITMAP *u = undo.front();
	undo.erase(undo.begin());
	blit(picture, u, 0, 0, 0, 0, u->w, u->h);
	undo.push_back(u);	
}

void GPUSetZoom(int level) {
	if (level < 0)return;
	zoom = level;
	scale_x = 1.0f / float((zoom + 1));
	scale_y = 1.0f / float((zoom + 1));
	window_x = 0.0f;
	window_y = 0.0f;
	window_w = VWIDTH * scale_x;
	window_h = VHEIGHT * scale_y;
}

void GPUSetScroll(int x, int y) {
	float fx, fy;

	fx = float(x);
	fy = float(y);

	window_x = fx;
	window_y = fy;
	
	if (window_x < 0.0f)window_x = 0.0f;
	if (window_y < 0.0f)window_y = 0.0f;
	avail_w = 0;
	avail_h = 0;

	if (!picture)return;
	avail_w = (picture->w) - window_w;
	avail_h = (picture->h) - window_h;
}

extern FillType active_fill;

int findColor(int color) {

	int dist[256];
	int distr[256];
	int distg[256];
	int distb[256];

	int r = getr(color);
	int g = getg(color);
	int b = getb(color);
	int c, cr, cg, cb;
	for (int i = 0; i < 256; i++) {
		c = PaintDX::palette[i];
		cr = getr(c);
		cg = getg(c);
		cb = getb(c);

		dist[i] = abs(r - cr)+abs(g - cg)+abs(b - cb);
		distr[i] = abs(r - cr);
		distg[i] = abs(g - cg);
		distb[i] = abs(b - cb);
	}

	int lowest_dist_r = 255;
	int lowest_dist_g = 255;
	int lowest_dist_b = 255;
	int lowest_dist = 255;
	int lowest_match = color;

	// Find Lowest distance RGB match
	for (int i = 0; i < 256; i++) {
		if (dist[i] < lowest_dist) {
			lowest_dist = dist[i];
			lowest_match = i;
		}
		/*if (distr[i] <= lowest_dist_r)
			if (distg[i] <= lowest_dist_g)
				if (distb[i] <= lowest_dist_b) {
					lowest_dist_r = distr[i];
					lowest_dist_g = distg[i];
					lowest_dist_b = distb[i];
					lowest_match = i;
				}*/
	}
	return lowest_match;
}

int findAverage(int x, int y) {
	int c[5] = {
		PaintDX::palette[getpixel(picture, x, y)],
		PaintDX::palette[getpixel(picture, x, y + 1)],
		PaintDX::palette[getpixel(picture, x + 1, y)],
		PaintDX::palette[getpixel(picture, x, y - 1)],
		PaintDX::palette[getpixel(picture, x - 1, y)]
	};
	int r = float(getr(c[0]) + getr(c[1]) + getr(c[2]) + getr(c[3]) + getr(c[4])) / 5.0f;
	int g = float(getg(c[0]) + getg(c[1]) + getg(c[2]) + getg(c[3]) + getg(c[4])) / 5.0f;
	int b = float(getb(c[0]) + getb(c[1]) + getb(c[2]) + getb(c[3]) + getb(c[4])) / 5.0f;
	int index = findColor(makecol(r, g, b));
	return index;
}

int findHalf(int color) {
	color = PaintDX::palette[color];
	int r = float(getr(color)) * 0.75f;
	int g = float(getg(color)) * 0.75f;
	int b = float(getb(color)) * 0.75f;
	int index = findColor(makecol(r, g, b));
	return index;
}

int findDouble(int color) {
	color = PaintDX::palette[color];
	int r = float(getr(color)+1) * 1.25f;
	int g = float(getg(color)+1) * 1.25f;
	int b = float(getb(color)+1) * 1.25f;
	r = r > 255 ? 255 : r;
	g = g > 255 ? 255 : g;
	b = b > 255 ? 255 : b;
	int index = findColor(makecol(r, g, b));
	return index;
}

void pointBrush(BITMAP *bmp, int x, int y, int c) {
	static int dash_timer = 0;
	static int last_x = x;
	static int last_y = y;
	
	int delta_x = last_x - x;
	int delta_y = last_y - y;
	
	if ((y != last_y) || (x != last_x)) {
		dash_timer++;
		last_x = x;
		last_y = y;
	}

	
	DashMode d = (DashMode)active_subtool[T_DASH];
	switch (d) {
		case DM_SOLID:
			dash_timer = 0;
			break;
	
		case DM_ODDEVEN:
			dash_timer %= (active_subtool[active_tool]+1)*2;
			break;
		
		case DM_SPARE:
			dash_timer %= (active_subtool[active_tool] + 1) * 3;
			break;
	}

	drawing_mode(DRAW_MODE_MASKED_PATTERN, pattern[active_subtool[T_PATTERNS]], click_x, click_y);
	Gradient *g = &PaintDX::gradients[current_gradient];

	if (dash_timer==0) {
		if (!PaintDX::brush.enabled) {
			for (int iy = 0; iy < 8; iy++) {
				for (int ix = 0; ix < 8; ix++) {

					int mx = (x - 4) + ix;
					int my = (y - 1) + iy;
					if (getpixel(brushes, (active_subtool[T_BRUSH] * 8) + ix, iy) != makecol(255, 0, 255)) {

						bool transparent = false;

						switch (PaintDX::color_mode) {
						case CM_REPLACE:
							transparent = true;
						case CM_PAINT:
							if (brush == nullptr) {
								PaintDX::color_mode = CM_SINGLECOLOR;
								PaintDX::updateMainMenu();
							}
							else {
								// blit brush bitmap onto picture
								if (transparent) {

								}
								else {

								}
								break;
							}


						case CM_SINGLECOLOR:
							putpixel(bmp, mx, my, c);
							continue;

						case CM_CYCLE:
							putpixel(bmp, mx, my, c);
							if (g->forward)GPUSetColor(c >= g->index_end ? g->index_start : c + 1);
							else GPUSetColor(c <= g->index_start ? g->index_end : c - 1);
							continue;

						case CM_SMOOTH:
							if (PaintDX::mouseclick)putpixel(bmp, mx, my, findAverage(mx, my));
							continue;

						case CM_SMEAR:
							putpixel(bmp, mx + delta_x, my + delta_y, getpixel(bmp, mx, my));
							continue;

						case CM_SHADE:
							if (PaintDX::mouseclick)putpixel(bmp, mx, my, findHalf(getpixel(picture, mx, my)));
							else putpixel(bmp, mx, my, findDouble(getpixel(picture, mx, my)));
							continue;
						}
					}
				}
			}
		} else { // PaintDX::custom_brush = true 
			switch (PaintDX::color_mode) {
				default:
				case CM_PAINT:PaintDX::brush.drawTransparent(x, y); break;
				case CM_REPLACE:PaintDX::brush.draw(x, y); break;
				case CM_SHADE: PaintDX::brush.shade(x, y); break;
				case CM_SMOOTH: PaintDX::brush.smooth(x, y); break;
				case CM_SINGLECOLOR: PaintDX::brush.drawMasked(x, y); break;
			}
			
		}
	}
}

void setRGB(int color, int r, int g, int b) {
	PaintDX::palette[color] = makecol(r, g, b);
}

void setHSV(int color, float h, float s, float v) {
	int ir, ig, ib;
	s /= 100.0f;
	v /= 100.0f;
	hsv_to_rgb(h, s, v, &ir, &ig, &ib);
	PaintDX::palette[color] = makecol(ir, ig, ib);
}

void getRGB(int color, int &r, int  &g, int &b) {
	r = getr(color);
	g = getg(color);
	b = getb(color);
}

void getHSV(int color, float &h, float  &s, float &v) {
	rgb_to_hsv(getr(color), getg(color), getb(color), &h, &s, &v);
	s *= 100.0f;
	v *= 100.0f;
}


void pointFlood(BITMAP *bmp, int x, int y, int c) {
	
	int original = getpixel(vram, x, y);
	if (original == makecol(255, 0, 255))return;
	
	drawing_mode(DRAW_MODE_MASKED_PATTERN, pattern[active_subtool[T_PATTERNS]], click_x, click_y);

	floodfill(picture, x, y, active_color);
	solid_mode();
}

void pointHand(BITMAP *bmp, int x, int y, int c) {
	// Hand tool: drag, if zoom is enabled
	avail_w = (picture->w)-window_w;
	avail_h = (picture->h)-window_h;
	int move_x = float(unclick_x - click_x)*1.5f;
	int move_y = float(unclick_y - click_y)*1.5f;

	bool repeat = true;
	while (repeat) {
		repeat = false;
		if (avail_w - (window_x + move_x) > 0) {
			window_x += move_x;
			avail_w -= window_x;
			click_x = unclick_x;
		}
		else {
			if (move_x > 0) {
				move_x--;
				repeat = true;
			}
		}

		if (avail_h - (window_y + move_y) > 0) {
			window_y += move_y;
			avail_h -= window_y;
			click_y = unclick_y;
		}
		else {
			if (move_y > 0) {
				repeat = true;
				move_y--;
			}
		}

		if (window_x < 0.0f)window_x = 0.0f;
		if (window_y < 0.0f)window_y = 0.0f;
	}
	PaintDX::redraw();
}

void pointText(BITMAP *bmp, int x, int y, int c) {
	// put text cursor at x,y
}

void pointSpray(BITMAP *bmp, int x, int y, int c) {
	// randomly plot in given radius
	int area = (active_subtool[active_tool]+1) * 8;
	for(int i=0; i<8; i++){
		int rx = x + ((rand() % (area*2)) - (area));
		int ry = y + ((rand() % (area*2)) - (area));

		int distance = sqrtl(((x - rx)*(x - rx)) + ((y - ry)*(y - ry)));
		if (distance <= area) {
			pointBrush(bmp, rx, ry, c);			
		} else i--;
	}
	
}

void pointCopy(BITMAP *bmp, int x, int y, int c) {
	// copy instead of painting
}

void pointPipe(BITMAP *bmp, int x, int y, int c) {
	// capture color from canvas
	if (!PaintDX::mouseaux)active_color = getpixel(picture, x, y);
	else pasive_color = getpixel(picture, x, y);
}

void pointZoom(BITMAP *bmp, int x, int y, int c) {
	// zoom from click_x,click_y to x,y if mouserelease, show preview rectangle if still clicking
}

void initializePalette() {
#define palette PaintDX::palette
	palette[0] = 0;
	palette[255] = makecol(255, 0, 255);
	palette[1] = makecol(0, 0, 127);
	palette[2] = makecol(0, 127, 0);
	palette[3] = makecol(0, 127, 127);
	palette[4] = makecol(127, 0, 0);
	palette[5] = makecol(127, 0, 127);
	palette[6] = makecol(127, 127, 0);
	palette[7] = makecol(127, 127, 127);
	palette[8] = makecol(63, 63, 63);
	palette[9] = makecol(0, 0, 255);
	palette[10] = makecol(0,255,0);
	palette[11] = makecol(0, 255,255);
	palette[12] = makecol(255,0,0);
	palette[13] = makecol(255, 0, 255);
	palette[14] = makecol(255, 255, 0);
	palette[15] = makecol(255, 255, 255);
	
	for (int i = 0; i < 16; i++) {
		
		palette[16 + i] = makecol((i * 16),(i * 16),(i * 16));
		palette[32 + i] = makecol((i * 16), 0, 0);
		palette[48 + i] = makecol(0, (i * 16), 0);
		palette[64 + i] = makecol(0, 0, (i * 16));
		palette[80 + i] = makecol(0, (i * 16), (i * 16));
		palette[96 + i] = makecol((i * 16), 0, (i * 16));
		palette[112 + i] = makecol((i * 16), (i * 16), 0);
		palette[128 + i] = makecol((i * 16), i*8, (i * 16));
		palette[144 + i] = makecol(i*8, (i * 16), (i * 16));
		palette[160 + i] = makecol((i * 16), (i * 16), 255);
		palette[176 + i] = makecol((i * 16), 255, (i * 16));
		palette[192 + i] = makecol(255, (i * 16), (i * 16));
		palette[208 + i] = makecol(255, 128 + (i * 8), (i * 16));
		palette[224 + i] = makecol(128 + (i * 8), 255, 255);
		palette[240 + i] = makecol(255, 255, 128 + (i * 8));
	}
#undef palette
}

void spareToPage() {
	blit(spare, picture, 0, 0, 0, 0, spare->w, spare->h);
	PaintDX::redraw();
}

void flipPageSpare() {
	blit(spare, ovram, 0, 0, 0, 0, spare->w, spare->h);
	blit(picture, spare, 0, 0, 0, 0, spare->w, spare->h);
	blit(ovram, picture, 0, 0, 0, 0, spare->w, spare->h);
	clear_to_color(ovram, makecol(255, 0, 255));
	PaintDX::redraw();
}

void updatePalette(void) {
	// Update gradient subtool preview bitmaps
	int x = 0;
	for (int i = 0; i < 16; i++) {
		PaintDX::gradients[i].draw(gradients, x, 0, x+32, 16);
		x += 32;
	}
}

void drawSelection(int x) {
	x++;
	int dx = x + (TOOLBAR_WIDTH - 3);
	int y = (TOOLBAR_HEIGHT - BUTTON_EXTRA) + 1;
	int dy = y + (BUTTON_EXTRA - 3);
	int bg = PaintDX::palette[pasive_color];
	int fg = PaintDX::palette[active_color];

	//TODO: apply pattern style
	rectfill(ui_vram, x, y, dx, dy, bg);
	//TODO: apply pattern style
	rectfill(ui_vram, x + 10, y + 8, dx - 10, dy - 8, fg);
}

static BITMAP *gradient_preview = nullptr;

void drawGradientPreview(void *target, std::vector<ColorStop> &stops) {
	// Find destination bitmap width and height
	RECT r;
	GetClientRect((HWND)target, &r);
	int w = r.right - r.left;
	int h = r.bottom - r.top;
	
	// If bitmap does not exist yet, create it!
	if (!gradient_preview) {
		gradient_preview = create_bitmap(w, h);
		if (!gradient_preview) panic();
		clear_bitmap(gradient_preview);
	}
	
	// Calculate how much a stop will ocuppy
	drawing_mode(DRAW_MODE_SOLID, 0, 0, 0);
	int memo_x = 0;

	for (ColorStop cs : stops) {
		float x = w * cs.position;
		if(cs.dither_pattern<1)rectfill(gradient_preview, memo_x, 0, x, h, PaintDX::palette[cs.color]);
		else {
			rectfill(gradient_preview, memo_x, 0, x, h, PaintDX::palette[cs.alt_color]);
			drawing_mode(DRAW_MODE_MASKED_PATTERN, pattern[22+ cs.dither_pattern], 0, 0);
			rectfill(gradient_preview, memo_x, 0, x, h, PaintDX::palette[cs.color]);
			drawing_mode(DRAW_MODE_SOLID, 0,0,0);
		}
		memo_x = x;
	}
	if(stops.size())
	rectfill(gradient_preview, memo_x, 0, w, h, PaintDX::palette[stops[stops.size()-1].color]);
	
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint((HWND)target, &ps);
	stretch_blit_to_hdc(gradient_preview, hdc, 0, 0, w, h, 0, 0, w, h);
	EndPaint((HWND)target, &ps);
	InvalidateRect((HWND)target, &r, false);
}

void drawGradientPalette(void *target, bool selected[256], int mx, int my, int current) {
	clear_to_color(offram, pink);
	
	int cx, dx, dy, sx, sy;

	bool hover;
	int c = 0;
	for (int i = 0; i < 16; i++) {
		sx = (i*(GRADIENT_CELL_W + GRADIENT_SEL_W));
		dx = sx + GRADIENT_SEL_W;
		cx = dx + GRADIENT_CELL_W;
		for (int y = 0; y < 16; y++) {
			sy = y * GRADIENT_CELL_H;
			dy = sy + GRADIENT_CELL_H;
			hover = mx == i ? my == y ? true : false : false;
			rectfill(offram, sx, sy, dx, dy, selected[(i*16)+y] ? makecol(0,0,255) : hover ? makecol(128,128,255):white);
			rectfill(offram, dx, sy, dx + GRADIENT_CELL_W, dy, PaintDX::palette[(i * 16) + y]);
			rect(offram, dx, sy, dx + GRADIENT_CELL_W - 1, dy, makecol(32, 32, 32));
			if(current==c)rectfill(offram, dx+2, sy+2, dx + GRADIENT_CELL_W - 3, dy-2, INT_MAX - PaintDX::palette[(i * 16) + y]);
			c++;
		}
	}

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint((HWND)target, &ps);
	RECT r;
	GetClientRect((HWND)target, &r);
	int w = r.right - r.left;
	int h = r.bottom - r.top;
	stretch_blit_to_hdc(offram, hdc, 0, 0, 16 * (GRADIENT_CELL_W + GRADIENT_SEL_W), 16 * GRADIENT_CELL_H, 1, 1, w - 1, h - 2);
	EndPaint((HWND)target, &ps);
	InvalidateRect((HWND)target, &r, false);
}

void *GPUGetSnapShot() {
	return convert_bitmap_to_hbitmap(vram);
}


// Draws big palette on ui_vram
void drawPalette(int x, int height) {
	int colors_per_row = 256 / 6;
	int color_height = height / colors_per_row;
	if (color_height <= 0)return;
	int rmax = height / color_height;
	int i = 0; int y = TOOLBAR_HEIGHT;
	int cols = TOOLBAR_WIDTH / COLOR_WIDTH;
	for (int c = 0; c < cols; c++) {
		for (int r = 0; r < rmax; r++) {
			rectfill(ui_vram, x + (c * COLOR_WIDTH), y + (r * color_height), x + ((c * COLOR_WIDTH) + COLOR_WIDTH)-1, y + (r * color_height) + color_height-1, PaintDX::palette[i]);
			rect(ui_vram, x + (c * COLOR_WIDTH), y + (r * color_height), x + ((c * COLOR_WIDTH) + 7), y + (r * color_height) + (color_height-1), i==active_color?makecol(255,255,255):0);
			i++;
			if (i == 256) {
				rect(gradients, 0, gradients->h - 16, 32, gradients->h, PaintDX::palette[active_color]);
				return;
			}
		}
	}
	rect(gradients, 0, gradients->h - 16, 32, gradients->h, PaintDX::palette[active_color]);
}

void drawMenu(int x) {
	if (!tool_menus[active_tool].bitmap)return;
	
	int y = active_tool % 11;
	x = active_tool >= 11 ? 1 : 0;
	x *= BUTTON_WIDTH;
	y *= BUTTON_HEIGHT;
	x += ui_vram->w - TOOLBAR_WIDTH;
	y += BUTTON_HEIGHT;
	
	int size_x = tool_menus[active_tool].item_width * tool_menus[active_tool].columns;
	int size_y = tool_menus[active_tool].item_height * tool_menus[active_tool].rows;

	int cell_w = size_x / tool_menus[active_tool].columns;
	int cell_h = size_y / tool_menus[active_tool].rows;
	
	int cols = size_x / cell_w;
	int rows = size_y / cell_h;

	int sx = x - (size_x);
	int sy = y - (size_y / 2);
	int dx = sx + (size_x);
	int dy = sy + (size_y);

	
	BITMAP *bmp= (BITMAP*)tool_menus[active_tool].bitmap;
		
	int i = 0;
	if (bmp) {

		rectfill(ui_vram, sx + 2, sy + 2, dx + 2, dy + 2, makecol(128, 128, 128));
		rectfill(ui_vram, sx, sy, dx, dy, makecol(255, 255, 255));
		for (int r = 0; r < tool_menus[active_tool].rows; r++) {
			for (int c = 0; c < tool_menus[active_tool].columns; c++) {
				if (i == active_subtool[active_tool]) {
					rectfill(ui_vram, sx + (c*cell_w), sy + (r*cell_h), sx + ((c*cell_w) + cell_w), sy + ((r*cell_h) + cell_h)-1, makecol(255, 255, 0));
					masked_blit(bmp, ui_vram, i*cell_w, 0, sx + (c*cell_w), sy + (r*cell_h), cell_w, cell_h);
					rect(ui_vram, sx + (c*cell_w), sy + (r*cell_h), sx + ((c*cell_w) + cell_w), sy + ((r*cell_h) + cell_h)-1, makecol(0,0, 0));
				} else masked_blit(bmp, ui_vram, i*cell_w, 0, sx + (c*cell_w), sy + (r*cell_h), cell_w, cell_h);
				i++;
			}
		}
		rect(ui_vram, sx, sy, dx, dy, makecol(0, 0, 0));
	}
}

void drawButtons(int x) {
	int i=0;
	for (int c = 0; c < 2; c++) {
		for (int r = 0; r < BUTTON_ROWS; r++) {
			int color = i == active_tool ? makecol(255, 255, 0) : makecol(255, 255, 255);
			
			if ((i==T_GRID) && PaintDX::enable_grid) color = makecol(255, 200, 200);
			if ((i==T_SYMMETRY)&&PaintDX::enable_symmetry) color = makecol(255, 200, 200);
			
			rectfill(ui_vram, x + (c * BUTTON_WIDTH), (r * BUTTON_HEIGHT), x + (c * BUTTON_WIDTH) + BUTTON_WIDTH, (r * BUTTON_HEIGHT) + BUTTON_HEIGHT, color);
			rect(ui_vram, x + (c * BUTTON_WIDTH), (r*BUTTON_HEIGHT), x+(c*BUTTON_WIDTH)+BUTTON_WIDTH, (r*BUTTON_HEIGHT)+BUTTON_HEIGHT, 0);
			i++;
		}
	}
	drawing_mode(DRAW_MODE_XOR,0,0,0);
	draw_sprite(ui_vram, buttons, x, 0);
	drawing_mode(DRAW_MODE_SOLID,0,0,0);

	drawSelection(x);
	
	if (menu_tool) {
		drawMenu(x);
	}
}


void pencil(int x, int y) {
	if (PaintDX::enable_grid) {
		x -= PaintDX::grid_origin_x;
		y -= PaintDX::grid_origin_y;
		x -= x % PaintDX::grid_width;
		y -= y % PaintDX::grid_height;
	}
	do_line(picture, last_x, last_y, x, y, active_color, brush_point_callback[active_tool]);
	last_x = x;
	last_y = y;
}

bool click_lock = false;

void prepareStroke(int x, int y) {
	if (PaintDX::enable_grid) {
		x -= PaintDX::grid_origin_x;
		y -= PaintDX::grid_origin_y;
		x -= x % PaintDX::grid_width;
		y -= y % PaintDX::grid_height;
	}
	clear_to_color(ovram, pink);
	if (!click_lock) {
		unclick_x = x;
		unclick_y = y;
		click_x = x;
		click_y = y;
		click_lock = true;
	}
}

extern ToolType last_tool;


// Selects given index tool, additionally opening a subtool selection box
void GPUSetTool(int index) {
	if (active_tool != (ToolType)index)
		last_tool = (ToolType)active_tool;

	infobar_text = "Tool " + std::to_string(index) + " selected.";

	active_tool = (ToolType)index;
	menu_tool = true;
}



void rotateColors(int start, int end) {
	if (start == end)return;
	int backup = PaintDX::palette[start];

	for (int i = start; i < end; i++) {
		PaintDX::palette[i] = PaintDX::palette[i + 1];
	}
	PaintDX::palette[end] = backup;
}

void PaintDX::moveLeft(bool alt) {
	if (window_x > 0) {
		window_x--;
		avail_w++;
		redraw();
	}
}
void PaintDX::moveRight(bool alt) {
	if (window_x < avail_w) {
		window_x++;
		avail_w--;
		redraw();
	}
}
void PaintDX::moveUp(bool alt) {
	if (window_y > 0) {
		window_y--;
		avail_h++;
		redraw();
	}
}
void PaintDX::moveDown(bool alt) {
	if (window_y < avail_h) {
		window_y++;
		avail_h--;
		redraw();
	}
}

void PaintDX::minus(bool alt) {
	Gradient *g = &gradients[current_gradient];
	// if brush is active, modify it instead of picking other color!
	if (!enable_multicycle) {
		active_color = g->forward ? active_color - (alt ? 16 : 1) : active_color + (alt ? 16 : 1);
		active_color = g->forward ? active_color > g->index_end ? g->index_start : active_color : active_color < g->index_start ? g->index_end : active_color;
	}
	else {
		active_color -= (alt ? 16 : 1);
		active_color = active_color < 0 ? 255 : active_color;
	}
	redraw();
}
void PaintDX::plus(bool alt) {
	Gradient *g = &gradients[current_gradient];
	// if brush is active, modify it instead of picking other color!
	if (!enable_multicycle) {
		active_color = g->forward ? active_color + (alt ? 16 : 1) : active_color - (alt ? 16 : 1);
		active_color = g->forward ? active_color > g->index_end ? g->index_start : active_color : active_color < g->index_start ? g->index_end : active_color;
	}
	else {
		active_color += (alt ? 16 : 1);
		active_color > 255 ? 0 : active_color;
	}
	redraw();
}

void PaintDX::updateCycle(int index) {
	if (!PaintDX::enable_cycle)return;
	int old[256];
	memcpy(old, palette, sizeof(int) * 256);

	Gradient g = gradients[index];	// cycle gradients
	rotateColors(g.index_start, g.index_end);
	g.calculateStops();
	// update palette


	for (int y = 0; y < vram->h; y++) {
		for (int x = 0; x < vram->w; x++) {
			for (int c = g.index_start; c < g.index_end; c++) {
				if (((long*)vram->line[y])[x] == old[c]) ((long*)vram->line[y])[x] = palette[c];
			}
		}
	}

	redraw();
}

std::vector<Point> shape;

int last_shape_x;
int last_shape_y;

void prepareShape(int x, int y) {
	if (PaintDX::enable_grid) {
		x -= PaintDX::grid_origin_x;
		y -= PaintDX::grid_origin_y;
		x -= x % PaintDX::grid_width;
		y -= y % PaintDX::grid_height;
	}
	if (!click_lock) {
		unclick_x = x;
		unclick_y = y;
		click_x = x;
		click_y = y;
		click_lock = true;
	}
	
	if ((last_shape_x != x)|| (last_shape_y != y)) {
		shape.push_back({ x,y });
		last_shape_x = x;
		last_shape_y = y;
	}	
}


void drawShape(bool real_color = false) {
	if (!shape.size())return;
	clear_to_color(ovram, pink);
	Point o = shape[0];
	Point i = shape[0];
	for (Point p : shape) {
		line(ovram, o.x, o.y, p.x, p.y, real_color ? active_color : PaintDX::palette[active_color]);
		o = p;
	}
	line(ovram, o.x, o.y, i.x, i.y, real_color ? active_color : PaintDX::palette[active_color]);	
}

void fillShape() {
	floodfill(ovram, 0, 0, makecol(255, 0, 255));
	if (!shape.size())return;
	Point o = shape[0];

	Point centroid = { 0, 0 };
	double signedArea = 0.0;
	double x0 = 0.0; // Current vertex X
	double y0 = 0.0; // Current vertex Y
	double x1 = 0.0; // Next vertex X
	double y1 = 0.0; // Next vertex Y
	double a = 0.0;  // Partial signed area

					 // For all vertices
	int i = 0;
	int vertexCount = int(shape.size());
	for (i = 0; i<vertexCount; ++i)
	{
		x0 = shape[i].x;
		y0 = shape[i].y;
		x1 = shape[(i + 1) % vertexCount].x;
		y1 = shape[(i + 1) % vertexCount].y;
		a = x0 * y1 - x1 * y0;
		signedArea += a;
		centroid.x += (x0 + x1)*a;
		centroid.y += (y0 + y1)*a;
	}

	signedArea *= 0.5;
	centroid.x /= (6.0*signedArea);
	centroid.y /= (6.0*signedArea);

	floodfill(ovram, centroid.x, centroid.y, makecol(255,0,255));
	drawing_mode(DRAW_MODE_MASKED_PATTERN, pattern[active_subtool[T_PATTERNS]], click_x, click_y);
	floodfill(ovram, centroid.x, centroid.y, active_color);
	solid_mode();
	
}

void lockMouse(int x, int y) {
	if (PaintDX::enable_grid) {
		x -= PaintDX::grid_origin_x;
		y -= PaintDX::grid_origin_y;
		x -= x % PaintDX::grid_width;
		y -= y % PaintDX::grid_height;
	}
	if (!click_lock) {
		click_x = x;
		click_y = y;
		click_lock = true;
	}
}

void unlockMouse() {
	click_lock = false;	
}

void GPURectangleFill(BITMAP *bmp, int x, int y, int dx, int dy) {
	// Select corresponding fillmode
	for (int iy = y; iy < dy; iy++) {
		do_line(ovram, x, iy, dx, iy, PaintDX::palette[active_color], pointBrush);
	}
}

void overlayRectangle() {
	clear_to_color(ovram, pink);
	rect(ovram, click_x, click_y, unclick_x, unclick_y, makecol(255, 0, 0));
}

void overlayLine() {
	clear_to_color(ovram, pink);
	line(ovram, click_x, click_y, unclick_x, unclick_y, makecol(255, 0, 0));
}

void PaintDX::getRGB(int index, int &r, int &g, int &b) {
	int color = palette[index];
	r = getr(color);
	g = getg(color);
	b = getb(color);
}

void PaintDX::setRGB(int index, int r, int g, int b, bool redraw) {
	int c = makecol(r, g, b);
	int rep = palette[index];

	for (int y = 0; y < vram->h; y++) {
		for (int x = 0; x < vram->w; x++) {
			if (((long*)vram->line[y])[x] == rep) ((long*)vram->line[y])[x] = c;
		}
	}

	palette[index] = c;
	if (redraw)PaintDX::redraw();
}

void GPUPaint(int x, int y) {
	
	/*int x = rx + int(window_x);
	int y = ry + int(window_y);*/
	
	infobar_text = "CLICK( " + std::to_string(click_x) + " , " + std::to_string(click_y) + " ) : UNCLICK( " + std::to_string(unclick_x) + " , " + std::to_string(unclick_y) +
		" ) : SCROLL( " + std::to_string((int)window_x) + " , " + std::to_string((int)window_y) + " )";
	
	if (last_x == -1) last_x = x;
	if (last_y == -1) last_y = y;
	
	switch (active_tool) {
		default:break;	
		case T_SQUARE:	toolSquare(false, x, y); break;
		case T_LINE:	toolLine(false, x, y); break;
		case T_PIPE:	toolPipe(false, x, y); break;
		case T_ZOOM:	toolZoom(false, x, y); break;
		case T_COPY:	toolCopy(false, x, y); break;	
		case T_HAND:	toolHand(false, x, y); break;
		case T_FLOOD:	toolFlood(false, x, y); break;		
		case T_CIRCLE:	toolCircle(false, x, y); break;
		case T_POLY:	toolPoly(false, x, y); break;
		case T_SHAPE:	toolShape(false, x, y); break;
		case T_PERSPECTIVE:toolPerspective(false, x, y); break;
		case T_DASH:	toolBrush(false, x, y); break;
		case T_SPRAY:	toolSpray(false, x, y); break;		
		case T_TEXT:	toolText(false, x, y); break;
		case T_BEZIER:	toolBezier(false, x, y); break;
	}

	last_x = x;
	last_y = y;

}

void GPUReleaseMouse(int x, int y) {

	infobar_text = "CLICK( " + std::to_string(click_x) + " , " + std::to_string(click_y) + " ) : UNCLICK( " + std::to_string(unclick_x) + " , " + std::to_string(unclick_y) +
		" ) : SCROLL( " + std::to_string((int)window_x) + " , " + std::to_string((int)window_y) + " )";

	clear_to_color(ovram, pink);
	if (!menu_tool) {
		if ((unclick_x < vram->w) && (unclick_y < vram->h)) {

			unlockMouse();
			
			GPUSaveState();
			GPUSyncPainting();
			
			switch (active_tool) {			
				case T_SQUARE:	toolSquare(true, x, y);	break;
				case T_LINE:	toolLine(true, x, y); break;
				case T_PIPE:	toolPipe(true, x, y); break;
				case T_ZOOM:	toolZoom(true, x, y); break;
				case T_COPY:	toolCopy(true, x, y); break;				
				case T_HAND:	toolHand(true, x, y); break;
				case T_FLOOD:	toolFlood(true, x, y);break;
				case T_CIRCLE:	toolCircle(true, x, y); break;			
				case T_POLY:	toolPoly(true, x, y); break;				
				case T_SHAPE:	toolShape(true, x, y); break;				
				case T_PERSPECTIVE:toolPerspective(true, x, y); break;
				case T_DASH:	toolBrush(true, x, y); break;
				case T_SPRAY:	toolSpray(true, x, y); break;
				case T_TEXT:	toolText(true, x, y); break;
				case T_BEZIER:	toolBezier(true, x, y); break;				
				default:
					break;
			}
		}

	} else {
		switch (active_tool) {
			// return to previous tool
			case T_CLEAR:
				if (MessageBoxA(NULL, "This will erase picture contents", "Warning", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK)
					clear_to_color(picture, 15);
					menu_tool = false;
					active_tool = last_tool;
					PaintDX::redraw();
					break;

			case T_GRID:
				PaintDX::enable_grid ^= 1;
				menu_tool = false;
				active_tool = last_tool;
				PaintDX::redraw();
				break;

			case T_SYMMETRY:
				PaintDX::enable_symmetry ^= 1; 
				menu_tool = false;
				active_tool = last_tool;
				PaintDX::redraw();
				break;

			case T_UNDO:
				GPUUndo();
				menu_tool = false;
				active_tool = last_tool;
				PaintDX::redraw();
				break;

				// return to previous tool directly
			case T_BRUSH:
			case T_GRADIENT:
			case T_PATTERNS:
				menu_tool = false;
				active_tool = last_tool;
				PaintDX::redraw();
				break;

			default:
				break;
		}
	}
	click_lock = false;
	last_x = -1;
	last_y = -1;
	menu_tool = false;
}

void GPUPeek(int x, int y) {
	active_color = getpixel(picture, x, y);
}

void GPUDrawInfoBar() {
	int x = 0;
	int y = ui_vram->h - 16;
	int dx = ui_vram->w - (TOOLBAR_WIDTH+1);
	int dy = ui_vram->h - 1;

	rect(ui_vram, x, y, dx, dy, 0);
	rectfill(ui_vram, x + 1, y + 1, dx - 1, dy - 1, makecol(255, 255,255));
	textprintf_ex(ui_vram, font, x + 3, y + 5, makecol(128,128,128), -1, "%s", infobar_text.c_str());
	textprintf_ex(ui_vram, font, x + 2, y + 4, 0, -1, "%s", infobar_text.c_str());

}

int cursor_x;
int cursor_y;

extern void feedAvi();

void GPURaster(struct HDC__ *context, int width, int height) {
	if ((oldwidth != width) || (oldheight != height)) {
		oldwidth = width;
		oldheight = height;
		if (ui_vram)destroy_bitmap(ui_vram);
		ui_vram = create_bitmap(width, height);
		if (!ui_vram)panic();
	}
	
	int rw, rh;

	float xratio = float(width - TOOLBAR_WIDTH) / float(VWIDTH);
	float yratio = float(height) / float(VHEIGHT);

	rw = int(float(xratio)*float(VWIDTH));
	rh = int(float(yratio)*float(VHEIGHT));
	
	// Copy indexed color picture to vram translating colors to current RGB color
	int c;	
	for (int y = 0; y < vram->h; y++) {
		for (int x = 0; x < vram->w; x++) {
			c = ((long*)picture->line[y])[x];
			if (c != 255) {
				((long*)vram->line[y])[x] = PaintDX::palette[c];
			} 
		}
	}

	/* DRAW GRID IF ENABLED */
	drawing_mode(DRAW_MODE_MASKED_PATTERN, oddeven, 0, 0);
	if (PaintDX::enable_grid) {
		int bx = PaintDX::grid_origin_x - PaintDX::grid_width;
		int by = PaintDX::grid_origin_y - PaintDX::grid_height;
		int ex = PaintDX::grid_width + PaintDX::grid_origin_x + vram->w;
		int ey = PaintDX::grid_height+ PaintDX::grid_origin_y + vram->h;
		for (int y = by; y < ey; y+=PaintDX::grid_height) {
			for (int x = bx; x < ex; x += PaintDX::grid_width) {
				rect(vram, x, y, PaintDX::grid_width, PaintDX::grid_height, 0);
			}
		}
	}
	solid_mode();

	// apply zoom and blit to viewport
	if (PaintDX::brush.enabled) {
		clear_to_color(ovram, pink);
		PaintDX::brush.preview(cursor_x, cursor_y, PaintDX::color_mode == CM_PAINT);
	}
	feedAvi();
	stretch_blit(vram, ui_vram, window_x, window_y, window_w, window_h, 0, 0, rw, rh);
	
	masked_stretch_blit(ovram, ui_vram, window_x, window_y, window_w, window_h, 0, 0, rw, rh);
	drawPalette(width - TOOLBAR_WIDTH, height - TOOLBAR_HEIGHT);
	drawButtons(width - TOOLBAR_WIDTH);
	if (PaintDX::enable_infobar) {
		GPUDrawInfoBar();
	}
	stretch_blit_to_hdc(ui_vram, context, 0, 0, width, height, 0, 0, width, height);

	
}

void GPUSetSubTool(int index) {
	infobar_text = "Subtool " + std::to_string(index) + " selected.";

	active_subtool[active_tool] = index;
}

void GPUSetColor(int index) {
	active_color = index;

}

void GPUSetBackColor(int index) {
	pasive_color = index;
}

void GPUDeinitialize() {
	destroy_bitmap(ui_vram);
	destroy_bitmap(offram);
	destroy_bitmap(ovram);
	destroy_bitmap(vram);
	destroy_bitmap(picture);
	destroy_bitmap(spare);
	allegro_exit();
}

extern void*loadResource(int, int&, int&);


BITMAP *load(int id) {

	BITMAP *data;
	int w;
	int h;
	char* pixels = (char*)loadResource(id, w, h);

	data = create_bitmap(w, h);
	if(!data)panic();
	BITMAP *d = data;

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			unsigned char r = pixels[(w*y + x) * 4 + 2];
			unsigned char g = pixels[(w*y + x) * 4 + 1];
			unsigned char b = pixels[(w*y + x) * 4 + 0];
			putpixel(d, x, y, makecol(r, g, b));
		}
	}

	//clean up the bitmap and buffer unless you still need it
	delete[] pixels; // delete the array of objects
	return data;
}

void GPUCreatePicture(int width, int height) {

	VWIDTH = width;
	VHEIGHT = height;

	if (picture)destroy_bitmap(picture);
	if (backup)destroy_bitmap(backup);
	if (vram)destroy_bitmap(vram);
	if (ovram)destroy_bitmap(ovram);
	if (offram)destroy_bitmap(offram);
	if (spare)destroy_bitmap(spare);

	// clear undo levels
	for (BITMAP * b : undo) {
		if (b)destroy_bitmap(b);
		b = nullptr;
	}

	undo.clear();

	// Create picture page
	picture = create_bitmap(width, height);
	if (!picture)panic();
	clear_to_color(picture, 15);

	// Create picture page
	backup = create_bitmap(width, height);
	if (!backup)panic();
	clear_to_color(backup, 15);

	// Create canvas 
	vram = create_bitmap(width, height);
	if (!vram)panic();
	clear_to_color(vram, white);

	// Create overlay canvas 
	ovram = create_bitmap(width, height);
	if (!ovram)panic();
	clear_to_color(ovram, pink);

	// Create offscreen canvas 
	offram = create_bitmap(width, height);
	if (!offram)panic();
	clear_to_color(offram, pink);

	// Create spare page
	spare = create_bitmap(width, height);
	if (!spare)panic();
	clear_to_color(spare, 15);

	// Create undo levels
	for (int i = 0; i < UNDO_LEVELS * 2; i++) {
		BITMAP *b = create_bitmap(width, height);
		undo.push_back(b);
		if (!b)panic();
		clear_to_color(b, 15);
	}

	if (PaintDX::dxpainting) delete PaintDX::dxpainting;
	PaintDX::dxpainting = new DXPainting();

	PaintDX::dxpainting->create(width, height, 1, 60);
		
	initializePalette();
	updatePalette();

	GPUSetScroll(0, 0); 
	GPUSetZoom(0);	

	std::string caption = std::string("PaintDX - *");
	SetWindowTextA(PaintDX::handle, caption.c_str());
}

#include "resource.h"
void GPUInitialize(struct HWND__ *hnd) {
	memset(active_subtool, 0, sizeof(int) * 22);
	memset(pattern, 0, sizeof(BITMAP*) * 32);
	
	allegro_init();
	
	install_timer();
	

	// Set zoom vars
	GPUSetZoom(1);
	GPUSetScroll(0, 0);
	
	oldhnd = win_get_window();
	win_set_window(hnd);
	set_display_switch_mode(SWITCH_BACKGROUND);
	set_color_depth(32);
	white = makecol(255, 255, 255);
	pink = makecol(255, 0, 255);

	// Create gradient output (tool) bitmaps
	gradients = create_bitmap(32 * 16, 16);
	if (!gradients)panic();
	clear_to_color(gradients, white);

	// Create spray radius bitmap
	sprays = create_bitmap(8 * 16, 16);
	if (!sprays)panic();
	clear_to_color(sprays, pink);
	for (int i = 0; i < 8; i++) {
		int x = (i * 16)+8;
		int y = 8;
		circlefill(sprays, x, y, i, 0);
	}

	// Create dash bitmaps
	oddeven = create_bitmap(2, 2);
	if (!oddeven)panic();
	clear_to_color(oddeven, pink);
	putpixel(oddeven, 0, 0, 0);
	putpixel(oddeven, 1, 1, 0);
		
	brush_point_callback[T_BRUSH] = pointBrush;
	brush_point_callback[T_DASH] = pointBrush;
	brush_point_callback[T_LINE] = pointBrush;
	brush_point_callback[T_BEZIER] = pointBrush;
	brush_point_callback[T_SQUARE] = pointBrush;
	brush_point_callback[T_CIRCLE] = pointBrush;
	brush_point_callback[T_POLY] = pointBrush;
	brush_point_callback[T_SHAPE] = pointBrush;
	brush_point_callback[T_FLOOD] = pointFlood;
	brush_point_callback[T_SPRAY] = pointSpray;
	brush_point_callback[T_COPY] = pointCopy;
	brush_point_callback[T_TEXT] = pointText;
	brush_point_callback[T_GRID] = pointBrush;
	brush_point_callback[T_PERSPECTIVE] = pointBrush;
	brush_point_callback[T_ZOOM] = pointZoom;
	brush_point_callback[T_HAND] = pointHand;
	brush_point_callback[T_PIPE] = pointPipe;
	brush_point_callback[T_SYMMETRY] = pointBrush;
	brush_point_callback[T_UNDO] = pointBrush;
	brush_point_callback[T_CLEAR] = pointBrush;
	brush_point_callback[T_GRADIENT] = pointBrush;
	brush_point_callback[T_PATTERNS] = pointBrush;
	

	buttons = load(IDB_BUTTONS);
	brushes = load(IDB_BRUSHES);
	dashmodes = load(IDB_DASHMODES);
	patterns = load(IDB_PATTERNS);


	for (int i = 0; i < 32; i++) {
		pattern[i] = create_bitmap(16, 16);
		if (!pattern[i])panic();
		blit(patterns, pattern[i], i * 16, 0, 0, 0, 16, 16);
	}

	if (!buttons)panic();
	if (!brushes)panic();
	if (!dashmodes)panic();

	tool_menus[T_BRUSH		] = { 5, 4, 8, 8,	brushes		};//brushes
	tool_menus[T_DASH		] = { 3, 1, 22,22,	dashmodes	};//dashmodes
	tool_menus[T_LINE		] = { 0, 0, 16,16,	nullptr		};//line mode
	tool_menus[T_BEZIER		] = { 0, 0, 16,16,	nullptr		};//curve mode
	tool_menus[T_SQUARE		] = { 0, 0, 16,16,	nullptr		};//fill rect
	tool_menus[T_CIRCLE		] = { 0, 0, 16,16,	nullptr		};//fill circle
	tool_menus[T_POLY		] = { 0, 0, 16,16,	nullptr		};//polygon fill
	tool_menus[T_SHAPE		] = { 0, 0, 16,16,	nullptr		};//freeshape filll
	tool_menus[T_FLOOD		] = { 0, 0, 16,16,	nullptr		};//flood fill
	tool_menus[T_SPRAY		] = { 8, 1, 16,16,	sprays		};//spray
	tool_menus[T_COPY		] = { 0, 0, 16,16,	nullptr		};//copy rectangle
	tool_menus[T_TEXT		] = { 0, 0, 16,16,	nullptr		};//text tool
	tool_menus[T_GRID		] = { 0, 0, 16,16,	nullptr		};//toggle grid
	tool_menus[T_PERSPECTIVE] = { 0, 0, 16,16,	nullptr		};//perspective tool
	tool_menus[T_ZOOM		] = { 0, 0, 16,16,	nullptr		};//zoom
	tool_menus[T_HAND		] = { 0, 0, 16,16,	nullptr		};//hand tool
	tool_menus[T_PIPE		] = { 0, 0, 16,16,	nullptr		};//colorpipe
	tool_menus[T_SYMMETRY	] = { 0, 0, 16,16,	nullptr		};//symmetry
	tool_menus[T_UNDO		] = { 0, 0, 16,16,	nullptr		};//undo
	tool_menus[T_CLEAR		] = { 0, 0, 16,16,	nullptr		};//clear
	tool_menus[T_GRADIENT	] = { 1,16, 32,16,	gradients	};//gradient
	tool_menus[T_PATTERNS	] = {16, 2, 16,16,  patterns 	};//patterns

	GPUCreatePicture(320, 240);

	PaintDX::setCycleRate(0);
}

#include <stdarg.h>
#include <cstdio>
void GPUPrint(int x, int y, const char *format, ...) {
	char buffer[1024];
	va_list ap;
	va_start(ap, format);
	vsprintf_s(buffer, format, ap);
	va_end(ap);
	set_window_title(buffer);
	textprintf(ui_vram, font, x, y, 0, "%s", buffer);
}

void pictureNew() {
	if (MessageBoxA(NULL, "This will erase picture and palette contents", "Warning", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK) {
		GPUCreatePicture(VWIDTH, VHEIGHT);
		PaintDX::redraw();
	}
}
void pictureBackup() {
	blit(picture, backup, 0, 0, 0, 0, backup->w, backup->h);
}
void pictureRevert() {
	blit(backup, picture, 0, 0, 0,0,backup->w, backup->h);
	PaintDX::redraw();
}

#include <map>
void convertBitmap(BITMAP *bmp) {
	std::map<int, int> colors_in;
	PALETTE	colors_out;
	int i = 0;
	for (int y = 0; y < bmp->h; y++) {
		for (int x = 0; x < bmp->w; x++) {
			int c = getpixel(bmp, x, y);
			if (colors_in.find(c) != colors_in.end())continue;
			colors_in[c] = i;
			i++;
		}
	}

	if (colors_in.size() > 256) {
		std::vector<int> reds;
		std::vector<int> greens;
		std::vector<int> blues;
		std::vector<int> neutrals;
		std::map<int, int>::iterator pit = colors_in.begin();
		int r, g, b;
		
		// Separate r,g,b and neutral colors
		while(pit != colors_in.end()) {
			r = getr(pit->first);
			g = getg(pit->first);
			b = getb(pit->first);
			if ((r > g) && (g >= b)) reds.push_back(pit->first);
			else if ((g > r) && (r >= b)) greens.push_back(pit->first);
			else if ((b > g) && (g >= r)) blues.push_back(pit->first);
			else neutrals.push_back(pit->first);
			pit++;
		}

		// sort each channel
		std::vector<int> rsort;
		std::vector<int> gsort;
		std::vector<int> bsort;
		std::vector<int> nsort;

		for (int i = 0; i < 256; i++) {
			for (int e : reds) {
				if (getr(e) == i)rsort.push_back(e);
			}
		}
		for (int i = 0; i < 256; i++) {
			for (int e : greens) {
				if (getg(e) == i)gsort.push_back(e);
			}
		}
		for (int i = 0; i < 256; i++) {
			for (int e : blues) {
				if (getb(e) == i)bsort.push_back(e);
			}
		}
		for (int i = 0; i < 256; i++) {
			for (int e : neutrals) {
				if((getr(e) == i)
				&&(getg(e) == i)
				&&(getb(e) == i))nsort.push_back(e);
			}
		}

		reds = rsort;
		greens = gsort;
		blues = bsort;
		neutrals = nsort;

		// Lets make some numbers..
		size_t total = reds.size() + greens.size() + blues.size() + neutrals.size();
		float pcr = float(reds.size()) / float(total);
		float pcg = float(greens.size()) / float(total);
		float pcb = float(blues.size()) / float(total);
		float pcn = float(neutrals.size()) / float(total);
		
		// Make a rule of three to solve how much colors should have each channel
		int rcount = int(pcr * 256.0f);
		int gcount = int(pcg * 256.0f);
		int bcount = int(pcb * 256.0f);
		int ncount = int(pcn * 256.0f);

		// Calculate stepness 
		float rstep = float(reds.size()) / float(rcount);
		float gstep = float(greens.size()) / float(gcount);
		float bstep = float(blues.size()) / float(bcount);
		float nstep = float(neutrals.size()) / float(ncount);

		// now collect them again, but skipping colors inbetween
		rsort.clear();
		gsort.clear();
		bsort.clear();
		nsort.clear();

		for (float i = 0.0f; i < float(reds.size()); i += rstep) {
			rsort.push_back(reds[int(i)]);
		}
		for (float i = 0.0f; i < float(greens.size()); i += gstep) {
			gsort.push_back(greens[int(i)]);
		}
		for (float i = 0.0f; i < float(blues.size()); i += bstep) {
			bsort.push_back(blues[int(i)]);
		}
		for (float i = 0.0f; i < float(neutrals.size()); i += nstep) {
			nsort.push_back(neutrals[int(i)]);
		}

		int o = 0;
		for (int i : rsort) { if (o > 255) continue; colors_out[o].r = getr(i); colors_out[o].g = getg(i); colors_out[o].b = getb(i); o++; }
		for (int i : gsort) { if (o > 255) continue; colors_out[o].r = getr(i); colors_out[o].g = getg(i); colors_out[o].b = getb(i); o++; }
		for (int i : bsort) { if (o > 255) continue; colors_out[o].r = getr(i); colors_out[o].g = getg(i); colors_out[o].b = getb(i); o++; }
		for (int i : nsort) { if (o > 255) continue; colors_out[o].r = getr(i); colors_out[o].g = getg(i); colors_out[o].b = getb(i); o++; }
		
		// Finally paint converting colors
		for (int y = 0; y < bmp->h; y++) {
			for (int x = 0; x < bmp->w; x++) {
				int c = getpixel(bmp, x, y);
				putpixel(bmp, x, y, bestfit_color(colors_out, getr(c), getg(c), getb(c)));
			}
		}
		int i = 0;
		for(RGB r : colors_out) {
			PaintDX::palette[i] = makecol(r.r, r.g, r.b);
			i++;
		}
	} else {
		// If palette is small enough, we can just add the colors and modify the palette
		for (int y = 0; y < bmp->h; y++) {
			for (int x = 0; x < bmp->w; x++) {
				putpixel(bmp, x, y, colors_in[getpixel(bmp, x, y)]);
			}
		}
		auto pit = colors_in.begin();
		while (pit != colors_in.end()) {
			PaintDX::palette[pit->second] = pit->first;
			pit++;
		}
	}
}



typedef enum E_SupportedFormat {
	SF_BMP,
	SF_PDX,
	SF_COUNT
}SupportedFormat;

void pictureSaveAs();

void pictureSave() {
	if (PaintDX::dxpainting->name.compare("")) {
		memcpy(PaintDX::dxpainting->palette, PaintDX::palette, sizeof(int) * 256);
		PaintDX::dxpainting->save();
		PaintDX::redraw();
	} else {
		pictureSaveAs();
	}
}

void pictureSaveAs() {
	OPENFILENAMEA ofn;       // common dialog box structure
	char szFile[260] = { 0 };       // if using TCHAR macros

									// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = PaintDX::handle;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Paint Deluxe Files\0*.PDX\0Bitmap Files\0*.BMP\0All\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetSaveFileNameA(&ofn)) {
		std::string name = ofn.lpstrFile;
		std::string extension = name.substr(name.find_last_of(".") + 1);

		SupportedFormat f;

		if (!extension.compare("bmp"))f = SF_BMP;
		else if (!extension.compare("BMP"))f = SF_BMP;
		else if (!extension.compare("pdx"))f = SF_PDX;
		else if (!extension.compare("PDX"))f = SF_PDX;
		else f = SF_PDX;
		
		PaintDX::dxpainting->name = name;

		if(f==SF_PDX)pictureSave();		
		else PaintDX::dxpainting->saveBitmap(name);		
	}
}

void pictureLoad() {
	OPENFILENAMEA ofn;       // common dialog box structure
	char szFile[260] = { 0 };       // if using TCHAR macros

									 // Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = PaintDX::handle;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Paint Deluxe Files\0*.PDX\0Bitmap Files\0*.BMP\0All\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileNameA(&ofn))	{
		std::string name = ofn.lpstrFile;
		std::string extension = name.substr(name.find_last_of(".") + 1);

		SupportedFormat f = SF_PDX;
		if (!extension.compare("bmp"))f = SF_BMP;
		else if (!extension.compare("BMP"))f = SF_BMP;
		else if (!extension.compare("pdx"))f = SF_PDX;
		else if (!extension.compare("PDX"))f = SF_PDX;
		else f = SF_PDX;

		switch (f) {
				
			case SF_BMP:
				BITMAP * b;
				b = load_bitmap(name.c_str(), nullptr);
				if (!b) {
					panic();
					break;
				}
				GPUCreatePicture(b->w, b->h);
				convertBitmap(b);
				blit(b, picture, 0, 0, 0, 0, b->w, b->h);
				GPUSyncPainting();
				break;
		
			case SF_PDX:
				if (PaintDX::dxpainting)delete PaintDX::dxpainting;
				PaintDX::dxpainting = new DXPainting();


				if (!PaintDX::dxpainting->load(name)) {
					allegro_message("ERROR: Could not load '%s'", name.c_str());
					return;
				}

				PaintDX::dxpainting->current_frame = -1;
				PaintDX::dxpainting->selectFrame(0);

				PaintDX::dxpainting->updatePalette();
				PaintDX::dxpainting->updateGradients();
				PaintDX::dxpainting->updatePicture();

				GPUSyncPainting();
				break;

			default:
				allegro_message("ERROR: '%s' is not a supported file type", extension.c_str());
				break;
		}
		PaintDX::redraw();
	}
}
void GPUClean() {
	clear_to_color(ovram, makecol(255, 0, 255));
	GPUSetColor(active_color);
}

void normalizeMouseRect() {
	int minx = __min(click_x, unclick_x);
	int miny = __min(click_y, unclick_y);
	int maxx = __max(click_x, unclick_x);
	int maxy = __max(click_y, unclick_y);
	click_x = minx;
	click_y = miny;
	unclick_x = maxx;
	unclick_y = maxy;
}

void centerDialog(void *p){
	HWND dlg = (HWND)p;
	RECT r, rp;
	GetClientRect((HWND)PaintDX::handle, &rp);
	GetClientRect(dlg, &r);
	int w = r.right - r.left;
	int h = r.bottom - r.top;
	int x = ((rp.right - rp.left) / 2) - (w / 2);
	int y = ((rp.bottom - rp.top) / 2) - (h / 2);
	SetWindowPos(dlg, 0, x, y, w, h, true);
}


void GPUReplaceColor(BITMAP *bmp, int o, int r) {
	for (int y = 0; y < bmp->h; y++) {
		for (int x = 0; x < bmp->w; x++) {
			if (getpixel(bmp, x, y) == o)putpixel(bmp, x, y, r);
		}
	}
}

void GPURemap(BITMAP *bmp, int sx, int sy, int dx, int dy) {
	for (int y = sy; y < dy; y++) {
		for (int x = sx; x < dx; x++) {
			int c = getpixel(bmp, x, y);
			if (c > 255)continue;
			putpixel(bmp, x, y, PaintDX::palette[c]);
		}
	}
}


