/**
* This is an interface for the project allocator. This class is responsible for
* taking in information such as project start and end dates, fund allocation
* over time, and amount of work in each time frame and will return the bin
* percentage needed to enter in oracle to get the same result.
*/

#include "Month.h"

#ifndef PROJECTALLOCATOR_H
#define PROJECTALLOCATOR_H

class ProjectAllocator {

public:

	/**Default constructor. Should never be used
	*/
	ProjectAllocator();

	/**
	* constructor given the start and end date of the project.
	*
	* @param startDate
	*            starting date of the project
	* @param endDate
	*            ending date of the project
	*/
	ProjectAllocator(std::string startDate, std::string endDate);

	/**
	* This allocates the given amount of funds to the time slot.
	*
	* @param startDate
	*            starting date of the time slot (inclusive)
	* @param endDate
	*            ending date of the time slot (inclusive)
	* @param amount
	*            the amount of funds to allocate to the time slot
	* @param workUnits
	*            the amount of work units to designate to this time frame
	* @requires startDate and endDate are within the project dates the given
	*           time slot does not overlap with any other time slot given
	*           startDate comes before endDate
	*/
	void allocateFunds(std::string startDate, std::string endDate, double amount,
		double workUnits);

	/**
	* This returns the necessary input into the given number of bins to get the
	* curve in oracle necessary to get as close as possible to the given funds
	* allocated thus far in the project.
	*
	* @return an array of size 21 that gives the percent allocation into each
	*         bin the first bin is automatically size zero
	*/
	std::vector<double> getBinAllocation();

	/**
	* modifies each months allocation to give more weight to months with higher
	* work units and vice versa.
	*/
	void proportionalizeAllocations();

	/*checks whether the given time frame overlaps with any allocation periods in this.
	@requires startDate and endDate are valid dates in MM/DD/YYYY
	*/
	bool noOverlap(std::string startDate, std::string endDate);

	/*checks whether the given date is within the project time frame
	@requires date is a valid date in MM/DD/YYYY
	*/
	bool withinProject(std::string date);

private:
	/**
	* the first day in this.
	*/
	Day startDay;

	/**
	* the last day in this.
	*/
	Day lastDay;

	/**
	* ordered list of days in this (not including non-working days).
	*/
	std::vector<Day> days;

	/**
	* list of the months to which funds have been allocated.
	*/
	std::vector<Month> months;

	/**
	* amount of funds allocated to this.
	*/
	double fundAllocation;

};


#endif
