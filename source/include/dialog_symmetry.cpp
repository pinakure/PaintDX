#include "PaintDX.hpp"
#include "stdafx.h"
#include "../main.h"
#include "dialogs.hpp"

void openDialogSymmetry() { DialogBox(hInst, MAKEINTRESOURCE(IDD_SYMMETRY), (HWND)PaintDX::handle, SymmetryDialog); }

static HWND handle;

extern bool select_point;
extern bool select_box;
extern int select_point_x;
extern int select_point_y;
extern int select_box_x;
extern int select_box_y;
extern int select_box_h;
extern int select_box_h;
extern bool point_selected;
extern bool box_selected;


static bool backup_disable;
static bool backup_cyclic;
static bool backup_tile;
static bool backup_mirror;
static int backup_points;
static int backup_width;
static int backup_height;

bool symmetry_disable = true;
bool symmetry_cyclic = false;
bool symmetry_tile = false ;
bool symmetry_mirror = false;
int symmetry_points = 6;
int symmetry_width = 16;
int symmetry_height = 16;
int symmetry_x = 320/2;
int symmetry_y = 240/2;

static bool click = false;
static bool unclick = false;
static int mouse_x = 0;
static int mouse_y = 0;
static int click_x = 0;
static int click_y = 0;
static int unclick_x = 0;
static int unclick_y = 0;

#include <string>
static bool deserialize(bool disable, bool cyclic, bool mirror, bool tile, int points, int width ,int height) {
	SetWindowTextA(GetDlgItem(handle, IDC_SYMMETRY_WIDTH), std::to_string(width).c_str());
	SetWindowTextA(GetDlgItem(handle, IDC_SYMMETRY_HEIGHT), std::to_string(height).c_str());
	SetWindowTextA(GetDlgItem(handle, IDC_SYMMETRY_POINTS), std::to_string(points).c_str());
	SendMessage(GetDlgItem(handle, IDC_SYMMETRY_DISABLED), BM_SETCHECK, disable ? BST_CHECKED : BST_UNCHECKED, true);
	SendMessage(GetDlgItem(handle, IDC_SYMMETRY_CYCLIC), BM_SETCHECK, cyclic? BST_CHECKED : BST_UNCHECKED, true);
	SendMessage(GetDlgItem(handle, IDC_SYMMETRY_MIRROR), BM_SETCHECK, mirror? BST_CHECKED : BST_UNCHECKED, true);
	SendMessage(GetDlgItem(handle, IDC_SYMMETRY_TILE), BM_SETCHECK, tile ? BST_CHECKED : BST_UNCHECKED, true);
	return true;
}

static void serialize(bool &disable, bool &cyclic, bool &mirror, bool &tile, int &points, int &width, int &height) {
	char buffer[64];
	GetWindowTextA(GetDlgItem(handle, IDC_SYMMETRY_POINTS), buffer, 64);
	points= atoi(buffer);
	GetWindowTextA(GetDlgItem(handle, IDC_SYMMETRY_WIDTH), buffer, 64);
	width  = atoi(buffer);
	GetWindowTextA(GetDlgItem(handle, IDC_SYMMETRY_HEIGHT), buffer, 64);
	height = atoi(buffer);
	
	disable = SendMessage(GetDlgItem(handle, IDC_SYMMETRY_DISABLED), BM_GETCHECK, 0, 0);
	cyclic = SendMessage(GetDlgItem(handle, IDC_SYMMETRY_CYCLIC), BM_GETCHECK, 0, 0);
	mirror = SendMessage(GetDlgItem(handle, IDC_SYMMETRY_MIRROR), BM_GETCHECK, 0, 0);
	tile = SendMessage(GetDlgItem(handle, IDC_SYMMETRY_TILE), BM_GETCHECK, 0, 0);
}


static void revert() {
	symmetry_disable = backup_disable;
	symmetry_cyclic = backup_cyclic;
	symmetry_tile = backup_tile; 
	symmetry_mirror = backup_mirror;
	symmetry_points = backup_points;
	symmetry_width = backup_width;
	symmetry_height = backup_height;	
	deserialize(symmetry_disable, symmetry_cyclic, symmetry_mirror, symmetry_tile, symmetry_points, symmetry_width, symmetry_height);
}

