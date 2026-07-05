#include "PaintDX.hpp"
#include "stdafx.h"
#include "../main.h"
#include "dialogs.hpp"

void openDialogGrid() { DialogBox(hInst, MAKEINTRESOURCE(IDD_GRID), (HWND)PaintDX::handle, GridDialog); }

void gridAdjust() {

}

void gridFromBrush() {

}

static HWND handle;

#include <string>
static bool deserialize(int width, int height) {
	SetWindowTextA(GetDlgItem(handle, IDC_GRID_WIDTH), std::to_string(width).c_str());
	SetWindowTextA(GetDlgItem(handle, IDC_GRID_HEIGHT), std::to_string(height).c_str());
	return true;
}

static void serialize(int &width, int &height) {
	char buffer[64];
	GetWindowTextA(GetDlgItem(handle, IDC_GRID_WIDTH), buffer, 64);
	width = atoi(buffer);
	GetWindowTextA(GetDlgItem(handle, IDC_GRID_HEIGHT), buffer, 64);
	height = atoi(buffer);
}

static bool handleCommand(HWND hnd, int id) {
	switch (id) {
	case IDC_GRID_ADJUST:		gridAdjust(); return true;
	case IDC_GRID_FROMBRUSH:	gridFromBrush(); return true;
	case IDOK:
	case IDCANCEL:
		EndDialog(hnd, id);
		return true;
	default: return false;
	}
}

/*void getUserRectangle() {
	while(click_x == unclick_x) {
		MSG msg;
		if(GetMessage(&msg, nullptr, 0, 0)) DispatchMessage(&msg);


	}
}*/


static WNDPROC oldProc;
static INT_PTR CALLBACK updateProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_KEYDOWN:
			serialize(PaintDX::grid_width, PaintDX::grid_height);
			break;

		case WM_KEYUP:
			serialize(PaintDX::grid_width, PaintDX::grid_height);
			if (PaintDX::enable_grid)PaintDX::redraw();
			break;
	}
	return CallWindowProc(oldProc, hDlg, message, wParam, lParam);
}

extern void centerDialog(void*);

INT_PTR CALLBACK GridDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
	UNREFERENCED_PARAMETER(lParam);

	handle = hDlg;

	switch (message)
	{
		case WM_INITDIALOG:
			centerDialog(hDlg);
			deserialize(PaintDX::grid_width, PaintDX::grid_height);
			oldProc = (WNDPROC)SetWindowLongPtr(GetDlgItem(handle, IDC_GRID_WIDTH	), GWLP_WNDPROC, (LONG_PTR)updateProc);
			oldProc = (WNDPROC)SetWindowLongPtr(GetDlgItem(handle, IDC_GRID_HEIGHT	), GWLP_WNDPROC, (LONG_PTR)updateProc);
			return true;
		
		case WM_COMMAND:
			return handleCommand(hDlg, LOWORD(wParam));
	}
	return (INT_PTR)FALSE;
}
