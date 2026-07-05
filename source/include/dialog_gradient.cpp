#include "PaintDX.hpp"
#include "stdafx.h"
#include "../main.h"
#include "dialogs.hpp"

HWND dialog_gradient = nullptr;

int backup_palette[256];

std::vector<Gradient>		PaintDX::gradients;

extern void openDialogPalette();
extern void drawGradientPreview(void *target, std::vector<ColorStop> &stops);
extern void drawGradientPalette(void *handle, bool selected[256], int x = 0, int y = 0, int active = 0);


static int mouse_x = 0;
static int mouse_y = 0;
static int active_index = 0;
static int start_index = 0;
static int end_index = 0;

int current_gradient = 0;
static int current_spatter = 0;
static int current_speed = 0; 
static bool cycle_forward = true;

static bool selection[256];

static std::vector<ColorStop> stops;


void openDialogGradient() { DialogBox(hInst, MAKEINTRESOURCE(IDD_GRADIENTS), (HWND)PaintDX::handle, GradientDialog); }

static void selectGradient(int index);

static bool updatePalette(HWND hDlg, bool click, bool endstop, bool force=false) {
	RECT r;
	HWND hnd = GetDlgItem(hDlg, IDC_GRADIENT_VIEWPORT);
	GetClientRect(hnd, &r);
	MapWindowPoints(hnd, hDlg, (LPPOINT)&r, 2);
	if(((mouse_x >= r.left) && (mouse_x <= r.right) && (mouse_y >= r.top) && (mouse_y <= r.bottom))||force) {
		int column = (mouse_x - r.left) / (GRADIENT_CELL_W + GRADIENT_SEL_W);
		int row = (mouse_y - r.top) / (GRADIENT_CELL_H);
		if (click) {
			active_index = (column * 16) + row;
			
			end_index = active_index;
			if (!endstop)start_index = active_index;
			else {
				int m = max(end_index, start_index);
				start_index = min(end_index, start_index);
				end_index = m;
				PaintDX::gradients[current_gradient].index_start = start_index;
				PaintDX::gradients[current_gradient].index_end = end_index;
				PaintDX::gradients[current_gradient].calculateStops();
				drawGradientPreview(GetDlgItem(hDlg, IDC_GRADIENT_PREVIEW), PaintDX::gradients[current_gradient].stops);
				GetClientRect(GetDlgItem(hDlg, IDC_GRADIENT_PREVIEW), &r);
				InvalidateRect(GetDlgItem(hDlg, IDC_GRADIENT_PREVIEW), &r, false);
			}

			memset(selection, 0, sizeof(bool) * 256);
			for (int i = min(start_index, end_index+1); i < max(start_index, end_index+1); i++) {
				selection[i] = true;
			}
			
		}
		drawGradientPalette(hnd, selection, column, row);
		GetClientRect(hDlg, &r);
		InvalidateRect(hDlg, &r, false);
		return TRUE;
	}
	return FALSE;
}

static HWND handle = nullptr;

#include <CommCtrl.h>
#include <string>

