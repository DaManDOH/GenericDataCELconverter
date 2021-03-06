﻿#include "DataSet.h"

const std::wstring & DataSet::getDataSetName() const {
	return _name;
}

unsigned int DataSet::getRowCount() const {
	return _rows;
}

unsigned int DataSet::getFirstDataElementPos() const {
	return _startPos;
}

unsigned int DataSet::getNextDataSetPos() const {
	return _posNext;
}

const std::vector<DataHeaderParameter> & DataSet::getHeaderParams() const {
	return _params;
}

const std::vector<ColumnMetadata> & DataSet::getColumnsMetadata() const {
	return _columnMeta;
}

const std::vector<unsigned char> & DataSet::getFlattenedDataElements() const {
	return _data;
}

std::wstring & DataSet::setDataSetName() {
	return _name;
}

void DataSet::setRowCount(unsigned int newRowCount) {
	_rows = newRowCount;
}

void DataSet::setFirstDataElementPos(unsigned int newStartPos) {
	_startPos = newStartPos;
}

void DataSet::setNextDataSetPos(unsigned int newNextPos) {
	_posNext = newNextPos;
}

std::vector<DataHeaderParameter> & DataSet::setHeaderParams() {
	return _params;
}

std::vector<ColumnMetadata> & DataSet::setColumnsMetadata() {
	return _columnMeta;
}

void DataSet::setFlattenedDataElementsReserve(unsigned long long maxNumElements) {
	_data.reserve(maxNumElements);
}

void DataSet::setFlattenedDataElements(const unsigned char * start, const unsigned char * end) {
	_data.assign(start, end);
}

void DataSet::setFlattenedDataElements(std::vector<unsigned char> const & sourceVec) {
	_data.assign(sourceVec.begin(), sourceVec.end());
}

int readBigEndianInt(std::vector<unsigned char>::const_iterator & cursor) {
	char retval[4];
	retval[3] = *(cursor++);
	retval[2] = *(cursor++);
	retval[1] = *(cursor++);
	retval[0] = *(cursor++);
	return *((int*)retval);
}

std::string readString(std::vector<unsigned char>::const_iterator & cursor, int charCount) {
	std::string retval;
	retval.reserve(charCount);
	for (int i = charCount - 1; i > -1; i--) {
		retval.push_back((char)*(cursor++));
	}
	return retval;
}

std::wstring readWstring(std::vector<unsigned char>::const_iterator & cursor, int charCount) {
	std::wstring retval;
	retval.reserve(charCount);
	unsigned short oneVal;
	for (int i = charCount - 1; i > -1; i--) {
		oneVal = (unsigned short)(*(cursor) << 8) + (unsigned short)*(cursor+1);
		retval.push_back((wchar_t)oneVal);
		cursor++; cursor++;
	}
	return retval;
}

std::ostream & operator<<(std::ostream & lhs_sout, const DataSet & rhs_obj) {
	std::string dataSetName(rhs_obj._name.cbegin(), rhs_obj._name.cend());
	unsigned int numColumns = (unsigned int)rhs_obj._columnMeta.size();
	unsigned int numRows = rhs_obj.getRowCount();
	lhs_sout << '"' << dataSetName << '(' << numColumns << 'x' << numRows << ')' << '[';

	std::vector<ColumnMetadata>::const_iterator oneColumnMeta;
	std::vector<ColumnMetadata>::const_iterator allMetadataEnd;
	oneColumnMeta = rhs_obj._columnMeta.cbegin();
	allMetadataEnd = rhs_obj._columnMeta.cend();

	std::wstring dummyWideColumnName;
	for (; oneColumnMeta < allMetadataEnd; oneColumnMeta++) {
		dummyWideColumnName = oneColumnMeta->getColumnName();
		std::string columnName(dummyWideColumnName.cbegin(), dummyWideColumnName.cend());
		lhs_sout << '(';
		lhs_sout << columnName << ',';
		lhs_sout << oneColumnMeta->getColumnTypeAsStr() << ',';
		lhs_sout << oneColumnMeta->getColumnSize();
		lhs_sout << ')';
	}

	lhs_sout << "]\"\n";

	if (numRows > 0) {
		int strLen;
		unsigned char oneElementBuff[4];
		std::string oneCELString;
		std::wstring oneCELWstring;
		std::vector<unsigned char>::const_iterator dataCursor, rawDataEnd;
		dataCursor = rhs_obj._data.cbegin();
		rawDataEnd = rhs_obj._data.cend();

		for (unsigned int i = 1; i < numRows+1; i++) {

			oneColumnMeta = rhs_obj._columnMeta.cbegin();
			for (; oneColumnMeta < allMetadataEnd; oneColumnMeta++) {

				switch (oneColumnMeta->getColumnType()) {
				case BYTE_COL:
					oneElementBuff[0] = *(dataCursor++);
					lhs_sout << *((char *)oneElementBuff);
					break;
				case UBYTE_COL:
					oneElementBuff[0] = *(dataCursor++);
					lhs_sout << *((unsigned char *)oneElementBuff);
					break;
				case SHORT_COL:
					oneElementBuff[1] = *(dataCursor++);
					oneElementBuff[0] = *(dataCursor++);
					lhs_sout << *((short *)oneElementBuff);
					break;
				case USHORT_COL:
					oneElementBuff[1] = *(dataCursor++);
					oneElementBuff[0] = *(dataCursor++);
					lhs_sout << *((unsigned short *)oneElementBuff);
					break;
				case INT_COL:
					oneElementBuff[3] = *(dataCursor++);
					oneElementBuff[2] = *(dataCursor++);
					oneElementBuff[1] = *(dataCursor++);
					oneElementBuff[0] = *(dataCursor++);
					lhs_sout << *((int *)oneElementBuff);
					break;
				case UINT_COL:
					oneElementBuff[3] = *(dataCursor++);
					oneElementBuff[2] = *(dataCursor++);
					oneElementBuff[1] = *(dataCursor++);
					oneElementBuff[0] = *(dataCursor++);
					lhs_sout << *((unsigned int *)oneElementBuff);
					break;
				case FLOAT_COL:
					oneElementBuff[3] = *(dataCursor++);
					oneElementBuff[2] = *(dataCursor++);
					oneElementBuff[1] = *(dataCursor++);
					oneElementBuff[0] = *(dataCursor++);
					lhs_sout << *((float*)oneElementBuff);
					break;
				case STRING_COL:
					strLen = readBigEndianInt(dataCursor);
					lhs_sout << '"' << readString(dataCursor, strLen) << '"';
					break;
				case WSTRING_COL:
					strLen = readBigEndianInt(dataCursor);
					oneCELWstring = readWstring(dataCursor, strLen);
					oneCELString.clear();
					oneCELString.assign(oneCELWstring.cbegin(), oneCELWstring.cend());
					lhs_sout << '"' << oneCELString << '"';
					break;
				} // end switch (oneColumnMeta->getColumnType())
				lhs_sout << ',';
			} // end for (; oneColumnMeta < allMetadataEnd; oneColumnMeta++)
			lhs_sout << '\n';
		} // end for upper-bounded by numRows
	}

	return lhs_sout;
}