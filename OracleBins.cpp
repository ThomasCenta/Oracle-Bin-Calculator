// GT_HelloWorldWin32.cpp
// compile with: /D_UNICODE /DUNICODE /DWIN32 /D_WINDOWS /c

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include "Model.h"


// Global variables
const int yChar = 20;       // vertical scrolling unit 
const int PROJECT_START_ID = 110;
const int PROJECT_END_ID = 111;
const int CREATE_MONTHLY_BINS_ID = 112;
const int CREATE_CUSTOM_BINS_ID = 113;
const int ADD_BIN_ID = 114;
const int CALCULATE_BINS_ID = 115;
const int CALCULATE_BINS_PROPORTIONALLY_ID = 116;
const int D_BETWEEN_ROWS = 50; //first row starts at y value of 100
SavedText errorBox;
COLORREF BACKGROUND_COLOR = RGB(255, 255, 255);
std::vector<SavedText> madeBins;
SCROLLINFO si;

// The main window class name.
static TCHAR szWindowClass[] = _T("Bin Calculator");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("Oracle Bins Calculator");

HINSTANCE hInst;

//this is going to be the model that is manipulated throughout this entire program
Model model;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


/* returns the needed maximum scrolling size
*/

int getMaxScroll(HWND hWnd){
	RECT temp;
	GetWindowRect(hWnd, &temp);
	int pageHeight = temp.bottom - temp.top;
	int neededScrollHeight = 300 + 50 * model.numAllocationPeriods() - pageHeight;
	int max = (int)std::ceil(neededScrollHeight * 1.0 / yChar);
	if (max < 0){
		max = 0;
	}
	return max;
}


/* checks if the project start date and end date are valid. prints an error message if true.
adjust the errorBox accordingly
*/
bool projectDateHasErrors(HWND hWnd, HDC hdc){
	int top = 26; //where to write the error
	std::string startString = textBoxToString(hWnd, PROJECT_START_ID);
	if (!isValidDate(startString)){
		drawNewError("invalid date", hdc, top, top + 20, 130, 230, RGB(255, 0, 0), BACKGROUND_COLOR, &errorBox, si.nPos * yChar);
		return true;
	}
	Day projectStart = Day(startString);
	std::string endString = textBoxToString(hWnd, PROJECT_END_ID);
	if (!isValidDate(endString)){
		drawNewError("invalid date", hdc, top, top + 20, 405, 505, RGB(255, 0, 0), BACKGROUND_COLOR, &errorBox, si.nPos * yChar);
		return true;
	}
	Day projectEnd = Day(endString);
	if (projectEnd.compare(&projectStart) < 0){
		drawNewError("end date comes before start date", hdc, top, top + 20, 130, 350, RGB(255, 0, 0), BACKGROUND_COLOR, &errorBox, si.nPos * yChar);
		return true;
	}
	return false;
}

void deleteBins(HDC hdc, COLORREF backgroundColor){
	SelectObject(hdc, GetStockObject(DC_PEN));
	SelectObject(hdc, GetStockObject(DC_BRUSH));
	SetDCBrushColor(hdc, backgroundColor);
	SetDCPenColor(hdc, backgroundColor);
	for (int i = 0; i < madeBins.size(); i++){
		Rectangle(hdc, madeBins[i].rect.left, madeBins[i].rect.top - yChar * si.nPos, madeBins[i].rect.right, madeBins[i].rect.bottom - yChar * si.nPos);
	}
	madeBins.clear();
}



