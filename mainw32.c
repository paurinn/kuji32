/*
Kuji32 Flash MCU Programmer
Copyright (C) 2014 Kari Sigurjonsson

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
@addtogroup prog32
@{
*/
#include "stdafx.h"
#include "resource.h"
#include <commdlg.h>

HINSTANCE hInst = INVALID_HANDLE_VALUE;
HWND hWnd = INVALID_HANDLE_VALUE;

struct params32 params;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK DlgProgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND hwndDlg;
char filepath[MAX_PATH];
char savefilename[MAX_PATH];
char compath[256];
char port[256];
volatile int busy = 0;
char readingenabled = 0;

//For app name and version.
char windowtitle[256];

extern struct mcu32_tag mcu32_map[];
extern struct chipdef32 chipdefs[MAX_MCU32_TYPE];

DWORD WINAPI ProcessThread( LPVOID lpParam );

void chdir_to_exe();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	MSG msg;

#ifdef READINGINHIBITED
	if (lpCmdLine && strcmp("pleaseenablereading", lpCmdLine) == 0) {
		readingenabled = 1;
	}
#else
		readingenabled = 1;
#endif

	if (process_chipdef32() != E_NONE) {
		MessageBox(NULL, "Error processing 'chipdef32.ini'", "Fatal Error", MB_OK);
		return 1;
	}

	WNDCLASS wc = {
		.lpfnWndProc	= WndProc,
		.hInstance		= hInstance,
		.hbrBackground	= (HBRUSH)(COLOR_BACKGROUND),
		.lpszClassName	= "kuji32",
		.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_KUJI32))// Windows logo
	};

	if (!RegisterClass(&wc))
		return 1;

	snprintf(windowtitle, sizeof(windowtitle), "%s v%d.%d.%d", version.text, version.major, version.minor, version.build);

	hInst = hInstance;
	hWnd = CreateWindow(
		wc.lpszClassName,
		windowtitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		1,
		CW_USEDEFAULT,
		0,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd) {
		MessageBox(NULL, "Could not create main window, terminating.", "Fatal Error", MB_OK);
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	hwndDlg = CreateDialog(
		hInstance,
		MAKEINTRESOURCE(IDD_DLGPROGRAM),
		hWnd,
		DlgProgProc
	);

	RECT box = {
		.left = 0,
		.top = 0,
		.right = 289,
		.bottom = 90
	};

	MapDialogRect(
		hwndDlg,
		&box
	);

	SetWindowPos(
		hwndDlg,
		NULL,
		10,
		10,
		box.right + 10,
		box.bottom + 10,
		SWP_SHOWWINDOW
	);

	SetWindowPos(
		hWnd,
		NULL,
		0,
		0,
		box.right + 35,
		box.bottom + 55,
		SWP_SHOWWINDOW
	);

	while( GetMessage( &msg, NULL, 0, 0 ) > 0 ) {
		DispatchMessage( &msg );
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK DlgProgProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HWND child;

	switch(uMsg) {
	case WM_INITDIALOG:
		if (readingenabled) {
			child = GetDlgItem(hwndDlg, IDC_RBREAD);
			Button_Enable(child, 1);
			Button_SetCheck(child, 1);
		} else {
			child = GetDlgItem(hwndDlg, IDC_RBREAD);
			ShowWindow(child, 0);

			child = GetDlgItem(hwndDlg, IDC_RBWRITE);
			Button_Enable(child, 1);
			Button_SetCheck(child, 1);
		}

		int i;
		HWND child = GetDlgItem(hwndDlg, IDC_CBXMCU);
		for (i = 1; mcu32_map[i].name; i++) {
			ComboBox_AddString(child, mcu32_map[i].name);
		}
		ComboBox_SetCurSel(child, 32);
		params.chip = &chipdefs[mcu32_map[ComboBox_GetCurSel(GetDlgItem(hwndDlg, IDC_CBXMCU))+1].type];

		child = GetDlgItem(hwndDlg, IDC_CBXPORT);
		for (i = 1; i < 100; i++) {
			snprintf(port, sizeof(port)-1, "COM %d", i);
			ComboBox_AddString(child, port);
		}
		ComboBox_SetCurSel(child, 0);

		params.savepath = savefilename;

		//Set default mode.
		if (readingenabled) {
			params.read = true;
			params.write = false;
			params.erase = false;
		} else {
			params.read = false;
			params.write = true;
			params.erase = true;
		}

		params.srecpath = filepath;

		snprintf(compath, sizeof(compath)-1, "%d", ComboBox_GetCurSel(GetDlgItem(hwndDlg, IDC_CBXPORT))+1);
		params.comarg = compath;

		params.freq = FREQ_4MHZ;

		int id;
		for (id = 0; id < N_FREQUENCY; id++) {
			if (params.chip->clock[id] == params.freq) {
				params.freqid = id;
				break;
			}
		}

		params.timeoutsec = 5;

		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam)) {
		case IDC_CBXMCU:
			params.chip = &chipdefs[mcu32_map[ComboBox_GetCurSel(GetDlgItem(hwndDlg, IDC_CBXMCU))+1].type];
			break;
		case IDC_CBXPORT:
			snprintf(compath, sizeof(compath)-1, "%d", ComboBox_GetCurSel(GetDlgItem(hwndDlg, IDC_CBXPORT))+1);
			break;
		case IDC_RBREAD:
			params.read = true;
			params.write = false;
			params.erase = false;
			break;
		case IDC_RBWRITE:
			params.write = true;
			params.read = false;
			//Enable auto-erasing before write.
			params.erase = true;
			break;
		case IDC_BTNGO:
			{
			if (busy) {
				MessageBox(NULL, "Already working on it!", "y0", MB_OK);
				break;
			}
			/*
				int id = ComboBox_GetCurSel(GetDlgItem(hwndDlg, IDC_CBXMCU));
				params.chip = &chipdefs[id];
			*/
				SYSTEMTIME t;
				GetLocalTime(&t);
				snprintf(savefilename, sizeof(savefilename)-1,
					"%04d%02d%02dT%02d%02d%02d.mhx",
					t.wYear,
					t.wMonth,
					t.wDay,
					t.wHour,
					t.wMinute,
					t.wSecond
				);
				busy = 1;
				CreateThread(NULL, 0, ProcessThread, &params, 0, NULL);
				//process32(&params);
			}
			break;
		case IDC_BTNSELSOURCE:
			{
				OPENFILENAME ofn ;
				ZeroMemory( &ofn , sizeof( ofn));
				ofn.lStructSize = sizeof ( ofn );
				ofn.hwndOwner = NULL  ;
				ofn.lpstrFile = filepath;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrFilter = "All\0*.*\0S-Record\0*.mhx\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL ;
				ofn.nMaxFileTitle = 0 ;
				ofn.lpstrInitialDir = NULL ;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST ;
				GetOpenFileName( &ofn );
				Edit_SetText(GetDlgItem(hwndDlg, IDC_EDTFILE), ofn.lpstrFile);
			}
		}
		break;
		default:
			return FALSE;
	}

	return TRUE;
}

DWORD WINAPI ProcessThread( LPVOID lpParam ) {
	struct params32 *params = (struct params32 *)lpParam;
	chdir_to_exe();
	process32(params);
	busy = 0;
	return 0;
}
/** @} */

