/** @author: Thomas Centa
This is an interface for the day. This class contains a date and
and stores the specified amount of funds allocated to that day. This will
also perform relevant operations such as telling whether or not this day is a
holiday
*/


#include <stdlib.h>
#include <string>


#ifndef DAY_H
#define DAY_H

class Day{

public:

	/*Default constructor. Should never be called.
	*/
	Day();

	/**
	* Constructor with date argument.
	*
	* @param input
	* @requires input is in the form MM/DD/YYYY and is a valid date
	* @ensures isHoliday is set if date is a common non-work day as described
	*          in interface fundAllocation is set to zero
	*/
	Day(std::string input);

	/**
	* This updates the allocation for this.
	*
	* @param amount
	*            the allocation allotted for this day
	* @updates this
	*/
	void updateAllocation(double amount);

	/**
	* returns the amount allocated to this day.
	*
	* @return the amount allocated in this
	*/
	double getAllocation();

	/**
	* reports whether the date of this is a holiday (non-working day). from
	* Christmas eve to day after new years (inclusive), MLK day, Memorial Day,
	* Independence day, Labor Day, and Thanksgiving and the day after
	* Thanksgiving
	*
	* @return whether or not this is a holiday
	*
	*/
	bool getIsHoliday();

	/**
	* Reports whether this is in the range [startDate, endDate].
	*
	* @param otherDay
	*            otherDay to compare this to
	* @requires otherDay does not point to null
	* @return -1 if this comes before otherDay, 0 if same day, 1 is this comes
	*         after
	*/
	int compare(Day * otherDay);

	/**
	* reports the month of this.
	*
	* @return the month of this
	*/
	int getMonth();

	/**
	* reports the day of this.
	*
	* @return the day of this
	*/
	int getDay();

	/**
	* reports the year of this.
	*
	* @return the year of this
	*/
	int getYear();

	/**
	* Gives a string representation in MM/DD/YYYY of the date after this.
	*
	* @return the date that comes after this
	*/
	std::string getNextDay();

private:
	/**
	* the funds allocated to this day.
	*/
	double fundAllocation;

	/**
	* the string representation of this
	*/
	std::string date;

	/**
	* the month of this date.
	*/
	int month;

	/**
	* the day of the month of this.
	*/
	int day;

	/**
	* the year of the date of this.
	*/
	int year;

	/**
	* holds whether or not this is a common non-work day.
	*/
	bool isHoliday;

};

#endif