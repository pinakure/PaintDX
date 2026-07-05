#include "paintdx.hpp"
#include <windows.h>

void			*PaintDX::handle = nullptr;
bool			PaintDX::mouseclick = false;
bool			PaintDX::mousemid = false;
bool			PaintDX::mouseaux = false;

bool			PaintDX::enable_grid=false;
int				PaintDX::grid_origin_x = 0;
int				PaintDX::grid_origin_y= 0;
int				PaintDX::grid_width=16;
int				PaintDX::grid_height=16;
ColorMode		PaintDX::color_mode = CM_SINGLECOLOR;
bool			PaintDX::enable_symmetry = false;
bool			PaintDX::enable_multicycle = false;//TECHNIQUE
bool			PaintDX::enable_cycle = false;//TAB
bool			PaintDX::enable_stencil=false;
bool			PaintDX::enable_lockforeground=false;
bool			PaintDX::enable_background_free=false;//if false, background is fixed (the other choice)
bool			PaintDX::font_plain=false;
bool			PaintDX::font_bold=false;
bool			PaintDX::font_underline=false;
bool			PaintDX::font_italic=false;
int				PaintDX::font_size=8;
bool			PaintDX::enable_infobar=false;
bool			PaintDX::enable_infobar_coordinates=false;
bool			PaintDX::enable_infobar_pixels=false;
bool			PaintDX::enable_antialias=false;
bool			PaintDX::enable_colorize=false;
bool			PaintDX::enable_translucent=false;
bool			PaintDX::enable_fast_feedback=false;
bool			PaintDX::enable_square_aspect=false;
bool			PaintDX::enable_video_patterns=false;
bool			PaintDX::enable_halftones=false;
bool			PaintDX::enable_brush_handle=false;
bool			PaintDX::enable_exclude_edge=false;
Measurement		PaintDX::measurement = MT_INCHES;
Brush			PaintDX::brush;
bool			PaintDX::custom_brush = false;


#include <string>

extern std::string infobar_text; 

extern void GPUDeinitialize();
extern void GPUInitialize(struct HWND__ *hnd);
extern void GPURaster(struct HDC__ *hdc, int width, int height);
extern void GPUPaint(int x, int y);
extern void GPUPrint(int x, int y, const char *format, ...);
extern void GPUPeek(int x, int y);
extern void GPUSetColor(int index);
extern void GPUSetBackColor(int index);
extern void GPUSetTool(int index);
extern void GPUSetupTool(int index);
extern void GPUSetSubTool(int index);
extern void GPUSaveState();
extern void GPUReleaseMouse(int x, int y);

extern int click_x, click_y;
extern int unclick_x, unclick_y;

extern bool click_lock;

extern bool menu_tool;
extern int active_tool;
extern int active_subtool[22];

extern Menu tool_menus[22];

void* menu_help;
void* menu_picture;
void* menu_brush;
void* menu_techniques;
void* menu_stencil;
void* menu_font;
void* menu_misc;
void* menu_preferences;

static bool canvas = false;

void PaintDX::raster(void *hdc, int width, int height) {
	GPURaster((struct HDC__*)hdc, width, height);
}



void PaintDX::updatePalette(int x, int y, int height) {
	int colors_per_row = 256 / 6;
	int color_height = height / colors_per_row;

	int rows = height / color_height;
	int row = y / color_height;
	int col = x / COLOR_WIDTH;
	int index = (col*rows) + row;
	infobar_text = "Color " + std::to_string(index) + " selected";
	if (mouseclick)GPUSetColor(index);
	if (mouseaux)GPUSetBackColor(index);

	RECT r;
	GetClientRect((HWND)handle, &r);
	r.left = r.right - (TOOLBAR_WIDTH);
	r.top = r.bottom - (height + BUTTON_EXTRA);
	InvalidateRect((HWND)handle, &r, false);
	SendMessage((HWND)handle, WM_SIZE, 0, 0);
	SendMessage((HWND)handle, WM_PAINT, 0, 0);
}

