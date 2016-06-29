/*
Implementation of the Month class
*/

#include "Month.h"

Month::Month(){}

Month::Month(std::vector<Day*> days, double fundAllocation,
	double workUnits, std::string startDate, std::string endDate) {

	this->days = days;
	this->fundAllocation = fundAllocation;
	double allocationPerDay = fundAllocation / days.size();
	//now set the funds for each day
	for (int i = 0; i < this->days.size(); i++) {
		(*this->days[i]).updateAllocation(allocationPerDay);
	}
	this->startDate = Day(startDate);
	this->endDate = Day(endDate);
	this->workUnits = workUnits;
}

void Month::setNormalizedAllocation(double totalRawFunds, double desiredFunds) {
	double ratio = desiredFunds / totalRawFunds;
	//go through and multiply all days by the ratio
	for (int i = 0; i < this->days.size(); i++) {
		double previousAllocation = (*this->days[i]).getAllocation();
		(*this->days[i]).updateAllocation(previousAllocation * ratio);
	}

}

double Month::getWorkUnits() {
	return this->workUnits;
}

double Month::getAllocation() {
	return this->fundAllocation;
}

double Month::getAllocationPerDay(){
	return this->fundAllocation / days.size();
}

Day * Month::getFirstDay() {
	return &this->startDate;
}

Day * Month::getLastDay() {
	return &this->endDate;
}
bool Month::containsDay(Day * day){
	bool toReturn = this->endDate.compare(day) >= 0;
	toReturn = toReturn && this->startDate.compare(day) <= 0;
	return toReturn;
}

void Month::setRawAllocation(double averageWorkUnits) {
	double ratio = this->workUnits / averageWorkUnits;
	this->fundAllocation = this->fundAllocation * ratio;
	double allocationPerDay = this->fundAllocation / this->days.size();
	//go through and multiply all days by the ratio
	for (int i = 0; i < this->days.size(); i++) {
		(*this->days[i]).updateAllocation(allocationPerDay);
	}
}