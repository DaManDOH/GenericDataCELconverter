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
	retval = ((int)dummy1 << 12) + ((int)dummy2 << 8) + ((int)dummy3 << 4) + (int)dummy4;

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
	retval = ((unsigned int)dummy1 << 12) + ((unsigned int)dummy2 << 8) + ((unsigned int)dummy3 << 4) + (unsigned int)dummy4;

	return retval;
}

bool extractStringFromFile(ifstream & sourceFile, string & result) {
	int aStringLen = extractIntFromFile(sourceFile);
	char oneChar;

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

// Here, the integer preceding the string represents the
// number of bytes, not the wide character count.
bool extractValueFromFile(ifstream & sourceFile, wstring & result) {
	int aStringLen = extractIntFromFile(sourceFile);
	char oneChar;
	wchar_t oneWchar;

	result.clear();
	for (int i = 0; i < aStringLen/2; i++) {
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

bool extractGenericDataHeader(ifstream & sourceFile, vector<DataHeaderParameter> & vectorToStoreParameters) {
	int paramCount;
	string dataTypeID;
	string fileGUID;
	wstring timeOfCreation;
	wstring fileLocale;
	wstring dummyWstring;

	extractStringFromFile(sourceFile, dataTypeID);
	extractStringFromFile(sourceFile, fileGUID);
	extractWstringFromFile(sourceFile, timeOfCreation);
	extractWstringFromFile(sourceFile, fileLocale);

	paramCount = extractIntFromFile(sourceFile);
	vectorToStoreParameters.reserve(paramCount);
	for (int i = 0; i < paramCount; i++) {
		DataHeaderParameter newDHP;
		extractWstringFromFile(sourceFile, dummyWstring);
		newDHP.setParamName(dummyWstring);
		extractValueFromFile(sourceFile, dummyWstring);
		newDHP.setParamValue(dummyWstring);
		extractWstringFromFile(sourceFile, dummyWstring);
		newDHP.setParamType(dummyWstring);
		vectorToStoreParameters.push_back(newDHP);
	}
	
	return true;
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

	vector<DataHeaderParameter> paramVector;

	if (!extractGenericDataHeader(ccgdCelFile, paramVector)) {
		return -3;
	}
	//cin.get();

	ccgdCelFile.close();

	return 0;
}
