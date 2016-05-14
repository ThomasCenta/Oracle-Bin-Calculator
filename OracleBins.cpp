// GT_HelloWorldWin32.cpp
// compile with: /D_UNICODE /DUNICODE /DWIN32 /D_WINDOWS /c

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include "Model.h"

// Global variables
const int PROJECT_START_ID = 100;
const int PROJECT_END_ID = 101;
const int CREATE_MONTHLY_BINS_ID = 200;
const int CREATE_CUSTOM_BINS_ID = 201;
const int ADD_BIN_ID = 110;
const int CALCULATE_BINS = 111;
const int CALCULATE_BINS_PROPORTIONALLY = 112;





// The main window class name.
static TCHAR szWindowClass[] = _T("win32app");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("Win32 Guided Tour Application");

HINSTANCE hInst;

//this is going to be the model that is manipulated throughout this entire program
//Model model = Model();

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Win32 Guided Tour"),
			NULL);

		return 1;
	}

	hInst = hInstance; // Store instance handle in our global variable

	// The parameters to CreateWindow explained:
	// szWindowClass: the name of the application
	// szTitle: the text that appears in the title bar
	// WS_OVERLAPPEDWINDOW: the type of window to create
	// CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
	// 500, 100: initial size (width, length)
	// NULL: the parent of this window
	// NULL: this application does not have a menu bar
	// hInstance: the first parameter from WinMain
	// NULL: not used in this application
	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		800, 500,
		NULL,
		NULL,
		hInstance,
		NULL
		);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Win32 Guided Tour"),
			NULL);

		return 1;
	}

	// The parameters to ShowWindow explained:
	// hWnd: the value returned from CreateWindow
	// nCmdShow: the fourth parameter from WinMain
	ShowWindow(hWnd,
		nCmdShow);
	UpdateWindow(hWnd);

	// Main message loop:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR greeting[] = _T("Hello, World!");
	TCHAR ProjectStartDate[] = _T("Project Start Date:");
	TCHAR ProjectEndDate[] = _T("Project End Date:");


	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		// Lay out the application
		

		TextOut(hdc,
			5, 6,
			ProjectStartDate, _tcslen(ProjectStartDate));
		TextOut(hdc,
			280, 6,
			ProjectEndDate, _tcslen(ProjectEndDate));
		CreateWindow(TEXT("Edit"), TEXT("MM/DD/YYYY"), WS_CHILD | WS_VISIBLE | WS_BORDER, 130, 5, 100, 20, hWnd, (HMENU)100, NULL, NULL);
		//model.setProjectStartID(100);
		CreateWindow(TEXT("Edit"), TEXT("MM/DD/YYYY"), WS_CHILD | WS_VISIBLE | WS_BORDER, 405, 5, 100, 20, hWnd, (HMENU)101, NULL, NULL);
		//model.setProjectEndID(101);
		//create the buttons
		CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("Create Monthly Bins"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 80, 50, 150, 24, hWnd, (HMENU)200, GetModuleHandle(NULL), NULL);
		CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("Create Custom Bins"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 300, 50, 150, 24, hWnd, (HMENU)201, GetModuleHandle(NULL), NULL);
		//use GetDlgItem(ID#) to get a window handle





		// End application-specific layout section.

		EndPaint(hWnd, &ps);
		break;
	case WM_COMMAND:
	{
		int commandID = LOWORD(wParam); //can't use switch because not all ID's are constants
		if (commandID == CREATE_MONTHLY_BINS_ID){

		}
		else if (commandID == CREATE_CUSTOM_BINS_ID){

		}
		else if (commandID == ADD_BIN_ID){

		}
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}