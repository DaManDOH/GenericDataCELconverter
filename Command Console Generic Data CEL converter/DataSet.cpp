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

const std::vector<unsigned char> & DataSet::getFlattenedDataElements() const {
	return data;
}

void DataSet::getFlattenedDataElementsAsFloat(std::vector<float> & dataVector) const {
	unsigned long numFloats = (unsigned long)data.size() / 4ul;
	dataVector.reserve(numFloats);
	unsigned char oneFloatBuff[4];
	float * oneFloatPtr;
	for (unsigned long i = 0; i < numFloats; i++) {
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

std::vector<unsigned char> & DataSet::setFlattenedDataElements() {
	return data;
}

std::ostream & operator<<(std::ostream & rhs_sout, const DataSet & lhs_obj) {
	std::string dataSetName(lhs_obj.getDataSetName().begin(), lhs_obj.getDataSetName().end());
	unsigned int numColumns = (unsigned int)lhs_obj.getColumnsMetadata().size();
	rhs_sout << dataSetName << '(' << numColumns << ')' << '[';
	std::vector<ColumnMetadata>::const_iterator oneColumnMeta = lhs_obj.getColumnsMetadata().begin();
	std::vector<ColumnMetadata>::const_iterator allMetadataEnd = lhs_obj.getColumnsMetadata().end();
	for (oneColumnMeta; oneColumnMeta < allMetadataEnd; oneColumnMeta++) {
		rhs_sout << '(';
		std::string dataSetColumnMetaName(
				oneColumnMeta->getColumnMetaName().begin(),
				oneColumnMeta->getColumnMetaName().end()
			);
		rhs_sout << dataSetColumnMetaName << ',';
		rhs_sout << oneColumnMeta->getColumnMetaTypeAsStr() << ',';
		rhs_sout << oneColumnMeta->getColumnMetaTypeSize();
		rhs_sout << ')';
	}
	rhs_sout << ']';
//#define __DCW_DATA_ELEMENT_OUTPUT__
#ifdef __DCW_DATA_ELEMENT_OUTPUT__

	std::vector<float> dataElements;
	lhs_obj.getFlattenedDataElementsAsFloat(dataElements);
	std::vector<float>::const_iterator oneDataElement = dataElements.begin();
	std::vector<float>::const_iterator allDataElementsEnd = dataElements.end();
	for (oneDataElement; oneDataElement < allDataElementsEnd; oneDataElement++) {
		rhs_sout << '\n' << *oneDataElement;
	}
#endif /* __DCW_DATA_ELEMENT_OUTPUT__ */

	return rhs_sout;
}