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

	extractStringFromFile(sourceFile, dataTypeID);
	extractStringFromFile(sourceFile, fileGUID);
	extractWstringFromFile(sourceFile, timeOfCreation);
	extractWstringFromFile(sourceFile, fileLocale);
	extractNameTypeValueTrips(sourceFile, vectorToStoreParameters);

	return true;
}

int main( int argc, char * argv[] ) {

	if (argc < 2) {
		cerr << "Need an intensity file.\nAny key to exit." << endl;
		cin.get();
		return -1;
	}

	int numberOfDataGroups, parentHeaderCount;
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

	// Debug
	cout << "ccgdCelFile.tellg(): " << ccgdCelFile.tellg() << endl;

	// Extract and ignore parent Generic Data Headers
	vector<DataHeaderParameter> parentParamVector; // Debug
	parentHeaderCount = extractIntFromFile(ccgdCelFile);
	for (int i = 0; i < parentHeaderCount; i++) {
		// Debug: vector<DataHeaderParameter> parentParamVector;
		extractGenericDataHeader(ccgdCelFile, parentParamVector);
	}


	// TODO: Fix sync problem. Temp offset correction.
	unsigned int extraThing01 = extractUintFromFile(ccgdCelFile);
	string extraThing02;
	extractStringFromFile(ccgdCelFile, extraThing02);
	unsigned int extraThing03 = extractUintFromFile(ccgdCelFile);
	unsigned int extraThing04 = extractUintFromFile(ccgdCelFile);
	wstring extraLocale;
	extractWstringFromFile(ccgdCelFile, extraLocale);
	unsigned int extraThing06 = extractUintFromFile(ccgdCelFile);

	// Debug
	cout << "ccgdCelFile.tellg(): 0x" << hex << ccgdCelFile.tellg() << endl;

	wstring extraName01;
	extractWstringFromFile(ccgdCelFile, extraName01);
	string extraValue01;
	extractStringFromFile(ccgdCelFile, extraValue01);
	wstring extraType01;
	extractWstringFromFile(ccgdCelFile, extraType01);
	wstring extraName02;
	extractWstringFromFile(ccgdCelFile, extraName02);
	string extraValue02;
	extractStringFromFile(ccgdCelFile, extraValue02);
	wstring extraType02;
	extractWstringFromFile(ccgdCelFile, extraType02);
	unsigned int extraThing07 = extractUintFromFile(ccgdCelFile);

	// Debug
	cout << "ccgdCelFile.tellg(): 0x" << hex << ccgdCelFile.tellg() << endl;

	//ccgdCelFile.seekg(15807);


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

			// TODO: Extract # of columns (Item 6 in Data Set section)
		}
	}
	//cin.get();

	ccgdCelFile.close();

	return 0;
}
