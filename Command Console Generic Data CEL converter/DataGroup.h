#ifndef __GENERIC_CEL_FILE_DATA_GROUP__
#define __GENERIC_CEL_FILE_DATA_GROUP__

//#include <iostream>
#include <string>
#include <vector>

#include "DataSet.h"

class DataGroup {
public:
	/*
	*/
	unsigned int getNextDataGroupPos() const;

	/*
	*/
	unsigned int getStartPos() const;

	/*
	*/
	int getDataSetCount() const;

	/*
	*/
	const std::wstring & getDataGroupName() const;

	/*
	*/
	const std::vector<DataSet> & getDataSets() const;
	

	/*
	*/
	void setNextDataGroupPos(unsigned int);

	/*
	*/
	void setStartPos(unsigned int);

	/*
	*/
	std::wstring & setDataGroupName();

	/*
	*/
	std::vector<DataSet> & setDataSets();

private:
	unsigned int _posNext;
	unsigned int _startPos;
	std::wstring _name;
	std::vector<DataSet> _dataSets;
};

std::ostream & operator<<(std::ostream &, const DataGroup &);

#endif /* __GENERIC_CEL_FILE_DATA_GROUP__ */