void PaintDX::redraw() {
	RECT r;
	GetClientRect((HWND)handle, &r);
	InvalidateRect((HWND)handle, &r, false);
	SendMessage((HWND)PaintDX::handle, WM_PAINT, 0, 0);
	updateMainMenu();
}

static int memo_last_speed = 0;
extern int current_gradient;

void PaintDX::setCycleRate(int speed) {
	if (speed == memo_last_speed)return;
	if (speed == 0) {
		if (memo_last_speed != 0)KillTimer((HWND)PaintDX::handle, current_gradient);
		memo_last_speed = 0;
		return;
	}
	else {
		if (memo_last_speed != 0)KillTimer((HWND)PaintDX::handle, current_gradient);
		SetTimer((HWND)PaintDX::handle, current_gradient, 8 * speed, nullptr);
		memo_last_speed = speed;
	}
}

void PaintDX::updateMenu(int x, int y) {
	int mx = x;
	int my = y;
	RECT r;
	GetClientRect((HWND)PaintDX::handle, &r);
	int width = r.right - r.left;

	y = active_tool % 11;
	x = active_tool >= 11 ? 1 : 0;
	x *= BUTTON_WIDTH;
	y *= BUTTON_HEIGHT;
	x += width - TOOLBAR_WIDTH;
	y += BUTTON_HEIGHT;

	int size_x = tool_menus[active_tool].item_width * tool_menus[active_tool].columns;
	int size_y = tool_menus[active_tool].item_height * tool_menus[active_tool].rows;

	int sx = x - (size_x);
	int sy = y - (size_y / 2);
	int dx = sx + (size_x);
	int dy = sy + (size_y);
	my -= sy;
	mx -= x;
	mx +=(size_x);
	
	if ((mx >=0) && (my >= 0) && (mx < size_x) && (my < size_y)) {
		//Menu hover
		int cell_w = size_x / tool_menus[active_tool].columns;
		int cell_h = size_y / tool_menus[active_tool].rows;

		int cols = size_x / cell_w;
		int rows = size_y / cell_h;
		int row = (my/cell_h);
		int col = (mx/cell_w);
		GPUSetSubTool((row * tool_menus[active_tool].columns) + col);
		r.left = sx;
		r.right = dx + 4;
		r.top = sy;
		r.bottom = dy + 4;
		InvalidateRect((HWND)PaintDX::handle, &r, false);
		UpdateWindow((HWND)handle);
	}

}

void PaintDX::updateButtons(int x, int y, int width, int height) {
	
	int mx = x;
	int my = y;

	if (y >= 11 * BUTTON_HEIGHT) {
		y -= 11 * BUTTON_HEIGHT;
		height -= 11 * BUTTON_HEIGHT;
		if (y > BUTTON_EXTRA) {
			y -= BUTTON_EXTRA;
			height -= BUTTON_EXTRA;
			PaintDX::updatePalette(x, y, height);
		}
		else {
			//handle middle area clicks
		}
	}
	else {
		// handle button clicks
		int row = y / BUTTON_HEIGHT;
		int col = x / BUTTON_WIDTH;
		int index = (col*BUTTON_ROWS) + row;
		
		if (mouseaux) {
			GPUSetupTool(index);
			mouseaux = false;
		} else {
			if (menu_tool) updateMenu(mx + width, my);
			else GPUSetTool(index);
		}
		
		RECT r;
		GetClientRect((HWND)handle, &r);
		r.left = TOOLBAR_WIDTH;
		r.bottom = TOOLBAR_HEIGHT + (tool_menus[active_tool].item_height*tool_menus[active_tool].rows);
		
		InvalidateRect((HWND)handle, &r, false);
		SendMessage((HWND)handle, WM_PAINT, 0, 0);
	}
}

extern float scale_x;
extern float scale_y;
extern float window_x;
extern float window_y;

extern int cursor_x;
extern int cursor_y;


extern int zoom;

