#include "PaintDX.hpp"
#include "stdafx.h"
#include "../main.h"
#include "dialogs.hpp"

void openDialogPatterns() { /*DialogBox(hInst, MAKEINTRESOURCE(IDD_PATTERNS), (HWND)PaintDX::handle, PatternDialog);*/ }
extern void centerDialog(void*);

INT_PTR CALLBACK PatternDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		centerDialog(hDlg);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
