#include "PaintDX.hpp"
#include "stdafx.h"
#include "../main.h"
#include "dialogs.hpp"
#include <string>
#include <CommCtrl.h>

static int backup_palette[256];
static int palette_clipboard[256];

static bool selection[256];


static int mouse_x = 0;
static int mouse_y = 0;
static int active_index = 0;
static int start_index = 0;
static int end_index = 0;

int current_color = 0;
static int current_r = 0;
static int current_g = 0;
static int current_b = 0;
static float current_h = 0.0f;
static float current_s = 0.0f;
static float current_v = 0.0f;

static HWND palette_viewport = nullptr;

extern void openDialogGradient();
void openDialogPalette() { DialogBox(hInst, MAKEINTRESOURCE(IDD_PALETTE), (HWND)PaintDX::handle, PaletteDialog); }
extern void drawGradientPalette(void *target, bool selected[256], int mx, int my, int active);
extern void setHSV(int color, float h, float  s, float v);
extern void setRGB(int color, int r, int g, int b);


// Controlador de mensajes del cuadro Palette

static bool rgb_mode = true;

HWND handle;

void getRGB(int color, int &r, int  &g, int &b);
void getHSV(int color, float &h, float  &s, float &v);

void selectColor(int index) {
	HWND mode_rgb = GetDlgItem(handle, IDC_PALETTE_RGB);
	HWND mode_hsv = GetDlgItem(handle, IDC_PALETTE_HSV);
	HWND r_bar = GetDlgItem(handle, IDC_PALETTE_R_BAR);
	HWND g_bar = GetDlgItem(handle, IDC_PALETTE_G_BAR);
	HWND b_bar = GetDlgItem(handle, IDC_PALETTE_B_BAR);
	
	getRGB(PaintDX::palette[index], current_r, current_g, current_b);
	getHSV(PaintDX::palette[index], current_h, current_s, current_v);
	
	int r = rgb_mode ? current_r : int(current_h);
	int g = rgb_mode ? current_g : int(current_s);
	int b = rgb_mode ? current_b : int(current_v);

	// Set Range for sliders
	SendMessage(r_bar, TBM_SETRANGE, true, MAKELPARAM(0, rgb_mode ? 255 : 360));
	SendMessage(g_bar, TBM_SETRANGE, true, MAKELPARAM(0, rgb_mode ? 255 : 100));
	SendMessage(b_bar, TBM_SETRANGE, true, MAKELPARAM(0, rgb_mode ? 255 : 100));

	SendMessage(mode_rgb, BM_SETCHECK,  rgb_mode ? BST_CHECKED : BST_UNCHECKED, true);
	SendMessage(mode_hsv, BM_SETCHECK, !rgb_mode ? BST_CHECKED : BST_UNCHECKED, true);
	
	// Set Position for sliders
	SendMessage(r_bar, TBM_SETPOS, true, r);
	SendMessage(g_bar, TBM_SETPOS, true, g);
	SendMessage(b_bar, TBM_SETPOS, true, b);

	// Set static texts
	SetDlgItemTextA(handle, IDC_PALETTE_R_VALUE, std::to_string(r).c_str());
	SetDlgItemTextA(handle, IDC_PALETTE_G_VALUE, std::to_string(g).c_str());
	SetDlgItemTextA(handle, IDC_PALETTE_B_VALUE, std::to_string(b).c_str());
	SetDlgItemTextA(handle, IDC_PALETTE_R, rgb_mode ? "R" : "H");
	SetDlgItemTextA(handle, IDC_PALETTE_G, rgb_mode ? "G" : "S");
	SetDlgItemTextA(handle, IDC_PALETTE_B, rgb_mode ? "B" : "V");
}

/****************************************************************/
/* WORKING														*/	
/****************************************************************/

void backupPalette() {
	memcpy(backup_palette, PaintDX::palette, sizeof(int) * 256);		
}

void revertPalette() {
	memcpy(PaintDX::palette, backup_palette, sizeof(int) * 256);
	PaintDX::redraw();
	selectColor(current_color);
	drawGradientPalette(palette_viewport, selection, mouse_x, mouse_y, current_color);
	SendMessage(handle, WM_WINDOWPOSCHANGED, 0, 0);
	backupPalette();
}

