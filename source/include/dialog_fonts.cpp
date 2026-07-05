#include "PaintDX.hpp"
#include "stdafx.h"
#include "../main.h"
#include "dialogs.hpp"

void openDialogFonts() { DialogBox(hInst, MAKEINTRESOURCE(IDD_FONT), (HWND)PaintDX::handle, FontsDialog); }

extern void centerDialog(void*);



// Controlador de mensajes del cuadro Fonts
INT_PTR CALLBACK FontsDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
