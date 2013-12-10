#include "DataGroup.h"

unsigned int DataGroup::getNextDataGroupPos() const {
	return posNext;
}

unsigned int DataGroup::getStartPos() const {
	return startPos;
}

int DataGroup::getDataSetCount() const {
	return (int)dataSets.size();
}

const std::wstring & DataGroup::getDataGroupName() const {
	return name;
}

const std::vector<DataSet> & DataGroup::getDataSets() const {
	return dataSets;
}

void DataGroup::setNextDataGroupPos(unsigned int newNextPos) {
	posNext = newNextPos;
}

void DataGroup::setStartPos(unsigned int newStartPos) {
	startPos = newStartPos;
}

std::wstring & DataGroup::setDataGroupName() {
	return name;
}

std::vector<DataSet> & DataGroup::setDataSets() {
	return dataSets;
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