static void selectGradient(int index) {
	if (index < 0)index = 0;
	if(index > 15)index = 15;
	Gradient *g = &PaintDX::gradients[index];
	stops.clear();
	start_index = g->index_start;
	end_index = g->index_end;
	memset(selection, 0, sizeof(bool) * 256);
	for (int i = min(start_index, end_index+1); i < max(start_index, end_index+1); i++) {
		selection[i] = true;		
	}
	
	stops = g->stops;
	current_spatter = g->spatter;
	current_gradient = index;
	current_speed = g->cycle_speed;
	cycle_forward = g->forward;

	
	HWND cycle_up = GetDlgItem(handle, IDC_GRADIENT_UP);
	HWND cycle_down = GetDlgItem(handle, IDC_GRADIENT_DOWN);
	HWND gradient_bar = GetDlgItem(handle, IDC_GRADIENT_INDEX_VALUE);
	HWND speed_bar = GetDlgItem(handle, IDC_GRADIENT_SPEED_VALUE);
	HWND spatter_bar = GetDlgItem(handle, IDC_GRADIENT_SPATTER_VALUE);
	HWND gradient_label = GetDlgItem(handle, IDC_GRADIENT_INDEX_LABEL);
	HWND speed_label = GetDlgItem(handle, IDC_GRADIENT_SPEED_LABEL);
	HWND spatter_vp = GetDlgItem(handle, IDC_GRADIENT_SPATTER_PREVIEW);

	// Select cycle direction
	SendMessage(cycle_up, BM_SETCHECK, cycle_forward ? BST_CHECKED : BST_UNCHECKED, true);
	SendMessage(cycle_down, BM_SETCHECK, !cycle_forward?BST_CHECKED:BST_UNCHECKED, true);

	// Set Range for sliders
	SendMessage(gradient_bar, TBM_SETRANGE, true, 15 << 16);
	SendMessage(spatter_bar, TBM_SETRANGE, true, 7 << 16);
	SendMessage(speed_bar, TBM_SETRANGE, true, 7 << 16);

	// Set Position for sliders
	SendMessage(gradient_bar, TBM_SETPOS, true, current_gradient);
	SendMessage(spatter_bar, TBM_SETPOS, true, current_spatter);
	SendMessage(speed_bar, TBM_SETPOS, true, current_speed);
	
	// Set static texts
	SetDlgItemTextA(handle, IDC_GRADIENT_INDEX_LABEL, std::to_string(index).c_str());
	SetDlgItemTextA(handle, IDC_GRADIENT_SPEED_LABEL, std::to_string(current_speed).c_str());

	// Preview gradient on GRADIENT_PREVIEW
	// Preview spatter on GRADIENT_SPATTER_PREVIEW
	RECT r;
	HWND hnd;
	
	hnd = GetDlgItem(handle, IDC_GRADIENT_VIEWPORT);
	GetClientRect(hnd, &r);
	InvalidateRect(hnd, &r, false);
	drawGradientPalette(hnd, selection, 0, 0);
	
	hnd = GetDlgItem(handle, IDC_GRADIENT_PREVIEW);
	GetClientRect(hnd, &r);
	InvalidateRect(hnd, &r, false);
	drawGradientPreview(hnd, PaintDX::gradients[current_gradient].stops);

	GetClientRect(handle, &r);
	InvalidateRect(handle, &r, false);
}

extern void centerDialog(void*);


