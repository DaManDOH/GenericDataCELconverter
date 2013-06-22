/*
Program to parse Command Console Generic Data .CELL files

Dan C. Wlodarski

Info: http://www.affymetrix.com/support/developer/powertools/changelog/gcos-agcc/generic.html
*/

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "ColumnMetadata.h"
#include "DataHeaderParameter.h"

using namespace std;

int extractIntFromFile(ifstream & sourceFile) {
	unsigned char dummy1, dummy2;
	unsigned char dummy3, dummy4;
	int retval;

	dummy1 = sourceFile.get();
	dummy2 = sourceFile.get();
	dummy3 = sourceFile.get();
	dummy4 = sourceFile.get();
	retval = ((int)dummy1 << 24) + ((int)dummy2 << 16) + ((int)dummy3 << 8) + (int)dummy4;

	return retval;
}

unsigned int extractUintFromFile(ifstream & sourceFile) {
	unsigned char dummy1, dummy2;
	unsigned char dummy3, dummy4;
	unsigned int retval;

	dummy1 = sourceFile.get();
	dummy2 = sourceFile.get();
	dummy3 = sourceFile.get();
	dummy4 = sourceFile.get();
	retval = ((unsigned int)dummy1 << 24) + ((unsigned int)dummy2 << 16) + ((unsigned int)dummy3 << 8) + (unsigned int)dummy4;

	return retval;
}

bool extractStringFromFile(ifstream & sourceFile, string & result) {
	int aStringLen = extractIntFromFile(sourceFile);
	char oneChar;

	result.resize(aStringLen);
	result.clear();
	for (int i = 0; i < aStringLen; i++) {
		oneChar = sourceFile.get();
		result.append(1, oneChar);
	}

	return true;
}

bool extractWstringFromFile(ifstream & sourceFile, wstring & result) {
	int aStringLen = extractIntFromFile(sourceFile);
	char oneChar;
	wchar_t oneWchar;

	result.resize(aStringLen);
	result.clear();
	for (int i = 0; i < aStringLen; i++) {
		oneChar = sourceFile.get();
		oneWchar = (wchar_t)oneChar << 8;
		oneChar = sourceFile.get();
		oneWchar += (wchar_t)oneChar;
		result.append(1, oneWchar);
	}

	return true;
}

bool extractFileHeader(ifstream & sourceFile, int & groups, unsigned int & startPos) {
	unsigned char magicNum, versionNum;

	magicNum = sourceFile.get();
	if ( (int)magicNum != 59 ) {
		return false;
	}

	versionNum = sourceFile.get();
	if ((int)versionNum != 1) {
		return false;
	}

	groups = extractIntFromFile(sourceFile);
	startPos = extractUintFromFile(sourceFile);

	return true;
}

bool extractOneTriple (ifstream & sourceFile, DataHeaderParameter & oneParameter) {
	string dummyString;
	wstring dummyWstring;

	extractWstringFromFile(sourceFile, dummyWstring);
	oneParameter.setParamName(dummyWstring);
	extractStringFromFile(sourceFile, dummyString);
	oneParameter.setParamValue(dummyString);
	extractWstringFromFile(sourceFile, dummyWstring);
	oneParameter.setParamType(dummyWstring);

	return true;
}

bool extractNameTypeValueTrips(ifstream & sourceFile, vector<DataHeaderParameter> & vectorToStoreParameters) {
	int paramCount = extractIntFromFile(sourceFile);

	vectorToStoreParameters.reserve(paramCount);
	for (int i = 0; i < paramCount; i++) {
		DataHeaderParameter newDHP;
		extractOneTriple(sourceFile, newDHP);
		vectorToStoreParameters.push_back(newDHP);
	}

	return true;
}

bool extractGenericDataHeader(ifstream & sourceFile, vector<DataHeaderParameter> & vectorToStoreParameters) {
	string dataTypeID;
	string fileGUID;
	wstring timeOfCreation;
	wstring fileLocale;
	int parentHeaderCount;

	extractStringFromFile(sourceFile, dataTypeID);
	extractStringFromFile(sourceFile, fileGUID);
	extractWstringFromFile(sourceFile, timeOfCreation);
	extractWstringFromFile(sourceFile, fileLocale);
	extractNameTypeValueTrips(sourceFile, vectorToStoreParameters);

	// Extract and ignore parent Generic Data Headers
	bool recursiveCheck = true;
	vector<DataHeaderParameter> parentParamVector;
	parentHeaderCount = extractIntFromFile(sourceFile);
	for (int i = 0; i < parentHeaderCount; i++) {
		recursiveCheck &= extractGenericDataHeader(sourceFile, parentParamVector);
	}

	return recursiveCheck;
}

