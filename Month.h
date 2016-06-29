/**
* This is an interface for the project allocator. This class is responsible for
* taking in information such as project start and end dates, fund allocation
* over time, and amount of work in each time frame and will return the bin
* percentage needed to enter in oracle to get the same result.
*
* This class always keeps the days it contains updated on allocation
*
*/

#include "Day.h"
#include <vector>

#ifndef MONTH_H
#define MONTH_H

class Month {

public:

	/*Default constructor. Should never be called.
	*/
	Month();


	/**
	* Constructor with days, fundAllocation, and workUnits as input.
	*
	* @param days
	*            a possibly unordered list of working days to be contained in
	*            this
	* @param startDate
	*            the start date of this period (possibly a holiday)
	* @param endDate
	*            the end date of this period possibly a holiday)
	* @param fundAllocation
	*            the funds allocation to this
	* @param workUnits
	*            the number of work units designated to this
	* @requires there are no non-working days in days
	*/
	Month(std::vector<Day *> daysInput, double fundAllocation,
		double workUnits, std::string startDate, std::string endDate);

	/**
	* This alters the funds allocated to this month based on how many work
	* units it has. If this has less units than the average, its fund
	* allocation will be reduced, and vice versa.
	* @param averageWorkUnits
	*            the average number of work units to be used as a reference.
	*/
	void setRawAllocation(double averageWorkUnits);

	/**
	* Multiplies the funds by multiplying it by a ratio such that if done on
	* all months in the project, will give the desired sum of funds.
	*
	* @param totalRawFunds
	*            current sum of all funds in the project
	* @param desiredFunds
	*            the desired sum of funds
	*
	*/
	void setNormalizedAllocation(double totalRawFunds, double desiredFunds);

	/**
	* reports the number of work units designated to this.
	*
	* @return the number of work units designated to this
	*/
	double getWorkUnits();

	/**
	* reports the number of funds allocated to this.
	*
	* @return the number of funds allocated to this
	*/
	double getAllocation();

	/**
	* returns an alias to the first day in the time period of this.
	*
	* @return an alias to the first day in the time period of this
	*/
	Day * getFirstDay();

	/**
	* returns an alias to the last day in the time period of this.
	*
	* @return an alias to the last day in the time period of this
	*/
	Day * getLastDay();

	/*
		returns whether or not the given day is in the range of this
	*/
	bool containsDay(Day * day);

	/*
	returns the fund allocation of this divided by the number of working days
	*/
	double getAllocationPerDay();


private:
	/**
	* this is the earliest date in this.
	*/
	Day startDate;

	/**
	* this is the latest date in this.
	*/
	Day endDate;

	/**
	* this is a list of all dates in this. Does not contain holidays
	*/
	std::vector<Day *> days;

	/**
	* the amount of funds allocated to this.
	*/
	double fundAllocation;

	/**
	* the amount of work units that are designated to this.
	*/
	double workUnits;

};

#endif