// Controlador de mensajes del cuadro Gradient
INT_PTR CALLBACK GradientDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
	handle = hDlg;
	dialog_gradient = handle;

	RECT r;
	HWND hnd;

	
	static int last_current_gradient = current_gradient;

	
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		memset(selection, 0, sizeof(bool) * 256);
		active_index = 0;
		start_index = 0;
		end_index = 0;
		// initialize controls to their current values
		hnd = GetDlgItem(hDlg, IDC_GRADIENT_VIEWPORT);
		SetWindowLong(hnd, GWL_EXSTYLE, GetWindowLong(hnd, GWL_EXSTYLE) | WS_EX_LAYERED);

		GetClientRect(hDlg, &r);
		InvalidateRect(hDlg, &r, false);
		drawGradientPalette(hnd, selection, 0, 0);
		centerDialog(hDlg);

		return (INT_PTR)TRUE;

	case WM_HSCROLL:
		switch (LOWORD(wParam)) {
			case TB_THUMBTRACK:
				if ((HWND)lParam == GetDlgItem(hDlg, IDC_GRADIENT_INDEX_VALUE)) {
					current_gradient = SendMessage(GetDlgItem(hDlg, IDC_GRADIENT_INDEX_VALUE), TBM_GETPOS, 0, 0);
					selectGradient(current_gradient);
					return true;
				}
				if ((HWND)lParam == GetDlgItem(hDlg, IDC_GRADIENT_SPATTER_VALUE)) {
					current_spatter = SendMessage(GetDlgItem(hDlg, IDC_GRADIENT_SPATTER_VALUE), TBM_GETPOS, 0, 0);
					PaintDX::gradients[current_gradient].spatter = current_spatter;
					
					//the more spatter, the more colorstops, going from no dithering(0) to max dithering(4) at max level like this
					// S -> solid color
					// n -> dithering level
					// spatter level for a gradient using 2 colors
					// spatter level
					//												    SIZE		  FGPOS			 DITHPOS		 BGPOS
					//	0 :  2 color stops [SS]						2					0		        -			   1
					//	1 :  3 color stops [S1S]					2 + ((2*1)-1)		0			    1			   2			
					//	2 :  5 color stops [S212S]					2 + ((2*2)-1)		0			  1,2,3			   4
					//	3 :  7 color stops [S32123S]				2 + ((2*3)-1)		0		    1,2,3,4,5		   6
					//	4 :  9 color stops [S4321234S]				2 + ((2*4)-1)		0		  1,2,3,4,5,6,7		   8
					//	5 : 11 color stops [S543212345S]			2 + ((2*5)-1)		0	    1,2,3,4,5,6,7,8,9	   A
					//	6 : 13 color stops [S65432123456S]			2 + ((2*6)-1)		0	  1,2,3,4,5,6,7,8,9,A,B	   C
					//	7 : 15 color stops [S7654321234567S]		2 + ((2*7)-1)		0   1,2,3,4,5,6,7,8,9,A,B,C,D  E
					PaintDX::gradients[current_gradient].calculateStops();					
					selectGradient(current_gradient);
					return true;
				}

				if ((HWND)lParam == GetDlgItem(hDlg, IDC_GRADIENT_SPEED_VALUE)) {
					current_speed = SendMessage(GetDlgItem(hDlg, IDC_GRADIENT_SPEED_VALUE), TBM_GETPOS, 0, 0);
					PaintDX::gradients[current_gradient].cycle_speed = current_speed; 
					PaintDX::setCycleRate(0); 
					PaintDX::setCycleRate(current_speed);
					selectGradient(current_gradient);
					return true;
				}
				return TRUE;
		}
		return TRUE;

	case WM_WINDOWPOSCHANGED:
		GetClientRect(hDlg, &r);
		InvalidateRect(hDlg, &r, false);
		selectGradient(current_gradient);
		drawGradientPalette(GetDlgItem(hDlg, IDC_GRADIENT_VIEWPORT), selection, mouse_x, mouse_y, 0);
		drawGradientPreview(GetDlgItem(hDlg, IDC_GRADIENT_PREVIEW), PaintDX::gradients[current_gradient].stops);
		return (INT_PTR)TRUE;

	case WM_MOUSEMOVE:
		mouse_x = LOWORD(lParam);
		mouse_y = HIWORD(lParam);
		GetClientRect(hDlg, &r);
		InvalidateRect(hDlg, &r, false);
		return (INT_PTR)updatePalette(hDlg, false, false);
		
	case WM_LBUTTONDOWN:
		mouse_x = LOWORD(lParam);
		mouse_y = HIWORD(lParam);
		GetClientRect(hDlg, &r);
		InvalidateRect(hDlg, &r, false);
		return (INT_PTR)updatePalette(hDlg, true, false);

	case WM_LBUTTONUP:
		mouse_x = LOWORD(lParam);
		mouse_y = HIWORD(lParam);
		GetClientRect(hDlg, &r);
		InvalidateRect(hDlg, &r, false);
		return (INT_PTR)updatePalette(hDlg, true, true);

	case WM_COMMAND:

		int wmId = LOWORD(wParam);
		// Analizar las selecciones de menú:
		switch (wmId)
		{

		
		case IDC_GRADIENT_BUILD:
			PaintDX::gradients[current_gradient].generate(start_index, end_index);
			selectGradient(current_gradient);
			return (INT_PTR)TRUE;

		case IDC_GRADIENT_COPY:
			SendMessage(hDlg, WM_WINDOWPOSCHANGED, 0, 0);
			return (INT_PTR)TRUE;

		case IDC_GRADIENT_DOWN:
			if (!cycle_forward)return true;
			cycle_forward = false;
			SendMessage(GetDlgItem(hDlg, IDC_GRADIENT_UP), BM_SETCHECK, cycle_forward ? BST_CHECKED : BST_UNCHECKED, true);
			SendMessage(GetDlgItem(hDlg, IDC_GRADIENT_DOWN), BM_SETCHECK, !cycle_forward ? BST_CHECKED : BST_UNCHECKED, true);
			PaintDX::gradients[current_gradient].forward = cycle_forward;
			PaintDX::gradients[current_gradient].calculateStops();
			selectGradient(current_gradient);
			SendMessage(hDlg, WM_WINDOWPOSCHANGED, 0, 0);
			return (INT_PTR)TRUE;

		case IDC_GRADIENT_UP:
			if (cycle_forward)return true;
			cycle_forward = true;
			SendMessage(GetDlgItem(hDlg, IDC_GRADIENT_UP), BM_SETCHECK, cycle_forward ? BST_CHECKED : BST_UNCHECKED, true);
			SendMessage(GetDlgItem(hDlg, IDC_GRADIENT_DOWN), BM_SETCHECK, !cycle_forward ? BST_CHECKED : BST_UNCHECKED, true);
			PaintDX::gradients[current_gradient].forward = cycle_forward; 
			PaintDX::gradients[current_gradient].calculateStops();
			selectGradient(current_gradient);
			SendMessage(hDlg, WM_WINDOWPOSCHANGED, 0, 0);
			return (INT_PTR)TRUE;

		case IDC_GRADIENT_SWAP:
			return (INT_PTR)TRUE;

		case IDC_GRADIENT_PALETTE:
				EndDialog(hDlg, LOWORD(wParam));
				openDialogPalette();
				return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			dialog_gradient = nullptr;
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
