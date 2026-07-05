#include "PaintDX.hpp"
#include "stdafx.h"
#include "Resource.h"
#include "../main.h"
#include "dialogs.hpp"
#include <windows.h>

extern void centerDialog(void*);




static HWND handle;

void openDialogFill() { DialogBox(hInst, MAKEINTRESOURCE(IDD_FILL), (HWND)PaintDX::handle, FillDialog); }

FillType active_fill;

bool active_fillbg;
int active_level;
int active_contrast;
int active_translucent;

static int filltypes[10] = {
	IDC_FILLTYPE_COLORPATTERN,
	IDC_FILLTYPE_PERSPECTIVE,
	IDC_FILLTYPE_BRUSHWRAP,
	IDC_FILLTYPE_LEVEL,
	IDC_FILL_GRADIENTLINEAR_STRAIGHT,
	IDC_FILL_GRADIENTLINEAR_SHAPED,
	IDC_FILL_GRADIENTLINEAR_RIDGES,
	IDC_FILL_GRADIENTRADIAL_CIRCULAR,
	IDC_FILL_GRADIENTRADIAL_CONTOURS,
	IDC_FILL_GRADIENTRADIAL_HIGHLIGHT
};



void setFillLinearGradient(int type) {

}

void setFillRadialGradient(int type) {

}

void setFillColorPattern() {

}

void setFillBrushWrap() {

}

void setFillPerspective() {

}

void setFillLevelContrast() {

}

void setFillType(int type) {
	HWND fillmode[10];
	for (int i = 0; i < 10; i++) {
		fillmode[i] = GetDlgItem(handle, filltypes[i]);
	}

	for (int i = 0; i < 10; i++) {
		if (i == type) continue;
		SendMessage(fillmode[i], BM_SETCHECK, BST_UNCHECKED, true);
	}
	SendMessage(fillmode[type], BM_SETCHECK, BST_CHECKED, true);

	switch (type) {
	case FT_COLORPATTERN:	setFillColorPattern(); return;
	case FT_BRUSHWRAP:		setFillBrushWrap(); return;
	case FT_PERSPECTIVE:	setFillPerspective(); return;
	case FT_LEVEL:			setFillLevelContrast(); return;
	case FT_GRADIENT_LINEAR_STRAIGHT:
	case FT_GRADIENT_LINEAR_RIDGES:
	case FT_GRADIENT_LINEAR_SHAPED:
		setFillLinearGradient(type - FT_GRADIENT_LINEAR_STRAIGHT);
		return;

	case FT_GRADIENT_RADIAL_CIRCULAR:
	case FT_GRADIENT_RADIAL_CONTOURS:
	case FT_GRADIENT_RADIAL_HIGHLIGHT:
		setFillRadialGradient(type - FT_GRADIENT_RADIAL_CIRCULAR);
		break;
	}
}

void toggleFillBackground() {

}
#include <string>
static bool deserialize(int level, int contrast, int translucent, bool fillBg, FillType fillType) {
	SetWindowTextA(GetDlgItem(handle, IDC_FILLTYPE_LEVEL_VALUE), std::to_string(level).c_str());
	SetWindowTextA(GetDlgItem(handle, IDC_FILLTYPE_CONTRAST_VALUE), std::to_string(contrast).c_str());
	SetWindowTextA(GetDlgItem(handle, IDC_FILL_TRANSLUCENT), std::to_string(translucent).c_str());
	SendMessage(GetDlgItem(handle, IDC_FILL_BACKGROUND), BM_SETCHECK, fillBg ? BST_CHECKED : BST_UNCHECKED, true);
	setFillType(fillType);
	return true;
}

