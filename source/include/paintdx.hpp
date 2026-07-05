#ifndef PAINTDX_HPP
#define PAINTDX_HPP

#include "enum.h"
#include "gradient.hpp"
#include "brush.hpp"

class DXPainting;

class PaintDX {
public:
	static ColorMode				color_mode;
	static Measurement				measurement;
	static std::vector<Gradient>	gradients;
	static Brush					brush;
	static bool						custom_brush;
	static int						palette[256];

	static bool				enable_grid;
	static int				grid_origin_x;
	static int				grid_origin_y;
	static int				grid_width;
	static int				grid_height;
	
	static DXPainting		*dxpainting;

	static bool				enable_stencil;
	static bool				enable_lockforeground;
	static bool				enable_background_free;//if false, background is fixed (the other choice)
	static bool				font_plain;
	static bool				font_bold;
	static bool				font_underline;
	static bool				font_italic;
	static int				font_size;
	static bool				enable_infobar;
	static bool				enable_infobar_coordinates;
	static bool				enable_infobar_pixels;
	static bool				enable_antialias;
	static bool				enable_colorize;
	static bool				enable_translucent;
	static bool				enable_fast_feedback;
	static bool				enable_square_aspect;
	static bool				enable_video_patterns;
	static bool				enable_halftones;
	static bool				enable_brush_handle;
	static bool				enable_exclude_edge;

	
	static bool				enable_symmetry;
	static bool				enable_multicycle;
	static bool				enable_cycle;//TAB!

	
	static bool				mouseclick;
	static bool				mousemid;
	static bool				mouseaux;

	static void				initialize(void *hwnd);
	static void				deinitialize(void);
	static void				leftClickDown(int, int);
	static void				middleClickDown(int, int);
	static void				rightClickDown(int, int);
	static void				leftClickUp(int, int);
	static void				middleClickUp(int, int);
	static void				rightClickUp(int, int);

	static void				updateMainMenu();
	static void				updateMenu(int x, int y);
	static void				updateMouse(int x, int y);
	static void				updatePalette(int x, int y, int height);
	static void				updateButtons(int x, int y, int width, int height);

	static void*			loadResource(int id, int &w, int &h);
	static void*			handle;

	static void				raster(void *hdc, int width, int height);

	static void				getRGB(int index, int &r, int &g, int &b);
	static void				setRGB(int index, int r, int g, int b, bool redraw = true);

	static void				redraw();

	static void				loadSettings();
	static void				saveSettings();

	// KEYBOARD callbacks
	static void				moveLeft(bool alt);
	static void				moveRight(bool alt);
	static void				moveDown(bool alt);
	static void				moveUp(bool alt);
	static void				plus(bool alt);
	static void				minus(bool alt);

	static void				updateCycle(int index);
	static void				setCycleRate(int speed);
};

#endif