std::string getBeginningOfNextMonth(std::string date){
	int month = std::stoi(date.substr(0, 2));
	int year = std::stoi(date.substr(6, 4));
	std::string toReturn;
	if (month == 12){
		std::string nextYear = intToString(year + 1);
		while (nextYear.length() < 4) {
			nextYear = "0" + nextYear;
		}
		toReturn = "01/01/" + nextYear;
	}
	else{
		std::string nextMonth = intToString(month + 1);
		while (nextMonth.length() < 2) {
			nextMonth = "0" + nextMonth;
		}
		toReturn = nextMonth + "/01/" + date.substr(6, 4);
	}
	return toReturn;
}
std::string getEndOfThisMonth(std::string date){
	int month = std::stoi(date.substr(0, 2));
	int year = std::stoi(date.substr(6, 4));
	std::string lastDay;
	if (month == 2 && year % 4 == 0){
		lastDay = "29";
	}
	else if (month == 2){
		lastDay = "28";
	}
	else if (month == 1 || month == 3 || month == 5 || month == 7 || 
		month == 8 || month == 10 || month == 12){
		lastDay = "31";
	}
	else{
		lastDay = "30";
	}
	return date.substr(0, 3) + lastDay + date.substr(5, 5);
}

void createRow(std::string strStart, std::string strEnd, HWND hWnd){
	int startX = 5;
	int endX = 150;
	int fundX = 300;
	int workUnitsX = 450;
	int deleteX = 600;
	std::vector<int> nextRow = model.addAllocationPeriod();
	int rowY = model.numAllocationPeriods() * D_BETWEEN_ROWS + 50 - yChar * si.nPos;
	std::wstring stemp = std::wstring(strStart.begin(), strStart.end());
	LPCWSTR sw = stemp.c_str();
	std::wstring etemp = std::wstring(strEnd.begin(), strEnd.end());
	LPCWSTR ew = etemp.c_str();
	CreateWindow(TEXT("Edit"), sw, WS_CHILD | WS_VISIBLE | WS_BORDER, 
		startX, rowY, 100, 20, hWnd, (HMENU)nextRow[0], NULL, NULL);
	UpdateWindow(GetDlgItem(hWnd, nextRow[0]));
	CreateWindow(TEXT("Edit"), ew, WS_CHILD | WS_VISIBLE | WS_BORDER,
		endX, rowY, 100, 20, hWnd, (HMENU)nextRow[1], NULL, NULL);
	UpdateWindow(GetDlgItem(hWnd, nextRow[1]));
	CreateWindow(TEXT("Edit"), TEXT("Fund Allocation"), WS_CHILD | WS_VISIBLE | WS_BORDER,
		fundX, rowY, 120, 20, hWnd, (HMENU)nextRow[2], NULL, NULL);
	UpdateWindow(GetDlgItem(hWnd, nextRow[2]));
	CreateWindow(TEXT("Edit"), TEXT("Work Units"), WS_CHILD | WS_VISIBLE | WS_BORDER,
		workUnitsX, rowY, 100, 20, hWnd, (HMENU)nextRow[3], NULL, NULL);
	UpdateWindow(GetDlgItem(hWnd, nextRow[3]));
	CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("Delete Bin"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		deleteX, rowY, 150, 24, hWnd, (HMENU)nextRow[4], GetModuleHandle(NULL), NULL);
	UpdateWindow(GetDlgItem(hWnd, nextRow[4]));
}





