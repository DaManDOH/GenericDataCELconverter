/*
Program to parse Command Console Generic Data .CEL files

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
	//result.clear();
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
	vectorToStoreColumnMeta.reserve(dataSetColumnCount);
	for (unsigned int i = 0; i < dataSetColumnCount; i++) {
		ColumnMetadata newColumnMeta;
		extractOneColumnMeta(sourceFile, newColumnMeta);
		vectorToStoreColumnMeta.push_back(newColumnMeta);
	}
	return true;
}

unsigned int extractDataSet(ifstream & sourceFile, vector<ColumnMetadata> & vectorToStoreColumnMeta, vector<vector<unsigned char>> & vectorToStoreRows) {
	extractDataSetMeta(sourceFile, vectorToStoreColumnMeta);

	// Calculate the width of the rows' data.
	int sumOfColumnSizes = 0; // Represents the total bytewidth of the fixed-width rows.
	vector<ColumnMetadata>::const_iterator oneCM = vectorToStoreColumnMeta.begin();
	for (; oneCM < vectorToStoreColumnMeta.end(); oneCM++) {
		sumOfColumnSizes += oneCM->getColumnMetaTypeSize();
	}

	
	unsigned int dataSetRowCount = extractUintFromFile(sourceFile);
	cout << "\t\tNumber of rows: " << dataSetRowCount << "\n";
	unsigned int twentieth = dataSetRowCount/20;

	// Extract the data set's rows.
	vectorToStoreRows.reserve(dataSetRowCount);
	for (unsigned int i = 0; i < dataSetRowCount; i++) {
		vector<unsigned char> newRow;
		newRow.reserve(sumOfColumnSizes+1);
		sourceFile.get((char*)newRow.data(), sumOfColumnSizes);
		vectorToStoreRows.push_back(newRow);
		if ((i+1)%twentieth == 0) {
			cout << "\t\t\t" << (int)(i/(double)dataSetRowCount*100) << "%\n";
		}
	}

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

	cout << "Number of data groups: " << numberOfDataGroups << "\n";
	// Extract data groups
	vector<vector<vector<vector<unsigned char>>>> allGroups;
	allGroups.reserve(numberOfDataGroups);
	for (int i = 0; i < numberOfDataGroups; i++) {
		unsigned int nextDataGroupPos;
		unsigned int dataSetStartPos;
		int numberOfDataSets;
		wstring dataGroupName;

		nextDataGroupPos = extractUintFromFile(ccgdCelFile);
		dataSetStartPos = extractUintFromFile(ccgdCelFile);
		numberOfDataSets = extractIntFromFile(ccgdCelFile);
		extractWstringFromFile(ccgdCelFile, dataGroupName);

		cout << "Data group #" << i+1 << ": " << "" << "\n";

		cout << "\tNumber of data sets in group #" << i+1 << ": " << numberOfDataSets << "\n";
		// Extract data sets in the current group
		vector<vector<vector<unsigned char>>> allSets;
		allSets.reserve(numberOfDataSets);
		for (int j = 0; j < numberOfDataSets; j++) {
			wstring dataSetName;

			unsigned int firstDataElementPos = extractUintFromFile(ccgdCelFile);
			unsigned int nextDataSetPos = extractUintFromFile(ccgdCelFile);
			extractWstringFromFile(ccgdCelFile, dataSetName);

			cout << "\tData set #" << j+1 << " in group #" << i+1 << ": " << "" << "\n";

			vector<DataHeaderParameter> dataSetParamVector;
			extractNameTypeValueTrips(ccgdCelFile, dataSetParamVector);

			vector<ColumnMetadata> allColumnsMetadata;
			vector<vector<unsigned char>> rowVector;
			unsigned int dataSetRowCount = extractDataSet(ccgdCelFile, allColumnsMetadata, rowVector);
			allSets.push_back(rowVector);
		}
		allGroups.push_back(allSets);
	}

	ccgdCelFile.close();

	return 0;
}
