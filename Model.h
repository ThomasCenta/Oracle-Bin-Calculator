/*
	This is a class to store all relevant information about the GUI and perform project calculations
*/


#include "ProjectAllocator.h"
#include <Windows.h>

#ifndef MODEL_H
#define MODEL_H


struct SavedText { //this is text that wants to stay on the screen
	std::wstring text;
	RECT rect;
};

bool isValidDate(std::string); //define these here so OracleBins.cpp can also use it
void drawNewError(std::string errorMessage, HDC hdc, int top, int bottom, int left, int right, 
	COLORREF textColor,COLORREF backgroundColor, SavedText * errorBox, int scrollOffset);
std::string doubleToString(double input);

/* Restricts the string to 20 characters
*/
std::string textBoxToString(HWND hWnd, int ID);

class Model{

public:
	
	/*Default constructor
	*/
	Model();

	/*checks whether the given date is within the project time frame
	@requires date is a valid date in MM/DD/YYYY
	*/
	bool withinProject(std::string date);

	/*Returns the number of allocation periods in this
	*/
	int numAllocationPeriods();

	/*Returns the ID of the allocation with index i in this. Works exactly like a vector
	*/
	int getDeleteID(int i);

	/*initializes an allocation period. Makes its start date, end date, delete, fund, and work units ID's and returns them
	@return the start date, end date, fund, work unit, and delete ID's in that order
	@requires no other ID's are of the form X00...000(0,1,2,3,4) (ie. 100, 101, 102, 103, 104, 200, 201, 202, 203, 204) 
		where X is an int >= 1. This is because those will be the ID's created
	*/
	std::vector<int> addAllocationPeriod();

	/* deletes the allocation period with the given delete ID. Also moves each other allocation period up by 50 pixels
	@requires deleteID is an ID in allocationDeletes
	*/
	void deleteAllocationPeriod(int deleteID, HWND hDlg);

	/* deletes all windows in this (all allocation start dates, end dates, funds, and work units boxes)
	*/
	void deleteAllBoxes(HWND hDlg);

	/*reports whether the given ID is a a Delete ID in this
	*/
	bool isDeleteID(int ID);

	/* checks for any errors in the allocation periods. Returns true if one is found and writes a message for the error.
		returns after finding one error. will not list more. sets the errorBox corrdinates to the error posted
		@requires projectStart and projectEnd are valid dates
	*/
	bool hasErrors(HDC hdc, HWND hWnd, COLORREF errorTextColor, std::string projectStart, std::string projectEnd, 
		SavedText * errorBox, COLORREF backgroundColor, int scrollOffset);

	/* checks for any errors in the Work Units. Returns true if one is found and writes a message for the error.
	returns after finding one error. will not list more. sets the errorBox coordinates to the error posted
	*/
	bool hasWorkUnitErrors(HDC hdc, HWND hWnd, COLORREF errorTextColor, SavedText * errorBox, COLORREF backgroundColor, int scrollOffset);

	/* calculates the bins given the allocation periods and returns them as a vector of doubles
	printed values will extend to an x position of about 1000
	printed values will be at a y value of about 150 below the last allocation period
	@param binsY
		the y value of the location to put the bins
	@requires the allocation periods are valid as well as their funds
	*/
	std::vector<double> calculateBins(HWND hWnd, std::string projectStart, std::string projectEnd, bool proportionally);

	


private:

	/* identifiers to the start date text boxes for allocation periods
	*/
	std::vector<int> allocationStarts;
	
	/* identifiers to the end date text boxes for allocation periods
	*/
	std::vector<int> allocationEnds;
	
	/* identifeir to the fund text boxes for allocation periods
	*/
	std::vector<int> allocationFunds;

	/* identifier to the work unit text boxes for allocation periods
	*/
	std::vector<int> allocationWorkUnits;

	/* identifier to the delete buttons for allocation periods
	*/
	std::vector<int> allocationDeletes;

};

#endif