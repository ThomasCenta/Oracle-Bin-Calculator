/**
* Implementation of the Day class
*
* @author Thomas Centa
*
*/
#include "Day.h"


/**
*
* @param day
*            day from 1-last day of month
* @param month
*            month from 1-12
* @param year
*            any non-negative integer
* @return whether the given date is a holiday listed in interface
*/
static bool calculateHoliday(int day, int month, int year) {
	/*
	* as a reference, January 1st 2016 was a Friday. Every year after is
	* one day ahead, two if a leap year (every 4th year)
	*/
	//array with days until that month
	int daysUntilMonth[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273,
		304, 334 };
	//this will have 0 as monday, 6 as sunday. Start as first day in year
	int categoricalDay = 4 + 365 * (year - 2016);
	//modify day for leap years
	categoricalDay += (year - 2016) / 4;
	if (year > 2016 && year % 4 != 0) {
		categoricalDay += 1;//need to add one more in this case for 2016
	}
	//now add days for the month and day
	categoricalDay += daysUntilMonth[month - 1] + day - 1; //day in 2016
	//add a day if its a leap year and its past February
	if (year % 4 == 0 && month > 2) {
		categoricalDay += 1;
	}
	//now use clock arithmetic to figure out the day
	categoricalDay = categoricalDay % 7;
	if (categoricalDay < 0) {
		categoricalDay += 7;
	}
	//if its a weekend, say yes
	bool weekend = categoricalDay >= 5;
	//go through the holidays
	bool christmasVacation = month == 12 && day >= 25;
	christmasVacation = christmasVacation || month == 1 && day <= 2;
	bool mlkDay = month == 1 && categoricalDay == 0
		&& (day - 1) / 7 == 2;
	bool memorial = month == 5 && categoricalDay == 0 && day >= 25;
	bool julyFourth = month == 7 && day == 4;
	bool laborDay = month == 9 && categoricalDay == 0 && day <= 7;
	bool thanksgiving = month == 11
		&& (categoricalDay == 3 || categoricalDay == 4)
		&& (day - 1) / 7 == 3;
	//if any of these are true then return this date as a holiday
	return weekend || christmasVacation || mlkDay || memorial || julyFourth
		|| laborDay || thanksgiving;

}

Day::Day(){}


Day::Day(std::string input) {
	this->month = std::stoi(input.substr(0, 2));
	this->day = std::stoi(input.substr(3, 2));
	this->year = std::stoi(input.substr(6, 4));
	this->isHoliday = calculateHoliday(this->day, this->month, this->year);
	this->date = input;
	this->fundAllocation = 0;
}

void Day::updateAllocation(double amount) {
	this->fundAllocation = amount;
}

double Day::getAllocation() {
	return this->fundAllocation;
}

bool Day::getIsHoliday() {
	return this->isHoliday;
}

int Day::compare(Day * otherDay) {
	int toReturn = 0;
	if (this->year > (*otherDay).getYear()) {
		toReturn = 1;
	}
	else if (this->year < (*otherDay).getYear()) {
		toReturn = -1;
	}
	else if (this->month >(*otherDay).getMonth()) {
		toReturn = 1;
	}
	else if (this->month < (*otherDay).getMonth()) {
		toReturn = -1;
	}
	else if (this->day < (*otherDay).getDay()) {
		toReturn = -1;
	}
	else if (this->day >(*otherDay).getDay()) {
		toReturn = 1;
	}
	else {
	} //they are equal
	return toReturn;
}

int Day::getMonth() {
	return this->month;
}

int Day::getDay() {
	return this->day;
}

int Day::getYear() {
	return this->year;
}

std::string Day::getNextDay() {
	std::string toReturn = "";
	if (this->month == 12 && this->day == 31) {
		std::string nextYear = "" + (this->year + 1);
		while (nextYear.length() < 4) {
			nextYear = "0" + nextYear;
		}
		toReturn += "01/01/" + nextYear;
	}
	else if ((this->month == 1 || this->month == 3 || this->month == 5
		|| this->month == 7 || this->month == 8 || this->month == 10
		|| this->month == 12) && this->day == 31) {

		std::string nextMonth = (this->month + 1) + "";
		while (nextMonth.length() < 2) {
			nextMonth = "0" + nextMonth;
		}
		toReturn += nextMonth + "/01/" + this->date.substr(6, 4);

	}
	else if ((this->month == 4 || this->month == 6 || this->month == 9
		|| this->month == 11) && this->day == 30) {

		std::string nextMonth = (this->month + 1) + "";
		while (nextMonth.length() < 2) {
			nextMonth = "0" + nextMonth;
		}
		toReturn += nextMonth + "/01/" + this->date.substr(6, 4);

	}
	else if (this->month == 2 && this->year % 4 == 0 && this->day == 29) { //month == 2

		std::string nextMonth = (this->month + 1) + "";
		while (nextMonth.length() < 2) {
			nextMonth = "0" + nextMonth;
		}
		toReturn += nextMonth + "/01/" + this->date.substr(6, 4);
		;

	}
	else if (this->month == 2 && this->year % 4 != 0 && this->day == 28) { //month == 2

		std::string nextMonth = (this->month + 1) + "";
		while (nextMonth.length() < 2) {
			nextMonth = "0" + nextMonth;
		}
		toReturn += nextMonth + "/01/" + this->date.substr(6, 4);

	}
	else {
		std::string nextDay = "" + (this->day + 1);
		while (nextDay.length() < 2) {
			nextDay = "0" + nextDay;
		}
		toReturn += this->date.substr(0, 3) + nextDay
			+ this->date.substr(5, 5);

	}
	return toReturn;
}