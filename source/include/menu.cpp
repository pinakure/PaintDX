#include <list>
#include "paintdx.hpp"
#include <allegro.h>
#include <winalleg.h>
#include <vector>
#include <deque>
#include "enum.h"
#include "resource.h"

extern BITMAP *picture;
extern void GPUSaveState();
extern void GPUUndo();
extern void GPUSyncPainting();

/**************************************** CALLBACKS **************************************/

void pageDelete() {
	GPUSaveState();
	GPUSyncPainting();
	clear_to_color(picture, 15);
	PaintDX::redraw();
}
void pictureUndo() {
	GPUUndo();
}
void setPaintMode(ColorMode paintmode) {
	PaintDX::color_mode = paintmode;
	PaintDX::updateMainMenu();
	PaintDX::redraw();
}
void paintModeToggleMulticycle() {
	PaintDX::enable_multicycle ^= 1;
	PaintDX::updateMainMenu();
	PaintDX::redraw();
}
void fontTogglePlain() {
	PaintDX::font_plain = true;
	PaintDX::font_bold = false;
	PaintDX::font_italic = false;
	PaintDX::font_underline = false;
	PaintDX::updateMainMenu();
	PaintDX::redraw();
}
void fontToggleBold() {
	PaintDX::font_bold ^= true;
	PaintDX::font_plain = (!PaintDX::font_bold) && (!PaintDX::font_italic) && (!PaintDX::font_underline);
	PaintDX::updateMainMenu();
	PaintDX::redraw();
}
void fontToggleItalic() {
	PaintDX::font_italic ^= true;
	PaintDX::font_plain = (!PaintDX::font_bold) && (!PaintDX::font_italic) && (!PaintDX::font_underline);
	PaintDX::updateMainMenu();
	PaintDX::redraw();
}
void fontToggleUnderline() {
	PaintDX::font_underline ^= true;
	PaintDX::font_plain = (!PaintDX::font_bold) && (!PaintDX::font_italic) && (!PaintDX::font_underline);
	PaintDX::updateMainMenu();
	PaintDX::redraw();
}
void fontSetSize(int size) {
	PaintDX::font_size = size;
	PaintDX::updateMainMenu();
	PaintDX::redraw();
}
void toggleCycle() {
	PaintDX::enable_cycle ^= true;
	PaintDX::updateMainMenu();
	PaintDX::redraw();
}
void toggleAntialias() {
	PaintDX::enable_antialias ^= true;
	PaintDX::updateMainMenu();
	PaintDX::redraw();
}
void toggleTranslucent() {
	PaintDX::enable_translucent ^= true;
	PaintDX::updateMainMenu();
	PaintDX::redraw();
}
void toggleColorize() {
	PaintDX::enable_colorize ^= true;
	PaintDX::updateMainMenu();
	PaintDX::redraw();
}
void toggleInfoBar() {
	PaintDX::enable_infobar ^= true;
	PaintDX::updateMainMenu();
	PaintDX::redraw();
}
void toggleInfoBarCoordinates() {
	PaintDX::enable_infobar_coordinates ^= true;
	PaintDX::updateMainMenu();
	PaintDX::redraw();
}
void toggleInfoBarInPixels() {
	PaintDX::enable_infobar_pixels ^= true;
	PaintDX::updateMainMenu();
	PaintDX::redraw();
}
void toggleFastFeedback() {
	PaintDX::enable_fast_feedback ^= true;
	PaintDX::updateMainMenu();
	PaintDX::redraw();
}
void toggleSquareAspect() {
	PaintDX::enable_square_aspect ^= true;
	PaintDX::updateMainMenu();
	PaintDX::redraw();
}
void toggleVideoPatterns() {
	PaintDX::enable_video_patterns ^= true;
	PaintDX::updateMainMenu();
	PaintDX::redraw();
}
void toggleHalfTones() {
	PaintDX::enable_halftones ^= true;
	PaintDX::updateMainMenu();
	PaintDX::redraw();
}
void toggleBrushHandle() {
	PaintDX::enable_brush_handle ^= true;
	PaintDX::updateMainMenu();
	PaintDX::redraw();
}
void toggleExcludeEdge() {
	PaintDX::enable_exclude_edge ^= true;
	PaintDX::updateMainMenu();
	PaintDX::redraw();
}
void toggleStencil() {
	PaintDX::enable_stencil ^= true;
	PaintDX::updateMainMenu();
	PaintDX::redraw();
}
void stencilLockFg() {
	PaintDX::enable_lockforeground ^= true;
	PaintDX::updateMainMenu();
	PaintDX::redraw();
}
void stencilBackgroundFix() {
	PaintDX::enable_background_free = false;
	PaintDX::updateMainMenu();
	PaintDX::redraw();
}
void stencilBackgroundFree() {
	PaintDX::enable_background_free = true;
	PaintDX::updateMainMenu();
	PaintDX::redraw();
}
void setMeasurements(Measurement mt) {
	PaintDX::measurement = mt;
	PaintDX::updateMainMenu();
	PaintDX::redraw();
}


void pageSize() {
	// ??????
}
void pageView() {
	// calculate zoom to fit page
}
void brushRemap() {}
void pictureRemap() {}
void pictureReplaceBgFg() {}
void brushReplaceBgFg() {}
void pictureCopyBgFg() {}
void brushCopyBgFg() {}
void defaultPalette() {}
void restorePalette() {}

void paletteFromBrush() {}

