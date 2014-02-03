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

std::ostream & operator<<(std::ostream & rhs_sout, const DataGroup & lhs_obj) {
	std::wstring wideDataGroupName = lhs_obj.getDataGroupName();
	std::string dataGroupName(wideDataGroupName.cbegin(), wideDataGroupName.cend());
	rhs_sout << dataGroupName;
	std::vector<DataSet>::const_iterator oneDataSet = lhs_obj.getDataSets().begin();
	std::vector<DataSet>::const_iterator allDataSetsEnd = lhs_obj.getDataSets().end();
	for (; oneDataSet < allDataSetsEnd; oneDataSet++) {
		rhs_sout << '\n' << *oneDataSet;
	}
	return rhs_sout;
}
