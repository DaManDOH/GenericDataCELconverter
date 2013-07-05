/*
Program to parse Command Console Generic Data .CEL files

Dan C. Wlodarski

Info: http://www.affymetrix.com/support/developer/powertools/changelog/gcos-agcc/generic.html
*/

#include <fstream>
#include <iostream>
#ifdef __preload__
#include <sstream>
#endif /* __preload__ */
#include <string>
#include <vector>

#include "DataGroup.h"

using namespace std;

int extractIntFromFile(istream & sourceFile) {
	unsigned char dummy[4];
	int retval;

	sourceFile.read((char*)dummy, 4);
	retval = ((int)dummy[0] << 24);
	retval += ((int)dummy[1] << 16);
	retval += ((int)dummy[2] << 8);
	retval += (int)dummy[3];

	return retval;
}

unsigned int extractUintFromFile(istream & sourceFile) {
	unsigned char dummy[4];
	unsigned int retval;

	sourceFile.read((char*)dummy, 4);
	retval = ((unsigned int)dummy[0] << 24);
	retval += ((unsigned int)dummy[1] << 16);
	retval += ((unsigned int)dummy[2] << 8);
	retval += (unsigned int)dummy[3];

	return retval;
}

bool extractStringFromFile(istream & sourceFile, string & result) {
	int aStringLen = extractIntFromFile(sourceFile);
	char * buff = new char[aStringLen];

	sourceFile.read(buff, aStringLen);
	result.clear();
	result.reserve(aStringLen+1);
	result.assign(buff, aStringLen);
	delete [] buff;

	return true;
}

bool extractWstringFromFile(istream & sourceFile, wstring & result) {
	int aStringLen = extractIntFromFile(sourceFile);
	unsigned char * buff = new unsigned char[2*aStringLen];
	char16_t oneWchar;
	
	sourceFile.read((char*)buff, 2*aStringLen);
	result.clear();
	result.reserve(aStringLen+1);

	// .CEL files' "Unicode" wide-strings do not actually conform to
	// the Unicode standard. Each wide-byte is big-endian instead
	// of the required little-endian.
	for (int i = 0; i < aStringLen; i++) {
		oneWchar = buff[i*2] << 8;
		oneWchar += buff[(i+1)*2-1];
		result.append(1, oneWchar);
	}
	delete [] buff;

	return true;
}

bool extractFileHeader(istream & sourceFile, int & groups, unsigned int & startPos) {
	unsigned char magicNum, versionNum;

	sourceFile.read((char*)&magicNum, 1);
	if ( (int)magicNum != 59 ) {
		return false;
	}

	sourceFile.read((char*)&versionNum, 1);
	if ((int)versionNum != 1) {
		return false;
	}

	groups = extractIntFromFile(sourceFile);
	startPos = extractUintFromFile(sourceFile);

	return true;
}