void PaintDX::updateMouse(int x, int y) {	

	int mx = x;
	int my = y;

	RECT r;
	GetClientRect((HWND)handle, &r);
	int rw, rh;
	int width = r.right - r.left;
	int height = r.bottom - r.top;
	float xratio = float((width-TOOLBAR_WIDTH)*(zoom + 1)) / float(VWIDTH*(zoom + 1));
	float yratio = float(height*(zoom + 1)) / float(VHEIGHT*(zoom + 1));
	width -= TOOLBAR_WIDTH;
	rw = int(float(mx) / float(xratio));
	rh = int(float(my) / float(yratio));
	cursor_x = rw;
	cursor_y = rh;

	if (mouseclick || mouseaux) {
		

		if (mx > width) {
			//send to palette / button handler
			mx -= width;
			updateButtons(mx, my, width, height);
			canvas = true;
			click_lock = false;
			return;
		}		
		
		rw = int(float(rw)*scale_x);
		rh = int(float(rh)*scale_y);
		rw += int(window_x);
		rh += int(window_y);
	
		if (enable_grid) {
			rw-= grid_origin_x;
			rh -= grid_origin_y;
			rw -= rw % grid_width;
			rh -= rh % grid_height;
		}
		unclick_x = rw;
		unclick_y = rh;
		canvas = true;

		if ((mouseclick || mouseaux) && !menu_tool) {
			GPUPaint(rw, rh);
		}
		else if (menu_tool)updateMenu(mx, my);

		r.right = width;
		r.bottom = height;
		
	}
}

void PaintDX::leftClickDown(int x, int y) {
	PaintDX::mouseclick = true;
	PaintDX::updateMouse(x, y);	
}
void PaintDX::rightClickDown(int x, int y) {
	PaintDX::mouseaux = true;
	PaintDX::updateMouse(x, y);
}

void PaintDX::middleClickDown(int x, int y) {
	x = int(float(x)*scale_x);
	y = int(float(y)*scale_y);

}

void PaintDX::leftClickUp(int x, int y) {
	int mx = x;
	int my = y;
	RECT r;

	if (menu_tool) {
		
		GetClientRect((HWND)PaintDX::handle, &r);
		
		my = active_tool % 11;
		mx = active_tool >= 11 ? 1 : 0;
		mx *= BUTTON_WIDTH;
		my *= BUTTON_HEIGHT;
		mx += r.right - TOOLBAR_WIDTH;
		my += BUTTON_HEIGHT;

		int size_x = tool_menus[active_tool].item_width * tool_menus[active_tool].columns;
		int size_y = tool_menus[active_tool].item_height * tool_menus[active_tool].rows;

		int sx = mx - (size_x);
		int sy = my - (size_y / 2);
		int dx = sx + (size_x);
		int dy = sy + (size_y);
		r.left = sx;
		r.right = dx + 4;
		r.top = sy;
		r.bottom = dy + 4;

		InvalidateRect((HWND)PaintDX::handle, &r, false);
	}
	if (canvas) {
		x = int(float(x)*scale_x);
		y = int(float(y)*scale_y);
		GPUReleaseMouse(x,y);
	}
	
	SendMessage((HWND__*)handle, WM_PAINT, 0, 0);
	

	
	PaintDX::mouseclick = false;
}

void PaintDX::middleClickUp(int x, int y) {
	x = int(float(x)*scale_x);
	y = int(float(y)*scale_y);


}

void PaintDX::rightClickUp(int x, int y) {
	GPUReleaseMouse(x, y);	
	PaintDX::mouseaux = false;
}

void PaintDX::loadSettings() {

}

void PaintDX::saveSettings() {

}

void PaintDX::initialize(void *hWnd) {
	for(int i=0; i<16; i++)
		gradients.push_back(Gradient(i*16, (i*16)+15, 0, false, 0));
	
	GPUInitialize((HWND__*)hWnd);
	loadSettings();
	updateMainMenu();
}

void PaintDX::deinitialize(void) {
	saveSettings();
	GPUDeinitialize();
}