void copyPalette() {
	int blocksize = (end_index + 1) - start_index;
	int availsize = 256 - current_color;
	memset(palette_clipboard, 0, sizeof(int) * 256);
	memcpy(palette_clipboard + start_index, PaintDX::palette + start_index, sizeof(int)*(blocksize));
	if (blocksize > availsize)blocksize = availsize;

	memcpy(PaintDX::palette+current_color, palette_clipboard + start_index, sizeof(int)*(blocksize));
	
	PaintDX::redraw();
	selectColor(current_color);
	drawGradientPalette(palette_viewport, selection, mouse_x, mouse_y, current_color);
	SendMessage(handle, WM_WINDOWPOSCHANGED, 0, 0);
}

void setColorModeRGB() {
	rgb_mode = true;
	selectColor(current_color);
}

void setColorModeHSV() {
	rgb_mode = false;
	selectColor(current_color);
}

void updateR(int v) {
	if (rgb_mode) {
		current_r = v;
		setRGB(current_color, current_r, current_g, current_b);
	} else {
		current_h = float(v);
		setHSV(current_color, current_h, current_s, current_v);
	}
	PaintDX::redraw();
	selectColor(current_color);
	drawGradientPalette(palette_viewport, selection, mouse_x, mouse_y, current_color);
	SendMessage(handle, WM_WINDOWPOSCHANGED, 0, 0);
}

void updateG(int v) {
	if (rgb_mode) {
		current_g = v;
		setRGB(current_color, current_r, current_g, current_b);
	} else {
		current_s = float(v);
		setHSV(current_color, current_h, current_s, current_v);
	}
	PaintDX::redraw();
	selectColor(current_color);
	drawGradientPalette(palette_viewport, selection, mouse_x, mouse_y, current_color);
	SendMessage(handle, WM_WINDOWPOSCHANGED, 0, 0);
}

void updateB(int v) {
	if (rgb_mode) {
		current_b = v;
		setRGB(current_color, current_r, current_g, current_b);
	} else {
		current_v = float(v);
		setHSV(current_color, current_h, current_s, current_v);
	}
	PaintDX::redraw();
	selectColor(current_color);
	drawGradientPalette(palette_viewport, selection, mouse_x, mouse_y, current_color);
	SendMessage(handle, WM_WINDOWPOSCHANGED, 0, 0);
}

void spreadPalette() {
	Gradient g;
	g.generate(start_index, end_index);
	PaintDX::redraw();
	selectColor(current_color);
	drawGradientPalette(palette_viewport, selection, 0, 0, current_color);
	SendMessage(handle, WM_WINDOWPOSCHANGED, 0, 0);
}


static bool updatePalette(HWND hDlg, bool click, bool endstop, bool right) {
	RECT r;
	GetClientRect(palette_viewport, &r);
	MapWindowPoints(palette_viewport, hDlg, (LPPOINT)&r, 2);

	handle = hDlg;

	if (((mouse_x >= r.left) && (mouse_x <= r.right) && (mouse_y >= r.top) && (mouse_y <= r.bottom))) {
		int column = (mouse_x - r.left) / (GRADIENT_CELL_W + GRADIENT_SEL_W);
		int row = (mouse_y - r.top) / (GRADIENT_CELL_H);
		if (click) {
			active_index = (column * 16) + row;
			if (!right) {
				end_index = active_index;
				if (!endstop)start_index = active_index;
				else {
					int m = max(end_index, start_index);
					start_index = min(end_index, start_index);
					end_index = m;
				}
			
				memset(selection, 0, sizeof(bool) * 256);
				for (int i = min(start_index, end_index + 1); i < max(start_index, end_index + 1); i++) {
					selection[i] = true;
				}
			
			} else {
				current_color = active_index;
				selectColor(current_color);
			}
		}
		drawGradientPalette(palette_viewport, selection, column, row, current_color);
		GetClientRect(hDlg, &r);
		InvalidateRect(hDlg, &r, false);
		return TRUE;
	}
	return FALSE;
}

void swapPalette() {
	if (current_color == start_index)return;
	int t = PaintDX::palette[start_index];
	PaintDX::palette[start_index] = PaintDX::palette[current_color];
	PaintDX::palette[current_color] = t;
	PaintDX::redraw();
	selectColor(current_color);
	drawGradientPalette(palette_viewport, selection, 0, 0, current_color);
	SendMessage(handle, WM_WINDOWPOSCHANGED, 0, 0);
}
static bool handleCommand(HWND hnd, int id) {
	switch (id){
		case IDC_PALETTE_SPREAD:	spreadPalette(); return true;
		case IDC_PALETTE_GRADIENT:	backupPalette(); EndDialog(hnd, id); openDialogGradient(); return true;
		case IDC_PALETTE_COPY:		copyPalette(); return true;
		case IDC_PALETTE_SWAP:		swapPalette(); return true;
		case IDC_PALETTE_RGB:		setColorModeRGB(); return true;
		case IDC_PALETTE_HSV:		setColorModeHSV(); return true;
		case IDC_PALETTE_UNDO:		revertPalette(); return true;
		default: return false;
	}
}
extern void centerDialog(void*);