static void backup() {
	backup_disable = symmetry_disable;
	backup_cyclic = symmetry_cyclic;
	backup_tile = symmetry_tile;
	backup_mirror = symmetry_mirror;
	backup_points = symmetry_points;
	backup_width = symmetry_width;
	backup_height = symmetry_height;
}

static void placeCenter() {
	MSG msg;

	click = false;
	unclick = false;
	mouse_x = 0;
	mouse_y = 0;
	click_x = 0;
	click_y = 0;
	unclick_x = 0;
	unclick_y = 0;

	while (!point_selected) {
		if (GetMessage(&msg, nullptr, 0, 0)) {
			if (!TranslateAccelerator(msg.hwnd, nullptr, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
}



static bool handleCommand(HWND hnd, int id) {
	switch (id) {
		case IDC_SYMMETRY_DISABLED:		symmetry_cyclic = false; symmetry_mirror = false; symmetry_tile = false; symmetry_disable = true; break;
		case IDC_SYMMETRY_CYCLIC:		symmetry_cyclic = true;  symmetry_mirror = false; symmetry_tile = false; symmetry_disable = false; break;
		case IDC_SYMMETRY_MIRROR:		symmetry_cyclic = false; symmetry_mirror = true;  symmetry_tile = false; symmetry_disable = false; break;
		case IDC_SYMMETRY_TILE:			symmetry_cyclic = false; symmetry_mirror = false; symmetry_tile = true;  symmetry_disable = false; break;
		case IDC_SYMMETRY_PLACECENTER:  
			EndDialog(hnd, id); 
			select_point = true;
			placeCenter(); 
			symmetry_x = select_point_x;
			symmetry_y = select_point_y;
			point_selected = false;
			select_point = false;
			openDialogSymmetry(); 
			break;

		case IDOK:
		case IDCANCEL:
			if(id==IDCANCEL)revert();
			EndDialog(hnd, id);
			return true;
		default: return false;
	}

	serialize(symmetry_disable, symmetry_cyclic, symmetry_mirror, symmetry_tile, symmetry_points, symmetry_width, symmetry_height);
	return true;
}

static WNDPROC oldProc;
static INT_PTR CALLBACK updateProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_KEYDOWN:
		serialize(symmetry_disable, symmetry_cyclic, symmetry_mirror, symmetry_tile, symmetry_points, symmetry_width, symmetry_height); 
		break;

	case WM_KEYUP:
		serialize(symmetry_disable, symmetry_cyclic, symmetry_mirror, symmetry_tile, symmetry_points, symmetry_width, symmetry_height);
		if (PaintDX::enable_grid)PaintDX::redraw();
		break;
	}
	return CallWindowProc(oldProc, hDlg, message, wParam, lParam);
}

extern void centerDialog(void*);

INT_PTR CALLBACK SymmetryDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

	handle = hDlg;
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		centerDialog(hDlg);
		deserialize(symmetry_disable, symmetry_cyclic, symmetry_mirror, symmetry_tile, symmetry_points, symmetry_width, symmetry_height);
		backup();
		oldProc = (WNDPROC)SetWindowLongPtr(GetDlgItem(handle, IDC_SYMMETRY_POINTS	), GWLP_WNDPROC, (LONG_PTR)updateProc);
		oldProc = (WNDPROC)SetWindowLongPtr(GetDlgItem(handle, IDC_SYMMETRY_WIDTH	), GWLP_WNDPROC, (LONG_PTR)updateProc);
		oldProc = (WNDPROC)SetWindowLongPtr(GetDlgItem(handle, IDC_SYMMETRY_HEIGHT	), GWLP_WNDPROC, (LONG_PTR)updateProc);
		return (INT_PTR)TRUE;

	case WM_MOUSEMOVE:
		mouse_x = LOWORD(lParam);
		mouse_y = HIWORD(lParam);
		break;

	case WM_LBUTTONDOWN:
		mouse_x = LOWORD(lParam);
		mouse_y = HIWORD(lParam);
		click = true;
		unclick = false;
		break;

	case WM_LBUTTONUP:
		mouse_x = LOWORD(lParam);
		mouse_y = HIWORD(lParam);
		click = false;
		unclick = true;
		break;

	case WM_COMMAND:
		return handleCommand(hDlg, LOWORD(wParam));
	}
	return (INT_PTR)FALSE;
}