void brushRestore() {
	PaintDX::brush.restore();
}
void brushOutline() {
	PaintDX::brush.outline();
}
void brushHalveHorz() {
	PaintDX::brush.half(true, false);
}
void brushHalveVert() {
	PaintDX::brush.half(false, true);
}
void brushDoubleVert() {
	PaintDX::brush.twice(false, true);
}
void brushDoubleHorz() {
	PaintDX::brush.twice(true, false);
}
void brushStretchHorz() {
	PaintDX::brush.stretch(true, false);
}
void brushStretchVert() {
	PaintDX::brush.stretch(false, true);
}
void brushBendVert() {
	PaintDX::brush.bend(true);
}
void brushBendHorz() {
	PaintDX::brush.bend(false);
}
void brushShearVert() {
	PaintDX::brush.shear(false);
}
void brushShearHorz() {
	PaintDX::brush.shear(true);
}
void brushFlipVert() {
	PaintDX::brush.flip(false);
}
void brushFlipHorz() {
	PaintDX::brush.flip(true);
}
void brushLoad() {
	PaintDX::brush.load();
}
void brushSave() {
	PaintDX::brush.save();
}
void brushRotate(int angle) {
	PaintDX::brush.rotate((BrushAngle)angle);
}

void sparePreferences() {
}
void stencilMake() {
}
void stencilDiscard() {
}
void stencilReverse() {
}
void stencilLoad() {
}
void stencilSave() {
}

static int color_mode_menus[CM_COUNT] = {
	ID_TECHNIQUES_PAINT,
	ID_TECHNIQUES_SINGLECOLOR,
	ID_TECHNIQUES_REPLACE,
	ID_TECHNIQUES_SMEAR,
	ID_TECHNIQUES_SHADE,
	ID_TECHNIQUES_CYCLE,
	ID_TECHNIQUES_SMOOTH,
	ID_TECHNIQUES_MULTI
};

static void setMenuStatus(HMENU menu, int id, bool status) {
	MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
	mii.fMask = MIIM_STATE;
	GetMenuItemInfo(menu, id, false, &mii);
	mii.fState = status ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo(menu, id, false, &mii);
}

static int font_sizes[8] = { 8, 12, 18, 36, 48, 56, 72, 96 };

void PaintDX::updateMainMenu() {

	HMENU menu = GetMenu(PaintDX::handle);

	// Update Status for Techniques menu entries
	for (int i = 0; i < CM_COUNT; i++) {
		setMenuStatus(menu, color_mode_menus[i], PaintDX::color_mode == i);
	}
	// Update status for multicycle option (Techniques)
	setMenuStatus(menu, ID_TECHNIQUES_MULTI, PaintDX::enable_multicycle);

	// Update status for font styles
	setMenuStatus(menu, ID_FONT_PLAIN, (!PaintDX::font_bold) && (!PaintDX::font_italic) && (!PaintDX::font_underline));
	setMenuStatus(menu, ID_FONT_BOLD, PaintDX::font_bold);
	setMenuStatus(menu, ID_FONT_UNDERLINE, PaintDX::font_underline);
	setMenuStatus(menu, ID_FONT_ITALIC, PaintDX::font_italic);

	// Update status for font sizes
	for (int i = 0; i < 8; i++) {
		setMenuStatus(menu, ID_FONT_8PIXEL + i, PaintDX::font_size == (font_sizes[i]));
	}

	// Update status for misc menu entries
	setMenuStatus(menu, ID_MISC_ANTI, PaintDX::enable_antialias);
	setMenuStatus(menu, ID_MISC_COLORIZE, PaintDX::enable_colorize);
	setMenuStatus(menu, ID_MISC_CYCLECOLORS, PaintDX::enable_cycle);
	setMenuStatus(menu, ID_MISC_TRANSLUCENT, PaintDX::enable_translucent);
	setMenuStatus(menu, ID_MISC_INFOBAR, PaintDX::enable_infobar);
	setMenuStatus(menu, ID_MISC_COORDINATES, PaintDX::enable_infobar_coordinates);
	setMenuStatus(menu, ID_MISC_INPX, PaintDX::enable_infobar_pixels);

	// Update status for stencil menu entries
	setMenuStatus(menu, ID_STENCIL_STENCILON, PaintDX::enable_stencil);
	setMenuStatus(menu, ID_STENCIL_LOCKFOREGROUND, PaintDX::enable_lockforeground);
	setMenuStatus(menu, ID_BACKGROUND_FIX, !PaintDX::enable_background_free);
	setMenuStatus(menu, ID_BACKGROUND_FREE, PaintDX::enable_background_free);

	// Update status preferences menu entries
	setMenuStatus(menu, ID_PREFERENCES_FASTFEEDBACK, PaintDX::enable_fast_feedback);
	setMenuStatus(menu, ID_PREFERENCES_SQUAREASPECT, PaintDX::enable_square_aspect);
	setMenuStatus(menu, ID_PREFERENCES_VIDEOPATTERNS, PaintDX::enable_video_patterns);
	setMenuStatus(menu, ID_PREFERENCES_HALFTONES, PaintDX::enable_halftones);
	setMenuStatus(menu, ID_PREFERENCES_BRUSHHANDLE, PaintDX::enable_brush_handle);
	setMenuStatus(menu, ID_PREFERENCES_EXCLUDEEDGE, PaintDX::enable_exclude_edge);
	setMenuStatus(menu, ID_PREFERENCES_INCHES, PaintDX::measurement == MT_INCHES);
	setMenuStatus(menu, ID_PREFERENCES_CENTIMETERS, PaintDX::measurement == MT_CENTIMETERS);
	setMenuStatus(menu, ID_PREFERENCES_POINTS, PaintDX::measurement == MT_POINTS);

}
