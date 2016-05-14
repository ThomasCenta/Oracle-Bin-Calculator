/*
	This is a class to store all relevant information about the GUI and perform project calculations
*/


#include "ProjectAllocator.h"

#ifndef MODEL_H
#define MODEL_H

class Model{

public:
	
	/*Default constructor
	*/
	Model();

	void createNewProject(std::string startDate, std::string endDate);

	/*checks whether the given time frame is within 
	*/
	bool withinProject(std::string startDate, std::string endDate);

	/* returns strings with monthly allocation periods. odd elements are start dates and following those are
	corresponding end dates. Also sets the allocation periods in this as those months
	*/
	std::vector<std::string> getMonths(std::string startDate);


private:

	/* identifiers to the start date text boxes for allocation periods
	*/
	std::vector<int> allocationStarts;
	
	/* identifiers to the end date text boxes for allocation periods
	*/
	std::vector<int> allocationEnd;
	
	/* identifeir to the fund text boxes for allocation periods
	*/
	std::vector<int> allocationFunds;

	/* identifer for the add new allocation period button
	*/
	int addButton;

};

#endif