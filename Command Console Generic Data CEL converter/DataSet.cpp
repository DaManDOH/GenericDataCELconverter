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

void DataSet::getFlattenedDataRowsAsFloat(std::vector<float> & dataVector) const {
	int numFloats = data.size() / 4;
	unsigned char oneFloatBuff[4];
	float * oneFloatPtr;
	for (int i = 0; i < numFloats; i++) {
		oneFloatBuff[0] = data.at(i*4+3);
		oneFloatBuff[1] = data.at(i*4+2);
		oneFloatBuff[2] = data.at(i*4+1);
		oneFloatBuff[3] = data.at(i*4);
		oneFloatPtr = (float*)(&oneFloatBuff[0]);
		dataVector.push_back(*oneFloatPtr);
	}
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

