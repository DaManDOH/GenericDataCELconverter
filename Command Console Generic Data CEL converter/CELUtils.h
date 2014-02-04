#ifndef __GENERIC_CEL_FILE_UTILITIES__
#define __GENERIC_CEL_FILE_UTILITIES__

#include <iostream>

#include "DataGroup.h"

/*
*/
int extractIntFromStream(std::istream &);

/*
*/
unsigned int extractUintFromFile(std::istream &);

/*
*/
bool extractStringFromFile(std::istream &, std::string &);

/*
*/
bool extractWstringFromFile(std::istream &, std::wstring &);

/*
*/
bool extractFileHeader(std::istream &, int &, unsigned int &);

/*
*/
bool extractOneHeaderTriple (std::istream &, DataHeaderParameter &);

/*
*/
bool extractNameTypeValueTrips(std::istream &, std::vector<DataHeaderParameter> &);

/*
*/
bool extractGenericDataHeader(std::istream &, std::vector<DataHeaderParameter> &);

/*
*/
bool extractOneColumnMeta(std::istream &, ColumnMetadata &);

/*
*/
bool extractDataSetMeta(std::istream &, std::vector<ColumnMetadata> &);

/*
*/
unsigned int extractDataSet(std::istream &, DataSet &);

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
