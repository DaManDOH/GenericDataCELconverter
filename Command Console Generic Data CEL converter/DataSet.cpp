#include "DataSet.h"

const std::wstring & DataSet::getDataSetName() const {
	return name;
}

unsigned int DataSet::getDataRowCount() const {
	return (unsigned int)data.size();
}

unsigned int DataSet::getFirstDataElementPos() const {
	return startPos;
}

unsigned int DataSet::getNextDataSetPos() const {
	return posNext;
}

const std::vector<DataHeaderParameter> & DataSet::getHeaderParams() const {
	return params;
}

const std::vector<ColumnMetadata> & DataSet::getColumnsMetadata() const {
	return columnMeta;
}

const std::vector<unsigned char> & DataSet::getFlattenedDataRows() const {
	return data;
}

std::wstring & DataSet::setDataSetName() {
	return name;
}

void DataSet::setFirstDataElementPos(unsigned int newStartPos) {
	startPos = newStartPos;
}

void DataSet::setNextDataSetPos(unsigned int newNextPos) {
	posNext = newNextPos;
}

std::vector<DataHeaderParameter> & DataSet::setHeaderParams() {
	return params;
}

std::vector<ColumnMetadata> & DataSet::setColumnsMetadata() {
	return columnMeta;
}

std::vector<unsigned char> & DataSet::setFlattenedDataRows() {
	return data;
}

