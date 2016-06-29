#include "Model.h"

std::string textBoxToString(HWND hWnd, int ID){
	wchar_t* startText = new TCHAR[20];
	GetDlgItemText(hWnd, ID, startText, 20);
	std::wstring wstartText(startText);
	std::string startString(wstartText.begin(), wstartText.end());
	return startString;
}

void drawNewError(std::string errorMessage, HDC hdc, int top, int bottom, int left, int right, COLORREF textColor,
	COLORREF backgroundColor, SavedText * errorBox, int scrollOffset){
	//erase previous error
	SelectObject(hdc, GetStockObject(DC_PEN));
	SelectObject(hdc, GetStockObject(DC_BRUSH));
	SetDCBrushColor(hdc, backgroundColor);
	SetDCPenColor(hdc, backgroundColor);
	Rectangle(hdc, (*errorBox).rect.left, (*errorBox).rect.top - scrollOffset, (*errorBox).rect.right, (*errorBox).rect.bottom - scrollOffset);
	//create small text underneath that says it is not a valid date
	SetTextColor(hdc, textColor);

	(*errorBox).rect.top = top; //errorBox is saved without the offset
	(*errorBox).rect.bottom = bottom;
	(*errorBox).rect.right = right;
	(*errorBox).rect.left = left;
	RECT temp = (*errorBox).rect;
	temp.top -= scrollOffset;
	temp.bottom -= scrollOffset;
	std::wstring stemp = std::wstring(errorMessage.begin(), errorMessage.end());
	(*errorBox).text = stemp;
	LPCWSTR sw = stemp.c_str();
	DrawText(hdc, sw, -1, &temp, DT_LEFT);
}

bool isDigit(char c){
	return c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5'
		|| c == '6' || c == '7' || c == '8' || c == '9';
}

bool isDouble(std::string input){
	if (input.length() == 0){
		return false;
	}
	int numDots = 0;
	for (int i = 0; i < input.length(); i++){
		if (!isDigit(input[i])){
			if (!(input[i] == '.')){
				return false;
			}
			else{
				numDots++;
			}
		}
	}
	if (numDots > 1){
		return false;
	}
	return true;
}

