/*
	Implementation of ProjectAllocator class
*/

#include "ProjectAllocator.h"
#include <algorithm>





/* calculates the deviation of a solution for a system of equations
	@param equations
		vector of equations where equations are stored as vectors of doubles.
		The equations have coefficients of X1,...,X20 being percentages of that bin being allocated to that month
		where the actual variables represent the percentage of total funding allotted to that bin
		the last element in the equation vector is the percentage of total funds allocated to that month
	@param bins
		the solution to test
*/
double minFunction(std::vector<std::vector<double> > * equations, std::vector<int> * bins){
	double sum = 0;
	for (int i = 0; i < (*equations).size(); i++){
		double rowSum = (*equations)[i][(*equations)[i].size() - 1];
		for (int j = 0; j < (*equations)[i].size() - 1; j++){
			rowSum -= (*equations)[i][j] * ((*bins)[j]/10.0);
		}
		sum += std::abs(rowSum);
	}
	return sum;
}

/*
	uses a step size of 0.1
*/

std::vector<int> findBestMove(std::vector<std::vector<double> > * equations, std::vector<int> * bins){
	int step = 1;
	double currentMin = minFunction(equations, bins);
	std::vector<int> bestMove = std::vector<int>(2,1);
	for (int i = 0; i < (*bins).size(); i++){
		for (int j = 0; j < (*bins).size(); j++){
			if (i != j && (*bins)[i] >= step){
				(*bins)[i] -= step;
				(*bins)[j] += step;
				double test = minFunction(equations, bins);
				if (test < currentMin){
					currentMin = test;
					bestMove[0] = i;
					bestMove[1] = j;
				}
				(*bins)[i] += step;
				(*bins)[j] -= step;
			}
		}
	}
	return bestMove;
}

/* 
	@ updates bins
*/

void getBestBins(std::vector<std::vector<double> > * equations, std::vector<int> * bins){
	double step = 1;
	double currentMin = minFunction(equations, bins);
	double pastMin = currentMin + 1;

	while (currentMin < 0.999*pastMin){
		pastMin = currentMin;
		std::vector<int> bestMove = findBestMove(equations, bins);
		(*bins)[bestMove[0]] -= step;
		(*bins)[bestMove[1]] += step;
		currentMin = minFunction(equations, bins);
	}
}


ProjectAllocator::ProjectAllocator(){

}

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
	while (nextDay.compare(&(this->lastDay)) < 0) {
		std::string nextDate = nextDay.getNextDay();
		nextDay = Day(nextDate);
		if (!nextDay.getIsHoliday()) {
			this->days.push_back(nextDay);
		}
	}

}

std::vector<double> ProjectAllocator::getBinAllocation() {
	std::vector<double> bins = std::vector<double>(21, 0.0);
	std::vector<std::vector<double>> equations = std::vector<std::vector<double>>(this->months.size(), bins);
	double daysPerBin = this->days.size() / 20.0;
	for (int i = 0; i < this->days.size(); i++) {
		double periodStart = 0;
		double periodEnd = daysPerBin;
		for (int j = 1; j <= 20; j++){ //leave first as zero
			double stakeInDay = std::max(std::min(periodEnd - i, 1.0), 0.0) - std::min(std::max(periodStart - i, 0.0), 1.0);
			for (int k = 0; k < this->months.size(); k++){
				if (this->months[k].containsDay(&this->days[i])){
					equations[k][j - 1] += stakeInDay / daysPerBin;
				}
			}
			bins[j] += stakeInDay * this->days[i].getAllocation();
			periodStart += daysPerBin;
			periodEnd += daysPerBin;
		}
	}
	for (int i = 0; i < equations.size(); i++){//set the right hand side of the equations
		equations[i][bins.size() - 1] = 100*this->months[i].getAllocation()/this->fundAllocation;
	}
	//change them to percentages and change them to ints 0 <= bins[i] <= 1000 which represent percentages to one decimal
	std::vector<int> tempBins;
	int sum = 0;
	for (int i = 1; i < bins.size(); i++) {
		double percentage = 100 * (bins[i] / this->fundAllocation);
		tempBins.push_back(((int)(10 * percentage)));
		sum += tempBins[tempBins.size()-1];
	}
	int i = 0;
	while (sum < 1000){
		tempBins[i] += 1;
		sum += 1;
		i++;
		if (i >= tempBins.size()){ //this should never be called, but just in case;
			i = 0;
		}
	}
	//now set up the calculations. First get rid of first bin (not used)
	//get the best bins
	getBestBins(&equations, &tempBins);
	//set the bins to the best bins
	for (int i = 1; i < bins.size(); i++){
		bins[i] = tempBins[i - 1] / 10.0;
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

bool ProjectAllocator::noOverlap(std::string startDate, std::string endDate){
	Day firstDay = Day(startDate);
	Day endDay = Day(endDate);
	//make sure it does not overlap with any other previous allocations
	bool overlaps = false;
	for (int i = 0; i < this->months.size(); i++) {
		Day * monthStart = this->months[i].getFirstDay();
		Day * monthEnd = this->months[i].getLastDay();
		bool noOverlap = ((*monthStart).compare(&firstDay) < 0 && (*monthEnd).compare(&firstDay) < 0)
			|| ((*monthStart).compare(&endDay) > 0 && (*monthEnd).compare(&endDay) > 0);
		overlaps = overlaps || !noOverlap;
	}
	return !overlaps;
}

bool ProjectAllocator::withinProject(std::string date){
	Day day = Day(date);
	return day.compare(&this->startDay) >= 0 && day.compare(&this->lastDay) <= 0;
}