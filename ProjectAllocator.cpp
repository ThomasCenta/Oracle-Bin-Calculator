/*
	Implementation of ProjectAllocator class
*/

#include "ProjectAllocator.h"

ProjectAllocator::ProjectAllocator(std::string startDate, std::string endDate) {
	this->fundAllocation = 0;
	this->months = std::vector<Month>();
	this->days = std::vector<Day>();
	Day nextDay = Day(startDate);
	this->startDay = nextDay;
	//get a reference to the last day
	this->lastDay = Day(endDate);
	if (!nextDay.getIsHoliday()) {
		this->days.push_back(nextDay);
	}
	while (nextDay.compare(&this->lastDay) < 0) {
		std::string nextDate = nextDay.getNextDay();
		nextDay = Day(nextDate);
		if (!nextDay.getIsHoliday()) {
			this->days.push_back(nextDay);
		}
	}

}

std::vector<double> ProjectAllocator::getBinAllocation() {
	std::vector<double> bins = std::vector<double>(21);
	bins[0] = 0; //always leave first empty
	int daysPerBin = this->days.size() / 20;
	int extraDays = this->days.size() % 20;
	int daysInBin[21];
	daysInBin[0] = 0;
	for (int i = 1; i < 21; i++) {
		daysInBin[i] = daysPerBin;
		if (i <= extraDays) {
			daysInBin[i]++; //give extra days to the first bins
		}
	}
	int cumulativeDays[21];//days before bin i + 1
	int sum = 0;
	for (int i = 0; i < 21; i++) {
		sum += daysInBin[i];
		cumulativeDays[i] = sum;
	}
	int currentBin = 0;
	for (int i = 0; i < this->days.size(); i++) {
		while (cumulativeDays[currentBin] < i + 1) {
			currentBin++;
		}
		bins[currentBin] += this->days[i].getAllocation();
	}
	//change them to percentages
	for (int i = 0; i < bins.size(); i++) {
		bins[i] = 100 * (bins[i] / this->fundAllocation);
	}
	return bins;
}

void ProjectAllocator::allocateFunds(std::string startDate, std::string endDate, double amount, double workUnits) {
	Day firstDay = Day(startDate);
	Day endDay = Day(endDate);
	//get all days in this time frame
	this->fundAllocation += amount;
	std::vector<Day *> contained;
	for (int i = 0; i < this->days.size(); i++) {
		if (this->days[i].compare(&firstDay) >= 0
			&& this->days[i].compare(&endDay) <= 0) {
			contained.push_back(&this->days[i]);
		}
	}

	Month newMonth = Month(contained, amount, workUnits, startDate, endDate);
	this->months.push_back(newMonth);

}
void ProjectAllocator::proportionalizeAllocations() {
	//first find the average number of work units
	double totalWorkUnits = 0;
	for (int i = 0; i < this->months.size(); i++) {
		totalWorkUnits += this->months[i].getWorkUnits();
	}
	double averageWorkUnits = totalWorkUnits / this->months.size();
	for (int i = 0; i < this->months.size(); i++) {
		this->months[i].setRawAllocation(averageWorkUnits);
	}
	//now normalize it so this->fundAllocation doesn't change
	//find the totalNumber of funds allocated to months
	double totalFunds = 0;
	for (int i = 0; i < this->months.size(); i++) {
		totalFunds += this->months[i].getAllocation();
	}
	for (int i = 0; i < this->months.size(); i++) {
		this->months[i].setNormalizedAllocation(totalFunds,
			this->fundAllocation);
	}
}