INT_PTR CALLBACK PaletteDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
	UNREFERENCED_PARAMETER(lParam);

	RECT r;
	
	switch (message){
		case WM_INITDIALOG:
			memset(selection, 0, sizeof(bool) * 256);
			active_index = 0;
			start_index = 0;
			end_index = 0;
			// initialize controls to their current values
			palette_viewport = GetDlgItem(hDlg, IDC_GRADIENT_VIEWPORT);
			SetWindowLong(palette_viewport, GWL_EXSTYLE, GetWindowLong(palette_viewport, GWL_EXSTYLE) | WS_EX_LAYERED);
			setColorModeRGB();
			GetClientRect(hDlg, &r);
			InvalidateRect(hDlg, &r, false);
			drawGradientPalette(palette_viewport, selection, 0, 0, current_color);
			memset(palette_clipboard, 0, sizeof(int) * 256);
			backupPalette();
			SendMessage(GetDlgItem(hDlg, IDC_PALETTE_RGB), BM_SETCHECK, BST_CHECKED, true);
			centerDialog(hDlg);
			return (INT_PTR)TRUE;

		case WM_HSCROLL:
			switch (LOWORD(wParam)) {
				case TB_THUMBTRACK:
					if ((HWND)lParam == GetDlgItem(hDlg, IDC_PALETTE_R_BAR)) {
						updateR((int)SendMessage(GetDlgItem(hDlg, IDC_PALETTE_R_BAR), TBM_GETPOS, 0, 0));
						selectColor(current_color);
						return true;
					}
					if ((HWND)lParam == GetDlgItem(hDlg, IDC_PALETTE_G_BAR)) {
						updateG((int)SendMessage(GetDlgItem(hDlg, IDC_PALETTE_G_BAR), TBM_GETPOS, 0, 0));
						selectColor(current_color);
						return true;
					}
					if ((HWND)lParam == GetDlgItem(hDlg, IDC_PALETTE_B_BAR)) {
						updateB((int)SendMessage(GetDlgItem(hDlg, IDC_PALETTE_B_BAR), TBM_GETPOS, 0, 0));
						selectColor(current_color);
						return true;
					}				
			}
			return TRUE;

		case WM_WINDOWPOSCHANGED:
			palette_viewport = GetDlgItem(hDlg, IDC_PALETTE_VIEWPORT);
			drawGradientPalette(palette_viewport, selection, 0, 0, current_color);
			GetClientRect(hDlg, &r);
			InvalidateRect(hDlg, &r, false);
			selectColor(current_color);
			return (INT_PTR)TRUE;

		case WM_MOUSEMOVE:
			mouse_x = LOWORD(lParam);
			mouse_y = HIWORD(lParam);
			GetClientRect(hDlg, &r);
			InvalidateRect(hDlg, &r, false);
			return (INT_PTR)updatePalette(hDlg, false, false, false);

		case WM_LBUTTONDOWN:
			mouse_x = LOWORD(lParam);
			mouse_y = HIWORD(lParam);
			GetClientRect(hDlg, &r);
			InvalidateRect(hDlg, &r, false);
			return (INT_PTR)updatePalette(hDlg, true, false, false);

		case WM_LBUTTONUP:
			mouse_x = LOWORD(lParam);
			mouse_y = HIWORD(lParam);
			GetClientRect(hDlg, &r);
			InvalidateRect(hDlg, &r, false);
			return (INT_PTR)updatePalette(hDlg, true, true, false);

		case WM_RBUTTONDOWN:
			mouse_x = LOWORD(lParam);
			mouse_y = HIWORD(lParam);
			GetClientRect(hDlg, &r);
			InvalidateRect(hDlg, &r, false);
			return (INT_PTR)updatePalette(hDlg, true, false, true);

		case WM_RBUTTONUP:
			mouse_x = LOWORD(lParam);
			mouse_y = HIWORD(lParam);
			GetClientRect(hDlg, &r);
			InvalidateRect(hDlg, &r, false);
			return (INT_PTR)updatePalette(hDlg, true, true, true);

		case WM_COMMAND:
			if (handleCommand(hDlg, LOWORD(wParam))) return (INT_PTR)TRUE;
			else if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL){
				if (LOWORD(wParam) == IDOK) {
					backupPalette();
				} else {
					revertPalette();
				}
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			return false;
	}
	return (INT_PTR)FALSE;
}

