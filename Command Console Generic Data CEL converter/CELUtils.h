#ifndef __GENERIC_CEL_FILE_UTILITIES__
#define __GENERIC_CEL_FILE_UTILITIES__

#include <iostream>

#include "DataGroup.h"

/*
*/
int extractIntFromFile(std::istream & sourceFile);

/*
*/
unsigned int extractUintFromFile(std::istream & sourceFile);

/*
*/
bool extractStringFromFile(std::istream & sourceFile, std::string & result);

/*
*/
bool extractWstringFromFile(std::istream & sourceFile, std::wstring & result);

/*
*/
bool extractFileHeader(std::istream & sourceFile, int & groupCount, unsigned int & startPos);

/*
*/
bool extractOneHeaderTriple (std::istream & sourceFile, DataHeaderParameter & oneDHParameter);

/*
*/
bool extractNameTypeValueTrips(std::istream & sourceFile, std::vector<DataHeaderParameter> & vectorToStoreParameters);

/*
*/
bool extractGenericDataHeader(std::istream & sourceFile, std::vector<DataHeaderParameter> & vectorToStoreParameters);

/*
*/
bool extractOneColumnMeta(std::istream & sourceFile, ColumnMetadata & oneColumn);

/*
*/
bool extractDataSetMeta(std::istream & sourceFile, std::vector<ColumnMetadata> & vectorToStoreColumnMeta);

/*
*/
unsigned int extractDataSet(std::istream & sourceFile, DataSet & oneDataSet);

/*
*/
template<typename T>
T extractIntegralBytesFromFile(std::istream & sourceFile, int n_bytes) {
	unsigned char *dummy = new unsigned char[n_bytes];
	T retval = 0;
	int byteIndex = 0;

	sourceFile.read((char*)dummy, n_bytes);
	for (int bits = (n_bytes - 1) * 8; bits > -1; bits = bits - 8) {
		retval += ((T)dummy[byteIndex] << bits);
		byteIndex++;
	}

	delete [] dummy;

	return retval;
}

#endif /* __GENERIC_CEL_FILE_UTILITIES__ */