/* assumes an input value of <= 100
	returns a string with a double of precision to the tenths place
*/
std::string doubleToString(double d){
	int asInt = (int)(round(10 * d));
	char digits[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
	std::string toReturn = "";
	toReturn = toReturn + digits[(asInt / 1000) % 10];
	toReturn = toReturn + digits[(asInt / 100) % 10];
	toReturn = toReturn + digits[(asInt / 10) % 10];
	toReturn = toReturn + '.';
	toReturn = toReturn + digits[asInt % 10];
	if (toReturn[0] == '0'){
		toReturn = toReturn.substr(1, 5);
		if (toReturn[0] == '0'){
			toReturn = toReturn.substr(1, 4);
		}
	}
	return toReturn;
}

/**
* reports whether the given string is a valid date
*
* @param date
*            date to be checked
* @return whether or not date is in the form MM/DD/YYYY and is a valid date
*/
bool isValidDate(std::string date) {
	//check for valid length
	bool valid = date.length() == 10;
	//check its a valid month
	valid = valid && (date[0] == '0' || date[0] == '1');
	valid = valid && isDigit(date[1]);
	//check for correct forward slash placement
	valid = valid && date[2] == '/' && date[5] == '/';
	//check that year spots are digits
	for (int i = 6; i < 10; i++) {
		valid = valid && isDigit(date[i]);
	}
	//check that day spots are digits
	valid = valid && isDigit(date[3]);
	valid = valid && isDigit(date[4]);
	if (valid) {
		//check that the day is a valid day given the month
		int month = std::stoi(date.substr(0, 2));
		int day = std::stoi(date.substr(3, 2));
		int year = std::stoi(date.substr(6, 4));
		if (month == 1 || month == 3 || month == 5 || month == 7
			|| month == 8 || month == 10 || month == 12) {
			valid = valid && day <= 31;
		}
		else if (month == 4 || month == 6 || month == 9 || month == 11) {
			valid = valid && day <= 30;
		}
		else { //month == 2
			if (year % 4 == 0) { //leap year
				valid = valid && day <= 29;
			}
			else {
				valid = valid && day <= 28;
			}
		}
	}
	return valid;
}




Model::Model(){
}


int Model::numAllocationPeriods(){
	return this->allocationStarts.size();
}

std::vector<int> Model::addAllocationPeriod(){
	int startID;
	int endID;
	int fundID;
	int workUnitsID;
	int deleteID;
	if (this->allocationStarts.size() == 0){
		startID = 0;
		endID = 1;
		fundID = 2;
		workUnitsID = 3;
		deleteID = 4;
	}else{
		startID = this->allocationStarts[this->allocationStarts.size() - 1] + 100;
		endID = this->allocationEnds[this->allocationEnds.size() - 1] + 100;
		fundID = this->allocationFunds[this->allocationFunds.size() - 1] + 100;
		workUnitsID = this->allocationWorkUnits[this->allocationWorkUnits.size() - 1] + 100;
		deleteID = this->allocationDeletes[this->allocationDeletes.size() - 1] + 100;
	}
	//add the ID's to this
	this->allocationStarts.push_back(startID);
	this->allocationEnds.push_back(endID);
	this->allocationFunds.push_back(fundID);
	this->allocationWorkUnits.push_back(workUnitsID);
	this->allocationDeletes.push_back(deleteID);

	std::vector<int> toReturn = std::vector<int>(5);
	toReturn[0] = startID;
	toReturn[1] = endID;
	toReturn[2] = fundID;
	toReturn[3] = workUnitsID;
	toReturn[4] = deleteID;
	return toReturn;

}

int Model::getDeleteID(int i){
	return this->allocationDeletes[i];
}

void Model::deleteAllocationPeriod(int deleteID, HWND hDlg){
	int index;
	for (int i = 0; i < this->allocationDeletes.size(); i++){
		if (deleteID == this->allocationDeletes[i]){
			index = i;
			break;
		}
	}
	DestroyWindow(GetDlgItem(hDlg, deleteID));
	DestroyWindow(GetDlgItem(hDlg, allocationStarts[index]));
	DestroyWindow(GetDlgItem(hDlg, allocationEnds[index]));
	DestroyWindow(GetDlgItem(hDlg, allocationFunds[index]));
	DestroyWindow(GetDlgItem(hDlg, allocationWorkUnits[index]));
	allocationDeletes.erase(allocationDeletes.begin() + index);
	allocationStarts.erase(allocationStarts.begin() + index);
	allocationEnds.erase(allocationEnds.begin() + index);
	allocationFunds.erase(allocationFunds.begin() + index);
	allocationWorkUnits.erase(allocationWorkUnits.begin() + index);
	//now shift every other window up
	for (int i = index; i < allocationStarts.size(); i++){
		RECT currentStartPos;
		RECT currentEndPos;
		RECT currentFundPos;
		RECT currentWorkUnitPos;
		RECT currentDeletePos;
		GetWindowRect(GetDlgItem(hDlg, allocationStarts[i]), &currentStartPos);
		GetWindowRect(GetDlgItem(hDlg, allocationEnds[i]), &currentEndPos);
		GetWindowRect(GetDlgItem(hDlg, allocationFunds[i]), &currentFundPos);
		GetWindowRect(GetDlgItem(hDlg, allocationWorkUnits[i]), &currentWorkUnitPos);
		GetWindowRect(GetDlgItem(hDlg, allocationDeletes[i]), &currentDeletePos);
		MapWindowPoints(NULL, hDlg, (LPPOINT)&currentStartPos, 2);
		MapWindowPoints(NULL, hDlg, (LPPOINT)&currentEndPos, 2);
		MapWindowPoints(NULL, hDlg, (LPPOINT)&currentFundPos, 2);
		MapWindowPoints(NULL, hDlg, (LPPOINT)&currentWorkUnitPos, 2);
		MapWindowPoints(NULL, hDlg, (LPPOINT)&currentDeletePos, 2);

		MoveWindow(GetDlgItem(hDlg, allocationStarts[i]), currentStartPos.left, currentStartPos.top - 50,
			currentStartPos.right - currentStartPos.left, currentStartPos.bottom - currentStartPos.top, true);
		MoveWindow(GetDlgItem(hDlg, allocationEnds[i]), currentEndPos.left, currentEndPos.top - 50,
			currentEndPos.right - currentEndPos.left, currentEndPos.bottom - currentEndPos.top, true);
		MoveWindow(GetDlgItem(hDlg, allocationFunds[i]), currentFundPos.left, currentFundPos.top - 50,
			currentFundPos.right - currentFundPos.left, currentFundPos.bottom - currentFundPos.top, true);
		MoveWindow(GetDlgItem(hDlg, allocationWorkUnits[i]), currentWorkUnitPos.left, currentWorkUnitPos.top - 50,
			currentWorkUnitPos.right - currentWorkUnitPos.left, currentWorkUnitPos.bottom - currentWorkUnitPos.top, true);
		MoveWindow(GetDlgItem(hDlg, allocationDeletes[i]), currentDeletePos.left, currentDeletePos.top - 50,
			currentDeletePos.right - currentDeletePos.left, currentDeletePos.bottom - currentDeletePos.top, true);
	}
}

bool Model::isDeleteID(int ID){
	for (int i = 0; i < this->allocationDeletes.size(); i++){
		if (ID == allocationDeletes[i]){
			return true;
		}
	}
	return false;
}

void Model::deleteAllBoxes(HWND hDlg){
	while (allocationStarts.size() > 0){
		DestroyWindow(GetDlgItem(hDlg, allocationDeletes[0]));
		DestroyWindow(GetDlgItem(hDlg, allocationStarts[0]));
		DestroyWindow(GetDlgItem(hDlg, allocationEnds[0]));
		DestroyWindow(GetDlgItem(hDlg, allocationFunds[0]));
		DestroyWindow(GetDlgItem(hDlg, allocationWorkUnits[0]));
		allocationDeletes.erase(allocationDeletes.begin() + 0);
		allocationStarts.erase(allocationStarts.begin() + 0);
		allocationEnds.erase(allocationEnds.begin() + 0);
		allocationFunds.erase(allocationFunds.begin() + 0);
		allocationWorkUnits.erase(allocationWorkUnits.begin() + 0);
	}
}

bool Model::hasErrors(HDC hdc, HWND hWnd, COLORREF errorTextColor, std::string projectStart, 
	std::string projectEnd, SavedText * errorBox, COLORREF backgroundColor, int scrollOffset){
	ProjectAllocator project = ProjectAllocator();
	//check that all dates and numbers are valid
	for (int i = 0; i < this->allocationStarts.size(); i++){
		std::string startString = textBoxToString(hWnd, this->allocationStarts[i]);
		if (!isValidDate(startString)){
			RECT start;
			GetWindowRect(GetDlgItem(hWnd, allocationStarts[i]), &start);
			MapWindowPoints(NULL, hWnd, (LPPOINT)&start, 2);
			drawNewError("invalid date", hdc, start.bottom + 1, start.bottom + 20, start.left,
				start.left + 100, errorTextColor, backgroundColor, errorBox, scrollOffset);
			return true;
		}
		// now check the end date of this period
		std::string endString = textBoxToString(hWnd, this->allocationEnds[i]);
		if (!isValidDate(endString)){
			RECT end;
			GetWindowRect(GetDlgItem(hWnd, allocationEnds[i]), &end);
			MapWindowPoints(NULL, hWnd, (LPPOINT)&end, 2);
			drawNewError("invalid date", hdc, end.bottom + 1, end.bottom + 20, end.left,
				end.left + 100, errorTextColor, backgroundColor, errorBox, scrollOffset);
			return true;
		}
		//now check that the allocation is a double
		std::string fundString = textBoxToString(hWnd, this->allocationFunds[i]);
		if (!isDouble(fundString)){
			RECT fund;
			GetWindowRect(GetDlgItem(hWnd, allocationFunds[i]), &fund);
			MapWindowPoints(NULL, hWnd, (LPPOINT)&fund, 2);
			drawNewError("not a valid number", hdc, fund.bottom + 1, fund.bottom + 20, fund.left,
				fund.left + 150, errorTextColor, backgroundColor, errorBox, scrollOffset);
			return true;
		}
		
		//now check to make sure the allocation end date is after the start date
		Day startDay = Day(startString);
		Day endDay = Day(endString);
		if (startDay.compare(&endDay) > 0){
			RECT start;
			GetWindowRect(GetDlgItem(hWnd, allocationStarts[i]), &start);
			MapWindowPoints(NULL, hWnd, (LPPOINT)&start, 2);
			drawNewError("end date comes before start date", hdc, start.bottom + 1, start.bottom + 20, start.left,
				start.left + 250, errorTextColor, backgroundColor, errorBox, scrollOffset);
			return true;
		}
		//add this make sure this has no overlap with other periods
		if (project.noOverlap(startString, endString)){
			project.allocateFunds(startString, endString, 0, 0);
		}
		else{
			RECT start;
			GetWindowRect(GetDlgItem(hWnd, allocationStarts[i]), &start);
			MapWindowPoints(NULL, hWnd, (LPPOINT)&start, 2);
			drawNewError("this period overlaps with another period", hdc, start.bottom + 1, start.bottom + 20, start.left,
				start.left + 300, errorTextColor, backgroundColor, errorBox, scrollOffset);
			return true;
		}

	}
	return false;
}

bool Model::hasWorkUnitErrors(HDC hdc, HWND hWnd, COLORREF errorTextColor, SavedText * errorBox, COLORREF backgroundColor, int scrollOffset){
		// now check the work units
	for (int i = 0; i < this->allocationWorkUnits.size(); i++){
		std::string workString = textBoxToString(hWnd, this->allocationWorkUnits[i]);
		if (!isDouble(workString)){ //allowed to be empty
			RECT work;
			GetWindowRect(GetDlgItem(hWnd, allocationWorkUnits[i]), &work);
			MapWindowPoints(NULL, hWnd, (LPPOINT)&work, 2);
			drawNewError("not a valid number", hdc, work.bottom + 1, work.bottom + 20, work.left,
				work.left + 100, errorTextColor, backgroundColor, errorBox, scrollOffset);
			return true;
		}
	}
	return false;
}


std::vector<double> Model::calculateBins(HWND hWnd, std::string projectStart, std::string projectEnd, bool proportionally){

	ProjectAllocator project = ProjectAllocator(projectStart, projectEnd);
	//add the allocation periods to the project
	for (int i = 0; i < this->allocationStarts.size(); i++){
		std::string startString = textBoxToString(hWnd, this->allocationStarts[i]);
		std::string endString = textBoxToString(hWnd, this->allocationEnds[i]);
		std::string fundString = textBoxToString(hWnd, this->allocationFunds[i]);
		double funds = std::stod(fundString);

		int workUnits = 0;
		if (proportionally){
			std::string workString = textBoxToString(hWnd, this->allocationWorkUnits[i]);
			workUnits = std::stod(workString);
		}
		project.allocateFunds(startString, endString, funds, workUnits);
	}
	if (proportionally){
		project.proportionalizeAllocations();
	}
	return project.getBinAllocation();
}