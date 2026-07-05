#include <list>
#include "paintdx.hpp"
#include <allegro.h>
#include <winalleg.h>
#include <vector>
#include <deque>
#include "enum.h"
#include <string>
#include "tools.hpp"

extern void GPUClean();
extern BITMAP *picture, *ovram, *vram;
extern int click_x, click_y, unclick_x, unclick_y, active_color, pasive_color;
extern float window_x, window_y, window_w, window_h, scroll_x, scroll_y;

extern void lockMouse(int, int);
extern void overlayRectangle();
extern void overlayLine();
extern void normalizeMouseRect();
extern void pointBrush(BITMAP *bmp, int x, int y, int c);
extern void pointPipe(BITMAP *bmp, int x, int y, int c);
extern void pointHand(BITMAP *bmp, int x, int y, int c);
extern void GPUSyncPainting();
extern void fillShape();
extern void drawShape(bool);
extern void prepareShape(int, int);
extern void prepareStroke(int, int);
extern void pencil(int, int);
extern void GPUPaint(int, int);
extern std::vector<Point> shape;
extern void GPUSetZoom(int);
extern void GPUSetColor(int);
extern void GPURectangleFill(BITMAP *b, int,int,int,int);
extern int pink;
extern bool click_lock;
extern int zoom;
extern BITMAP *pattern[32];
extern int active_subtool[22];
//****************************************************************************/
// Tool callbacks
//****************************************************************************/



extern void openDialogGradient();
extern void openDialogGrid();
extern void openDialogPerspective();
extern void openDialogSymmetry();
extern void openDialogPalette();
extern void openDialogFonts();
extern void openDialogFill();
extern void openDialogBezier();
extern void openDialogPatterns();

// Opens dialog for given index, if existing (called on mouse2 click at toolbox)
void GPUSetupTool(int index) {
	switch ((ToolType)index) {
	case T_BEZIER:		openDialogBezier(); break;
	case T_SQUARE:		openDialogFill(); break;
	case T_CIRCLE:		openDialogFill(); break;
	case T_POLY:		openDialogFill(); break;
	case T_SHAPE:		openDialogFill(); break;
	case T_TEXT:		openDialogFonts(); break;
	case T_GRID:		openDialogGrid(); break;
	case T_PERSPECTIVE:	openDialogPerspective(); break;
	case T_SYMMETRY:	openDialogSymmetry(); break;
	case T_GRADIENT:	openDialogGradient(); break;
	case T_PATTERNS:	openDialogPatterns(); break;

		// no options to set up
	default:
	case T_CLEAR:
	case T_ZOOM:
	case T_HAND:
	case T_PIPE:
	case T_UNDO:
	case T_FLOOD:
	case T_SPRAY:
		return;
	}
}
void toolPipe(bool keyup, int x, int y) {
	if (keyup) {
		normalizeMouseRect();
		if (click_x == unclick_x)
			if (click_y == unclick_y) {
				// do nothing
				return;
			}
		// find average tone
	}
	else {
		lockMouse(x, y);
		overlayRectangle();
		pointPipe(picture, x, y, PaintDX::mouseaux);		
	}
	PaintDX::redraw();
}

void toolLine(bool keyup, int x, int y) {
	if (keyup) {
		do_line(ovram, click_x, click_y, unclick_x, unclick_y, active_color, pointBrush);
		masked_blit(ovram, picture, 0, 0, 0, 0, ovram->w, ovram->h);
		GPUClean();
		//GPUPaint(x, y);
	}
	else {
		lockMouse(x, y);
		overlayLine();
		if (PaintDX::enable_fast_feedback) {
			do_line(ovram, click_x, click_y, unclick_x, unclick_y, PaintDX::palette[active_color], pointBrush);
		}
	}
	PaintDX::redraw();
}

void toolSquare(bool keyup, int x, int y) {
	if (keyup) {
		normalizeMouseRect();
		for (int h = click_y; h < unclick_y; h++)
			do_line(ovram, click_x, h, unclick_x, h, active_color, pointBrush);
		masked_blit(ovram, picture, 0, 0, 0, 0, ovram->w, ovram->h);
		GPUClean();
		//GPUPaint(x, y);
	}
	else {
		lockMouse(x, y);
		overlayRectangle();
		if (PaintDX::enable_fast_feedback) {
			GPURectangleFill(ovram, click_x, click_y, unclick_x, unclick_y);
		}
	}
	PaintDX::redraw();
}

ToolType last_tool;
void GPUSetTool(int index);

void toolCopy(bool keyup, int x, int y) {
	if (keyup) {
		clear_to_color(ovram, pink);
		normalizeMouseRect();
		if (PaintDX::mouseclick) {
			//capture content to custom brush
			PaintDX::brush.define(click_x, click_y, unclick_x, unclick_y);
			
		}
		GPUSetTool(last_tool);
	}
	else {
		if (PaintDX::mouseaux) {
			// dispose brush
			PaintDX::brush.dispose();
		}
		else {
			lockMouse(x, y);
			overlayRectangle();
		}
	}
	PaintDX::redraw();
}