bool extractOneColumnMeta(ifstream & sourceFile, ColumnMetadata & oneColumn) {
	wstring dummyWstring;

	extractWstringFromFile(sourceFile, dummyWstring);
	oneColumn.setColumnMetaName(dummyWstring);
	oneColumn.setColumnMetaType((ColumnMetatypeEnumType)sourceFile.get());
	oneColumn.setColumnMetaTypeSize(extractIntFromFile(sourceFile));

	return true;
}

bool extractDataSetMeta(ifstream & sourceFile, vector<ColumnMetadata> & vectorToStoreColumnMeta) {
	unsigned int dataSetColumnCount = extractUintFromFile(sourceFile);
	for (unsigned int i = 0; i < dataSetColumnCount; i++) {
		ColumnMetadata newColumnMeta;
		extractOneColumnMeta(sourceFile, newColumnMeta);
		vectorToStoreColumnMeta.push_back(newColumnMeta);
	}
	return true;
}

//TODO: Better storage scheme. Strings are detecting their size as zero on all \0s.
bool extractOneRow(ifstream & sourceFile, string & oneRow, int byteCount) {
	char * rawBuff = new char[byteCount];
	sourceFile.get(rawBuff, byteCount);

	oneRow.resize(byteCount);
	oneRow = rawBuff;

	delete rawBuff;
	return true;
}

unsigned int extractDataRows(ifstream & sourceFile, const vector<ColumnMetadata> & vectorToStoreColumnMeta, vector<string> & vectorToStoreRows) {
	int sumOfColumnSizes = 0;
	vector<ColumnMetadata>::const_iterator oneCM = vectorToStoreColumnMeta.begin();
	for (; oneCM < vectorToStoreColumnMeta.end(); oneCM++) {
		sumOfColumnSizes += oneCM->getColumnMetaTypeSize();
	}

	unsigned int dataSetRowCount = extractUintFromFile(sourceFile);
	for (unsigned int i = 0; i < dataSetRowCount; i++) {
		string newRow;
		extractOneRow(sourceFile, newRow, sumOfColumnSizes);
		vectorToStoreRows.push_back(newRow);
	}
	return dataSetRowCount;
}

unsigned int extractData(ifstream & sourceFile, vector<ColumnMetadata> & vectorToStoreColumnMeta, vector<string> & vectorToStoreRows) {
	extractDataSetMeta(sourceFile, vectorToStoreColumnMeta);
	unsigned int dataSetRowCount = extractDataRows(sourceFile, vectorToStoreColumnMeta, vectorToStoreRows);
	return dataSetRowCount;
}

int main( int argc, char * argv[] ) {

	if (argc < 2) {
		cerr << "Need an intensity file.\nAny key to exit." << endl;
		cin.get();
		return -1;
	}

	int numberOfDataGroups;
	unsigned int dataGroupStartPos;

	string filename = argv[1]; // A little buffer overflow protection.
	ifstream ccgdCelFile(filename.c_str(), ios::binary);

	if (!extractFileHeader(ccgdCelFile, numberOfDataGroups, dataGroupStartPos)) {
		return -2;
	}

	vector<DataHeaderParameter> headerParamVector;
	if (!extractGenericDataHeader(ccgdCelFile, headerParamVector)) {
		return -3;
	}

	// Extract data groups
	for (int i = 0; i < numberOfDataGroups; i++) {
		unsigned int nextDataGroupPos;
		unsigned int dataSetStartPos;
		int numberOfDataSets;
		wstring dataGroupName;

		nextDataGroupPos = extractUintFromFile(ccgdCelFile);
		dataSetStartPos = extractUintFromFile(ccgdCelFile);
		numberOfDataSets = extractIntFromFile(ccgdCelFile);
		extractWstringFromFile(ccgdCelFile, dataGroupName);

		// Extract data sets in the current group
		for (int j = 0; j < numberOfDataSets; j++) {
			wstring dataSetName;

			unsigned int firstDataElementPos = extractUintFromFile(ccgdCelFile);
			unsigned int nextDataSetPos = extractUintFromFile(ccgdCelFile);
			extractWstringFromFile(ccgdCelFile, dataSetName);

			vector<DataHeaderParameter> dataSetParamVector;
			extractNameTypeValueTrips(ccgdCelFile, dataSetParamVector);

			vector<ColumnMetadata> vectorToStoreColumnMeta;
			vector<string> vectorToStoreRows;
			unsigned int dataSetRowCount = extractData(ccgdCelFile, vectorToStoreColumnMeta, vectorToStoreRows);
		}
	}

	ccgdCelFile.close();

	return 0;
}
