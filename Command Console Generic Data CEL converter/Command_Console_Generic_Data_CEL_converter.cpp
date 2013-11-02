/*
Program to parse Command Console Generic Data .CEL files

Dan C. Wlodarski

Info: http://www.affymetrix.com/support/developer/powertools/changelog/gcos-agcc/generic.html
*/

#include <fstream>
#include <iostream>
#include <string>

#include "pointer_vector.h"

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
	int stringLengthPrefix = extractIntFromFile(sourceFile);
	char * buff = new char[stringLengthPrefix];

	sourceFile.read(buff, stringLengthPrefix);
	result.clear();
	result.reserve(stringLengthPrefix+1);
	result.assign(buff, stringLengthPrefix);
	delete [] buff;

	return true;
}

bool extractWstringFromFile(istream & sourceFile, wstring & result) {
	int stringLengthPrefix = extractIntFromFile(sourceFile);
	unsigned char * buff = new unsigned char[2*stringLengthPrefix];
	char16_t oneWchar;

	sourceFile.read((char*)buff, 2*stringLengthPrefix);
	result.clear();
	result.reserve(stringLengthPrefix+1);

	// .CEL files' "Unicode" wide-strings do not actually conform to
	// the Unicode standard. Each wide-byte is big-endian instead
	// of the required little-endian.
	for (int i = 0; i < stringLengthPrefix; i++) {
		oneWchar = buff[i*2] << 8;
		oneWchar += buff[(i+1)*2-1];
		result.append(1, oneWchar);
	}
	delete [] buff;

	return true;
}

bool extractFileHeader(istream & sourceFile, int & groupCount, unsigned int & startPos) {
	unsigned char magicNum, versionNum;

	sourceFile.read((char*)&magicNum, 1);
	if ( (int)magicNum != 59 ) {
		return false;
	}

	sourceFile.read((char*)&versionNum, 1);
	if ((int)versionNum != 1) {
		return false;
	}

	groupCount = extractIntFromFile(sourceFile);
	startPos = extractUintFromFile(sourceFile);

	return true;
}

bool extractOneHeaderTriple (istream & sourceFile, DataHeaderParameter & oneDHParameter) {
	string dummyString;
	wstring dummyWstring;

	extractWstringFromFile(sourceFile, dummyWstring);
	oneDHParameter.setParamName(dummyWstring);
	extractStringFromFile(sourceFile, dummyString);
	oneDHParameter.setParamValue(dummyString);
	extractWstringFromFile(sourceFile, dummyWstring);
	oneDHParameter.setParamType(dummyWstring);

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
	cout << "\t\tColumns and widths: \n";
	for (; oneCM < metaDataEnd; oneCM++) {
		int columnSize = oneCM->getColumnMetaTypeSize();
		sumOfColumnSizes += columnSize;
		cout << "\t\t\t";
		wcout << oneCM->getColumnMetaName();
		cout << " (";
		cout << columnSize;
		cout << ")\n";
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
	} else {

		int numberOfDataGroups;
		unsigned int dataGroupStartPos;
		string filepath = argv[1]; // A little buffer overflow protection.

		ifstream ccgdCelFile(filepath.c_str(), ios::binary);

		if (!extractFileHeader(ccgdCelFile, numberOfDataGroups, dataGroupStartPos)) {
			cerr << "Bad file header.\n" << endl;
			retval = -2;
		} else {

			vector<DataHeaderParameter> headerParamVector;
			if (!extractGenericDataHeader(ccgdCelFile, headerParamVector)) {
				cerr << "Bad data header.\n" << endl;
				retval = -3;
			} else {

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

					cout << "\tNumber of data sets in group #";
					cout << i+1 << ": " << numberOfDataSets << "\n";

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

						extractDataSet(ccgdCelFile, oneSet);
						oneGroup.setDataSets().push_back(oneSet);

						/*
						if (oneSet.getColumnsMetadata().at(0).getColumnMetaType() == FLOAT_COL) {
							vector<float> tempData;
							oneSet.getFlattenedDataRowsAsFloat(tempData);
							ofstream tempout("D:\\tempoutput.txt");
							for (int temp_i = 0; temp_i < tempData.size(); temp_i++) {
								tempout << tempData.at(temp_i) << '\n';
							}
							tempout << flush;
							tempout.close();
						}
						*/
					}
					allGroups.push_back(oneGroup);
				}
			}
		}

		ccgdCelFile.close();

	}

#ifdef _DEBUG
	cout << "\nAny key to continue." << endl;
	cin.get();
#endif

	return retval;
}
