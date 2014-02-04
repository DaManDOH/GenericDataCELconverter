#include "DataGroup.h"

unsigned int DataGroup::getNextDataGroupPos() const {
	return _posNext;
}

unsigned int DataGroup::getStartPos() const {
	return _startPos;
}

int DataGroup::getDataSetCount() const {
	return (int)_dataSets.size();
}

const std::wstring & DataGroup::getDataGroupName() const {
	return _name;
}

const std::vector<DataSet> & DataGroup::getDataSets() const {
	return _dataSets;
}

void DataGroup::setNextDataGroupPos(unsigned int newNextPos) {
	_posNext = newNextPos;
}

void DataGroup::setStartPos(unsigned int newStartPos) {
	_startPos = newStartPos;
}

std::wstring & DataGroup::setDataGroupName() {
	return _name;
}

std::vector<DataSet> & DataGroup::setDataSets() {
	return _dataSets;
}

std::ostream & operator<<(std::ostream & lhs_sout, const DataGroup & rhs_obj) {
	std::string dataGroupName(rhs_obj._name.cbegin(), rhs_obj._name.cend());
	lhs_sout << '"' << dataGroupName << "\"\n";
	std::vector<DataSet>::const_iterator oneDataSet, allDataSetsEnd;
	oneDataSet = rhs_obj._dataSets.begin();
	allDataSetsEnd = rhs_obj._dataSets.end();
	for (; oneDataSet < allDataSetsEnd; oneDataSet++) {
		lhs_sout << *oneDataSet;
	}
	return lhs_sout;
}