void toolHand(bool keyup, int x, int y) {
	if (keyup) {
		clear_to_color(ovram, pink);
		pointHand(nullptr, 0, 0, 0);
	}
	else {
		lockMouse(x, y);
		clear_to_color(ovram, pink);
		do_line(ovram, click_x, click_y, x, y, makecol(255, 0, 0), putpixel);
	}

	PaintDX::redraw();
}

void toolFlood(bool keyup, int x, int y) {
	if (keyup) {
		pencil(unclick_x + window_x, unclick_y + window_y);
		GPUSyncPainting();
	}
	else {
		lockMouse(x, y);
		clear_to_color(ovram, pink);
		do_line(ovram, click_x, click_y, x, y, makecol(255, 0, 0), putpixel);
	}
	PaintDX::redraw();
}

void toolZoom(bool keyup, int x, int y) {
	if (keyup) {
		clear_to_color(ovram, pink);
		if (PaintDX::mouseclick) {
			GPUSetZoom(zoom + 1);
			click_x = window_x;
			click_y = window_y;
			unclick_x /= (zoom + 1);
			unclick_y /= (zoom + 1);
			pointHand(nullptr, 0, 0, 0);
		}
	}
	else {
		if (PaintDX::mouseaux) {
			GPUSetZoom(zoom>0 ? zoom - 1 : 0);
		}
		else {
			lockMouse(x, y);
			overlayRectangle();
		}
	}
	PaintDX::redraw();
}

void toolCircle(bool keyup, int x, int y) {
	if (keyup) {
		// Draw fill
		solid_mode();
		ellipsefill(ovram, click_x, click_y, abs(click_x - unclick_x), abs(click_y - unclick_y), pasive_color);
		floodfill(ovram, click_x, click_y, makecol(255, 0, 255));
		ellipse(ovram, click_x, click_y, abs(click_x - unclick_x), abs(click_y - unclick_y), pasive_color);
		drawing_mode(DRAW_MODE_MASKED_PATTERN, pattern[active_subtool[T_PATTERNS]], click_x, click_y);
		floodfill(ovram, click_x, click_y, pasive_color);
		solid_mode();

		// Draw perimeter
		//do_ellipse(ovram, click_x, click_y, abs(click_x - unclick_x), abs(click_y - unclick_y), active_color, pointBrush);

		// Copy to picture
		masked_blit(ovram, picture, 0, 0, 0, 0, ovram->w, ovram->h);
		clear_to_color(ovram, makecol(255, 0, 255));
		GPUSetColor(active_color);
		GPUPaint(-1, -1);
	}
	else {
		prepareStroke(x, y);
		do_ellipse(ovram, click_x, click_y, abs(click_x - unclick_x), abs(click_y - unclick_y), PaintDX::palette[active_color], pointBrush);		
	}
	PaintDX::redraw();
}

void toolShape(bool keyup, int x, int y) {
	if (keyup) {
		drawShape(true);
		fillShape();
		masked_blit(ovram, picture, 0, 0, 0, 0, ovram->w, ovram->h);
		clear_to_color(ovram, makecol(255, 0, 255));
		GPUSetColor(active_color);
		shape.clear();
		click_lock = false;
		PaintDX::redraw();
	}
	else {
		prepareShape(x, y);
		drawShape(false);
		PaintDX::redraw();
	}
	PaintDX::redraw();
}

void toolBrush(bool keyup, int x, int y) {
	if (keyup) {

	}
	else {
		//lockMouse(x, y);
		pencil(x, y);
	}
	PaintDX::redraw();
}

void toolPerspective(bool keyup, int x, int y) {
	if (keyup) {
	}
	else {
	}
	PaintDX::redraw();
}

void toolBezier(bool keyup, int x, int y) {
	if (keyup) {
	}
	else {
	}
	PaintDX::redraw();
}

void toolText(bool keyup, int x, int y) {
	if (keyup) {
	}
	else {
	}
	PaintDX::redraw();
}

void toolSpray(bool keyup, int x, int y) {
	if (keyup) {
	}
	else {
		lockMouse(x, y);
		pencil(x, y);
	}
	PaintDX::redraw();
}

void toolPoly(bool keyup, int x, int y) {
	if (keyup) {
		if (PaintDX::mouseaux) {
			drawShape(true);
			fillShape();
			masked_blit(ovram, picture, 0, 0, 0, 0, ovram->w, ovram->h);
			clear_to_color(ovram, makecol(255, 0, 255));
			GPUSetColor(active_color);
			shape.clear();			
		}
		click_lock = false;
	}
	else {
		if (!click_lock) {
			lockMouse(x, y);
			prepareShape(x, y);
			drawShape(false);			
		}
	}
	PaintDX::redraw();
}