bool extractOneHeaderTriple (istream & sourceFile, DataHeaderParameter & oneParameter) {
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

bool extractNameTypeValueTrips(istream & sourceFile, vector<DataHeaderParameter> & vectorToStoreParameters) {
	int paramCount = extractIntFromFile(sourceFile);

	vectorToStoreParameters.reserve(paramCount);
	for (int i = 0; i < paramCount; i++) {
		DataHeaderParameter newDHP;
		extractOneHeaderTriple(sourceFile, newDHP);
		vectorToStoreParameters.push_back(newDHP);
	}

	return true;
}

bool extractGenericDataHeader(istream & sourceFile, vector<DataHeaderParameter> & vectorToStoreParameters) {
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
	bool parentRecordsExtracted = true;
	vector<DataHeaderParameter> parentParamVector;
	parentHeaderCount = extractIntFromFile(sourceFile);
	parentParamVector.reserve(parentHeaderCount);
	for (int i = 0; i < parentHeaderCount; i++) {
		parentRecordsExtracted &= extractGenericDataHeader(sourceFile, parentParamVector);
	}

	return parentRecordsExtracted;
}

bool extractOneColumnMeta(istream & sourceFile, ColumnMetadata & oneColumn) {
	wstring dummyWstring;
	unsigned char oneChar;

	extractWstringFromFile(sourceFile, dummyWstring);
	oneColumn.setColumnMetaName(dummyWstring);
	sourceFile.read((char*)&oneChar, 1);
	oneColumn.setColumnMetaType((ColumnMetatypeEnumType)oneChar);
	oneColumn.setColumnMetaTypeSize(extractIntFromFile(sourceFile));

	return true;
}

bool extractDataSetMeta(istream & sourceFile, vector<ColumnMetadata> & vectorToStoreColumnMeta) {
	unsigned int dataSetColumnCount = extractUintFromFile(sourceFile);
	vectorToStoreColumnMeta.reserve(dataSetColumnCount);
	for (unsigned int i = 0; i < dataSetColumnCount; i++) {
		ColumnMetadata newColumnMeta;
		extractOneColumnMeta(sourceFile, newColumnMeta);
		vectorToStoreColumnMeta.push_back(newColumnMeta);
	}
	return true;
}

unsigned int extractDataSet(istream & sourceFile, DataSet & oneDataSet) {
	extractDataSetMeta(sourceFile, oneDataSet.setColumnsMetadata());

	// Calculate the width of the rows' data.
	int sumOfColumnSizes = 0; // Represents the total bytewidth of the fixed-width rows.
	vector<ColumnMetadata>::const_iterator oneCM = oneDataSet.getColumnsMetadata().begin();
	vector<ColumnMetadata>::const_iterator metaDataEnd = oneDataSet.getColumnsMetadata().end();
	for (; oneCM < metaDataEnd; oneCM++) {
		sumOfColumnSizes += oneCM->getColumnMetaTypeSize();
	}
	
	unsigned int thisSetRowCount = extractUintFromFile(sourceFile);
	cout << "\t\tNumber of rows: " << thisSetRowCount << "\n";

	// Extract the data set's rows. The DataSet class' buffer is
	// one-dimensional, so the reserve size should be the row
	// count by the sum of the column widths.
	unsigned int byteCount = thisSetRowCount*sumOfColumnSizes;
	oneDataSet.setFlattenedDataRows().reserve(byteCount);
	
	// TODO: Figure out how to use more OOP allocation.
	unsigned char * buff;
	if (byteCount > 0) {
		buff = new unsigned char[byteCount];
		sourceFile.read((char*)buff, byteCount);
		oneDataSet.setFlattenedDataRows().assign(&buff[0], &buff[byteCount-1]);
		delete [] buff;
	}

	// TODO: Figure out why the bytewidth are SOMETIMES out-of-sync.
	unsigned int currentPos = (unsigned int)sourceFile.tellg();
	unsigned int nextDataSetPos = oneDataSet.getNextDataSetPos();
	if (currentPos != nextDataSetPos) {
		cerr << dec << "\t\t!! ";
		cerr << nextDataSetPos - currentPos;
		cerr << "-byte correction!!\n";
		sourceFile.seekg(nextDataSetPos);
	}

	return thisSetRowCount;
}

int main( int argc, char * argv[] ) {
	int retval = 0;

	if (argc < 2) {
		cerr << "Need an intensity file.\n" << endl;
		retval = -1;
	}

	int numberOfDataGroups;
	unsigned int dataGroupStartPos;
	string filename = argv[1]; // A little buffer overflow protection.

#ifdef __preload__
	ifstream ccgdCelFileSource(filename.c_str(), ios::binary);
	istringstream ccgdCelFile;
	ccgdCelFile.set_rdbuf(ccgdCelFileSource.rdbuf());
	ccgdCelFileSource.close();
#else
	ifstream ccgdCelFile(filename.c_str(), ios::binary);
#endif /* preload */

	if (!extractFileHeader(ccgdCelFile, numberOfDataGroups, dataGroupStartPos)) {
		cerr << "Bad file header.\n" << endl;
		retval = -2;
	}

	vector<DataHeaderParameter> headerParamVector;
	if (!extractGenericDataHeader(ccgdCelFile, headerParamVector)) {
		cerr << "Bad data header.\n" << endl;
		retval = -3;
	}

	cout << "Number of data groups: " << numberOfDataGroups << "\n";
	// Extract data groups
	vector<DataGroup> allGroups;
	allGroups.reserve(numberOfDataGroups);
	for (int i = 0; i < numberOfDataGroups; i++) {
		DataGroup oneGroup;

		oneGroup.setNextDataGroupPos(extractUintFromFile(ccgdCelFile));
		oneGroup.setStartPos(extractUintFromFile(ccgdCelFile));
		int numberOfDataSets = extractIntFromFile(ccgdCelFile);
		extractWstringFromFile(ccgdCelFile, oneGroup.setDataGroupName());

		cout << "Data group #" << dec << i+1 << ": ";
		wcout << oneGroup.getDataGroupName();
		cout << "\n";

		cout << "\tNumber of data sets in group #" << i+1 << ": " << numberOfDataSets << "\n";

		// Extract data sets in the current group
		oneGroup.setDataSets().reserve(numberOfDataSets);
		for (int j = 0; j < numberOfDataSets; j++) {
			DataSet oneSet;

			oneSet.setFirstDataElementPos(extractUintFromFile(ccgdCelFile));
			oneSet.setNextDataSetPos(extractUintFromFile(ccgdCelFile));
			extractWstringFromFile(ccgdCelFile, oneSet.setDataSetName());

			cout << "\tData set #" << dec << j+1 << " in group #" << i+1 << ": ";
			wcout << oneSet.getDataSetName();
			cout << "\n";

			extractNameTypeValueTrips(ccgdCelFile, oneSet.setHeaderParams());

			unsigned int dataSetRowCount = extractDataSet(ccgdCelFile, oneSet);
			oneGroup.setDataSets().push_back(oneSet);
		}
		allGroups.push_back(oneGroup);
	}

#ifndef __preload__
	ccgdCelFile.close();
#endif /* undefined __preload__ */

#ifdef _DEBUG
	//cout << "\nAny key to continue." << endl;
	//cin.get();
#endif

	return retval;
}