int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	model = Model();

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

	//just set the initial error box to the zero pixel
	errorBox.rect.bottom = 0;
	errorBox.rect.top = 0;
	errorBox.rect.left = 0;
	errorBox.rect.right = 0;
	std::string empty = "";
	errorBox.text = std::wstring(empty.begin(), empty.end());
	
      


	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW | WS_VSCROLL,
		CW_USEDEFAULT, CW_USEDEFAULT,
		1100, 600,
		NULL,
		NULL,
		hInstance,
		NULL
		);
	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Oracle Bin Calculator"),
			NULL);

		return 1;
	}

	//set initial scrolling info
	si.cbSize = sizeof(si);
	si.fMask = SIF_DISABLENOSCROLL;
	si.nMin = 0;
	si.nMax = 0; //no need to scroll in the beginning
	si.nPos = 0;
	si.nPage = 5;
	SetScrollInfo(hWnd, SB_VERT, &si, true);

	// The parameters to ShowWindow explained:
	// hWnd: the value returned from CreateWindow
	// nCmdShow: the fourth parameter from WinMain
	ShowWindow(hWnd, nCmdShow);

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
	TCHAR ProjectStartDate[] = _T("Project Start Date:");
	TCHAR ProjectEndDate[] = _T("Project End Date:");
	int yPos;        // current vertical scrolling position 

	switch (message)
	{
	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);

		// Lay out the application
		TextOut(hdc, 5, 6 - yChar * si.nPos, TEXT("Project Start Date:"), _tcslen(ProjectStartDate));
		TextOut(hdc, 280, 6 - yChar * si.nPos, TEXT("Project End Date:"), _tcslen(ProjectEndDate));
		if (GetDlgItem(hWnd, CREATE_MONTHLY_BINS_ID) == NULL){
			CreateWindow(TEXT("Edit"), TEXT("MM/DD/YYYY"), WS_CHILD | WS_VISIBLE | WS_BORDER, 130, 5 - yChar * si.nPos, 100, 20,
				hWnd, (HMENU)PROJECT_START_ID, NULL, NULL);
		}
		if (GetDlgItem(hWnd, CREATE_CUSTOM_BINS_ID) == NULL){
			CreateWindow(TEXT("Edit"), TEXT("MM/DD/YYYY"), WS_CHILD | WS_VISIBLE | WS_BORDER, 405, 5 - yChar * si.nPos, 100, 20,
				hWnd, (HMENU)PROJECT_END_ID, NULL, NULL);
		}
		//create the buttons
		if (GetDlgItem(hWnd, CREATE_MONTHLY_BINS_ID) == NULL){
			CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("Create Monthly Allocations"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				80, 50 - yChar * si.nPos, 200, 24, hWnd, (HMENU)CREATE_MONTHLY_BINS_ID, GetModuleHandle(NULL), NULL);
		}
		if (GetDlgItem(hWnd, CREATE_CUSTOM_BINS_ID) == NULL){
			CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("Create Custom Allocations"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				350, 50 - yChar * si.nPos, 200, 24, hWnd, (HMENU)CREATE_CUSTOM_BINS_ID, GetModuleHandle(NULL), NULL);
		}
		//redraw the bins if they exist (needed for scrolling)
		for (int i = 0; i < madeBins.size(); i++){
			RECT temp = madeBins[i].rect;
			temp.bottom = temp.bottom - yChar * si.nPos;
			temp.top = temp.top - yChar * si.nPos;
			LPCWSTR sw = madeBins[i].text.c_str();
			DrawText(hdc, sw, -1, &temp, DT_LEFT);
		}
		
		// End application-specific layout section.

		EndPaint(hWnd, &ps);
		UpdateWindow(hWnd);
		break;
	}
	case WM_COMMAND:
	{
		int commandID = LOWORD(wParam); //can't use switch because not all ID's are constants
		if (commandID == CREATE_MONTHLY_BINS_ID || commandID == CREATE_CUSTOM_BINS_ID){
			
			hdc = GetDC(hWnd);
			//get rid of bins in case they exist
			deleteBins(hdc, BACKGROUND_COLOR);
			SetTextColor(hdc, RGB(255, 0, 0));
			//get rid of any error messages
			drawNewError("", hdc, 0, 0, 0, 0, BACKGROUND_COLOR, BACKGROUND_COLOR, &errorBox, si.nPos * yChar);
			//check the dates for errors
			if (projectDateHasErrors(hWnd, hdc)){
				break; //no point in continuing
			}
			//Make sure the rest is clear
			if (GetDlgItem(hWnd, ADD_BIN_ID) != NULL){
				DestroyWindow(GetDlgItem(hWnd, ADD_BIN_ID));
			}
			if (GetDlgItem(hWnd, CALCULATE_BINS_ID) != NULL){
				DestroyWindow(GetDlgItem(hWnd, CALCULATE_BINS_ID));
			}
			if (GetDlgItem(hWnd, CALCULATE_BINS_PROPORTIONALLY_ID) != NULL){
				DestroyWindow(GetDlgItem(hWnd, CALCULATE_BINS_PROPORTIONALLY_ID));
			}
			model.deleteAllBoxes(hWnd);
			//now its time to make the bins (only if creating monthly bins)
			std::string startString = textBoxToString(hWnd, PROJECT_START_ID);
			std::string endString = textBoxToString(hWnd, PROJECT_END_ID);;
			Day projectEnd = Day(endString);
			if (commandID == CREATE_MONTHLY_BINS_ID){
				std::string strStart = startString;
				std::string strEnd = getEndOfThisMonth(startString);
				Day nextEnd = Day(strEnd);
				Day nextStart = Day(startString);
				int count = 0;
				while (nextEnd.compare(&projectEnd) < 0){
					count++;
					createRow(strStart, strEnd, hWnd);
					strStart = getBeginningOfNextMonth(strEnd);
					strEnd = getEndOfThisMonth(strStart);
					nextStart = Day(strStart);
					nextEnd = Day(strEnd);
				}

				//add one more row for the project end
				createRow(strStart, endString, hWnd);
				
			}//now add the the buttons calculate bins, calculate bins proportionally, and add
			int addY = model.numAllocationPeriods() * D_BETWEEN_ROWS + 100 - si.nPos * yChar;
			CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("Add Custom Allocation"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				200, addY, 200, 24, hWnd, (HMENU)ADD_BIN_ID, GetModuleHandle(NULL), NULL);
			int calculateY = addY + 50;
			CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("Calculate Bins For Oracle"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				100, calculateY, 200, 24, hWnd, (HMENU)CALCULATE_BINS_ID, GetModuleHandle(NULL), NULL);
			CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("Calculate Bins Weighted By Work Units"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				400, calculateY, 270, 24, hWnd, (HMENU)CALCULATE_BINS_PROPORTIONALLY_ID, GetModuleHandle(NULL), NULL);
			//add scroll space
			si.nMax = getMaxScroll(hWnd);
			UpdateWindow(hWnd);
		}
		else if (commandID == ADD_BIN_ID){

			//always call this in case the bins need to be deleted.
			//doesn't hurt not to call it
			hdc = GetDC(hWnd);
			deleteBins(hdc, BACKGROUND_COLOR);
			//shift the three buttons on the bottom down
			RECT prevAdd;
			RECT prevCalc;
			RECT prevProp;
			GetWindowRect(GetDlgItem(hWnd, ADD_BIN_ID), &prevAdd);
			GetWindowRect(GetDlgItem(hWnd, CALCULATE_BINS_ID), &prevCalc);
			GetWindowRect(GetDlgItem(hWnd, CALCULATE_BINS_PROPORTIONALLY_ID), &prevProp);
			MapWindowPoints(NULL, hWnd, (LPPOINT)&prevAdd, 2);
			MapWindowPoints(NULL, hWnd, (LPPOINT)&prevCalc, 2);
			MapWindowPoints(NULL, hWnd, (LPPOINT)&prevProp, 2);
			MoveWindow(GetDlgItem(hWnd, CALCULATE_BINS_ID), prevCalc.left, prevCalc.top + D_BETWEEN_ROWS,
				prevCalc.right - prevCalc.left, prevCalc.bottom - prevCalc.top, true);
			MoveWindow(GetDlgItem(hWnd, CALCULATE_BINS_PROPORTIONALLY_ID), prevProp.left, prevProp.top + D_BETWEEN_ROWS,
				prevProp.right - prevProp.left, prevProp.bottom - prevProp.top, true);
			MoveWindow(GetDlgItem(hWnd, ADD_BIN_ID), prevAdd.left, prevAdd.top + D_BETWEEN_ROWS, 
				prevAdd.right - prevAdd.left, prevAdd.bottom - prevAdd.top, true);
			
			//now add the row
			createRow("MM/DD/YYYY", "MM/DD/YYYY", hWnd);
			UpdateWindow(hWnd);
			//update scroll max. 50 is the size of each row
			si.nMax = getMaxScroll(hWnd);
		}
		else if (model.isDeleteID(commandID)){
			hdc = GetDC(hWnd);
			deleteBins(hdc, BACKGROUND_COLOR);
			model.deleteAllocationPeriod(commandID, hWnd);
			//shift the three buttons on the bottom up
			RECT prevAdd;
			RECT prevCalc;
			RECT prevProp;
			GetWindowRect(GetDlgItem(hWnd, ADD_BIN_ID), &prevAdd);
			GetWindowRect(GetDlgItem(hWnd, CALCULATE_BINS_ID), &prevCalc);
			GetWindowRect(GetDlgItem(hWnd, CALCULATE_BINS_PROPORTIONALLY_ID), &prevProp);
			MapWindowPoints(NULL, hWnd, (LPPOINT)&prevAdd, 2);
			MapWindowPoints(NULL, hWnd, (LPPOINT)&prevCalc, 2);
			MapWindowPoints(NULL, hWnd, (LPPOINT)&prevProp, 2);

			MoveWindow(GetDlgItem(hWnd, ADD_BIN_ID), prevAdd.left, prevAdd.top - D_BETWEEN_ROWS,
				prevAdd.right - prevAdd.left, prevAdd.bottom - prevAdd.top, true);
			MoveWindow(GetDlgItem(hWnd, CALCULATE_BINS_ID), prevCalc.left, prevCalc.top - D_BETWEEN_ROWS,
				prevCalc.right - prevCalc.left, prevCalc.bottom - prevCalc.top, true);
			MoveWindow(GetDlgItem(hWnd, CALCULATE_BINS_PROPORTIONALLY_ID), prevProp.left, prevProp.top - D_BETWEEN_ROWS,
				prevProp.right - prevProp.left, prevProp.bottom - prevProp.top, true);
			UpdateWindow(hWnd);
			//set scroll max
			si.nMax = getMaxScroll(hWnd);
		}
		else if (commandID == CALCULATE_BINS_ID || commandID == CALCULATE_BINS_PROPORTIONALLY_ID){
			hdc = GetDC(hWnd);
			deleteBins(hdc, BACKGROUND_COLOR);
			SetTextColor(hdc, RGB(255, 50, 0));
			//get rid of any error messages
			drawNewError("", hdc, 0, 0, 0, 0, BACKGROUND_COLOR, BACKGROUND_COLOR, &errorBox, si.nPos * yChar);
			//make sure the project start and end dates are still valid
			std::string startString = textBoxToString(hWnd, PROJECT_START_ID);
			std::string endString = textBoxToString(hWnd, PROJECT_END_ID);
			if (projectDateHasErrors(hWnd, hdc) || model.hasErrors(hdc, hWnd, RGB(255, 0, 0), startString, endString, &errorBox, BACKGROUND_COLOR,si.nPos * yChar)){
				break; //no point in continuing
			}
			if (commandID == CALCULATE_BINS_PROPORTIONALLY_ID){
				if (model.hasWorkUnitErrors(hdc, hWnd, RGB(255, 0, 0), &errorBox, BACKGROUND_COLOR, si.nPos * yChar)){
					break; //no point in continuing
				}
			}
			std::vector<double> bins = model.calculateBins(hWnd, startString, endString, commandID == CALCULATE_BINS_PROPORTIONALLY_ID);
			int sum2 = 0.0;
			for (int i = 0; i < bins.size(); i++){
				
				sum2 += (int)(round(10*bins[i]));
			}
			if (sum2 != 1000){
				bins[5] += 100;
			}
			int binsYposition = model.numAllocationPeriods() * 50 + 200;
			RECT toWrite;
			toWrite.bottom = binsYposition + 50;
			toWrite.top = binsYposition;
			toWrite.left = 5; //initially
			toWrite.right = 45; //initially
			SetTextColor(hdc, RGB(0, 0, 0));
			for (int i = 0; i < bins.size(); i++){
				std::string next = doubleToString(bins[i]);
				std::wstring stemp = std::wstring(next.begin(), next.end());
				LPCWSTR sw = stemp.c_str();
				//want to write to adjust for scrolling
				RECT temp = toWrite;
				temp.bottom -= si.nPos * yChar;
				temp.top -= si.nPos * yChar;
				DrawText(hdc, sw, -1, &temp, DT_LEFT);
				//add it to the madeBins
				SavedText saveThis;
				saveThis.rect = toWrite;
				saveThis.text = stemp;
				madeBins.push_back(saveThis);
				toWrite.left += 50;
				toWrite.right += 50;
			}
			UpdateWindow(hWnd);
		}
	}
	break;

	case WM_VSCROLL:
		// Get all the vertial scroll bar information.
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		hdc = GetDC(hWnd);

		// Save the position for comparison later on.
		yPos = si.nPos;
		//set the new max

		switch (LOWORD(wParam))
		{

			// User clicked the HOME keyboard key.
		case SB_TOP:
			si.nPos = si.nMin;
			break;

			// User clicked the END keyboard key.
		case SB_BOTTOM:
			si.nPos = si.nMax;
			break;

			// User clicked the top arrow.
		case SB_LINEUP:
			if (si.nPos > si.nMin){
				si.nPos -= 1;
			}
			break;

			// User clicked the bottom arrow.
		case SB_LINEDOWN:
			if (si.nPos < si.nMax){
				si.nPos += 1;
			}
			break;

			// User clicked the scroll bar shaft above the scroll box.
		case SB_PAGEUP:
			if (si.nPos >= (int)si.nPage + (int)si.nMin){
				si.nPos -= si.nPage;
			}
			else{ //very close to minimum
				si.nPos = si.nMin;
			}
			break;

			// User clicked the scroll bar shaft below the scroll box.
		case SB_PAGEDOWN:
			if (si.nPos <= (int)si.nMax - (int)si.nPage){
				si.nPos += si.nPage;
			}
			else{ //very close to maximum
				si.nPos = si.nMax;
			}
			break;

			// User dragged the scroll box.
		/*case SB_THUMBTRACK:
			si.nTrackPos = HIWORD(wParam);
			if (si.nTrackPos > si.nMax){
				si.nTrackPos = si.nMax;
			}
			si.nPos = si.nTrackPos;
			break;*/


		case SB_THUMBPOSITION:
			si.nPos = HIWORD(wParam);
			if (si.nPos > si.nMax){
				si.nPos = si.nMax;
			}
			else if (si.nPos < si.nMin){
				si.nPos = si.nMin;
			}

			break;
		default:
			break;
		}

		// Set the position and then retrieve it.  Due to adjustments
		// by Windows it may not be the same as the value set.
		si.fMask = SIF_POS;
		SetScrollInfo(hWnd, SB_VERT, &si, true); 
		SetScrollRange(hWnd, SB_VERT, si.nMin, si.nMax, true); //SetScrollInfo doesn't set range for some reason
		GetScrollInfo(hWnd, SB_VERT, &si);

		// If the position has changed, scroll window and update it.
		if (si.nPos != yPos)
		{
			ScrollWindow(hWnd, 0, yChar * (yPos - si.nPos), NULL, NULL);
			//redraw error text
			SetTextColor(hdc, RGB(255, 0, 0));
			RECT temp = errorBox.rect;
			temp.top -= si.nPos * yChar;
			temp.bottom -= si.nPos * yChar;
			DrawText(hdc, errorBox.text.c_str(), -1, &temp, DT_LEFT);
			UpdateWindow(hWnd);
		}

		return 0;

	case WM_SIZE:
	{
		hdc = GetDC(hWnd);
		int nHeight = HIWORD(lParam);
		int neededY = 250 + D_BETWEEN_ROWS * model.numAllocationPeriods();
		int scrollRange = (neededY - nHeight) / 5;
		if ((neededY - nHeight) % 5 != 0){
			scrollRange++;
		}
		si.nMax = scrollRange;
		SetScrollRange(hWnd, SB_VERT, si.nMin, si.nMax, true);
		//redraw error text
		SetTextColor(hdc, RGB(255, 0, 0));
		RECT temp = errorBox.rect;
		temp.top -= si.nPos * yChar;
		temp.bottom -= si.nPos * yChar;
		DrawText(hdc, errorBox.text.c_str(), -1, &temp, DT_LEFT);
		UpdateWindow(hWnd);
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