static void serialize(int &level, int &contrast, int &translucent, bool &fillBg, FillType &fillType) {
	char buffer[64];
	GetWindowTextA(GetDlgItem(handle, IDC_FILLTYPE_LEVEL_VALUE), buffer, 64);
	level = atoi(buffer);
	GetWindowTextA(GetDlgItem(handle, IDC_FILLTYPE_CONTRAST_VALUE), buffer, 64);
	contrast = atoi(buffer);
	GetWindowTextA(GetDlgItem(handle, IDC_FILL_TRANSLUCENT), buffer, 64);
	translucent = atoi(buffer);
	fillBg = SendMessage(GetDlgItem(handle, IDC_FILL_BACKGROUND), BM_GETCHECK, 0, 0);

	for (int i = 0; i < 10; i++) {
		if (SendMessage(GetDlgItem(handle, filltypes[i]), BM_GETCHECK, 0, 0)) {
			fillType = (FillType)i;
			break;
		}
	}	
}

bool backup_fillbg;
int backup_translucent;
int backup_level;
int backup_contrast;
FillType backup_type;

static void revert() {
	active_fillbg = backup_fillbg ;
	active_translucent=backup_translucent;
	active_level=backup_level ;
	active_contrast = backup_contrast;
	active_fill = backup_type;
	deserialize(active_level, active_contrast, active_translucent, active_fillbg, active_fill);
}

static void backup() {
	backup_fillbg = active_fillbg;
	backup_translucent= active_translucent;
	backup_level= active_level;
	backup_contrast = active_contrast;
	backup_type = active_fill;
}

static bool handleCommand(HWND hnd, int id) {
	switch (id) {
	case IDC_FILLTYPE_COLORPATTERN:				setFillType(0); break;
		case IDC_FILLTYPE_PERSPECTIVE:			setFillType(1); break;
		case IDC_FILLTYPE_BRUSHWRAP:			setFillType(2); break;
		case IDC_FILLTYPE_LEVEL:				setFillType(3); break;
		case IDC_FILL_GRADIENTLINEAR_STRAIGHT:	setFillType(4); break;
		case IDC_FILL_GRADIENTLINEAR_SHAPED:	setFillType(5); break;
		case IDC_FILL_GRADIENTLINEAR_RIDGES:	setFillType(6); break;
		case IDC_FILL_GRADIENTRADIAL_CIRCULAR:	setFillType(7); break;
		case IDC_FILL_GRADIENTRADIAL_CONTOURS:	setFillType(8); break;
		case IDC_FILL_GRADIENTRADIAL_HIGHLIGHT:	setFillType(9); break;
		case IDC_FILL_BACKGROUND:				toggleFillBackground(); break;
		case IDOK:
		case IDCANCEL:
			if(id==IDCANCEL)revert();
			EndDialog(hnd, id);
			return true;
		default: return false;
	}

	serialize(active_level, active_contrast, active_translucent, active_fillbg, active_fill);
	return true;
}

static WNDPROC oldProc;
static INT_PTR CALLBACK updateProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_KEYDOWN:
		serialize(active_level, active_contrast, active_translucent, active_fillbg, active_fill);
		break;

	case WM_KEYUP:
		serialize(active_level, active_contrast, active_translucent, active_fillbg, active_fill);
		if (PaintDX::enable_grid)PaintDX::redraw();
		break;
	}
	return CallWindowProc(oldProc, hDlg, message, wParam, lParam);
}


INT_PTR CALLBACK FillDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	handle = hDlg;

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
		case WM_INITDIALOG:
			centerDialog(hDlg);
			deserialize(active_level, active_contrast, active_translucent, active_fillbg, active_fill);
			backup();
			oldProc = (WNDPROC)SetWindowLongPtr(GetDlgItem(handle, IDC_FILLTYPE_LEVEL_VALUE		), GWLP_WNDPROC, (LONG_PTR)updateProc);
			oldProc = (WNDPROC)SetWindowLongPtr(GetDlgItem(handle, IDC_FILLTYPE_CONTRAST_VALUE	), GWLP_WNDPROC, (LONG_PTR)updateProc);
			oldProc = (WNDPROC)SetWindowLongPtr(GetDlgItem(handle, IDC_FILL_TRANSLUCENT			), GWLP_WNDPROC, (LONG_PTR)updateProc);
			return (INT_PTR)TRUE;

		case WM_COMMAND:
			return handleCommand(hDlg, LOWORD(wParam));
	}
	return (INT_PTR)FALSE;
}
