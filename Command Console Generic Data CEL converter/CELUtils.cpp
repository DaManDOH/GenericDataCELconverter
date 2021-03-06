#include "CELUtils.h"

int extractInt(std::istream & sourceFile) {
	return extractIntegralBytes<int>(sourceFile, 4);
}

unsigned int extractUint(std::istream & sourceFile) {
	return extractIntegralBytes<unsigned int>(sourceFile, 4);
}

bool extractString(std::istream & sourceFile, std::string & result) {
	int stringLengthPrefix = extractInt(sourceFile);
	char * buff = new char[stringLengthPrefix];

	sourceFile.read(buff, stringLengthPrefix);
	result.clear();
	result.reserve(stringLengthPrefix+1);
	result.assign(buff, stringLengthPrefix);
	delete [] buff;

	return true;
}

bool extractWstring(std::istream & sourceFile, std::wstring & result) {
	int stringLengthPrefix = extractInt(sourceFile);
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

bool extractFileHeader(std::istream & sourceFile, int & groupCount, unsigned int & startPos) {
	int magicNum = extractIntegralBytes<int>(sourceFile, 1);
	if ( magicNum != 59 ) {
		return false;
	}

	int versionNum = extractIntegralBytes<int>(sourceFile, 1);
	if ( versionNum != 1 ) {
		return false;
	}

	groupCount = extractInt(sourceFile);
	startPos = extractUint(sourceFile);

	return true;
}

bool extractOneHeaderTriple (std::istream & sourceFile, DataHeaderParameter & oneDHParameter) {
	std::string dummyString;
	std::wstring dummyWstring;

	extractWstring(sourceFile, dummyWstring);
	oneDHParameter.setParamName(dummyWstring);
	extractString(sourceFile, dummyString);
	oneDHParameter.setParamValue(dummyString);
	extractWstring(sourceFile, dummyWstring);
	oneDHParameter.setParamType(dummyWstring);

	return true;
}

bool extractNameTypeValueTrips(std::istream & sourceFile, std::vector<DataHeaderParameter> & vectorToStoreParameters) {
	int paramCount = extractInt(sourceFile);

	vectorToStoreParameters.reserve(paramCount);
	for (int i = 0; i < paramCount; i++) {
		DataHeaderParameter newDHP;
		extractOneHeaderTriple(sourceFile, newDHP);
		vectorToStoreParameters.push_back(newDHP);
	}

	return true;
}

bool extractGenericDataHeader(std::istream & sourceFile, std::vector<DataHeaderParameter> & vectorToStoreParameters) {
	std::string dataTypeID;
	std::string fileGUID;
	std::wstring timeOfCreation;
	std::wstring fileLocale;
	int parentHeaderCount;

	extractString(sourceFile, dataTypeID);
	extractString(sourceFile, fileGUID);
	extractWstring(sourceFile, timeOfCreation);
	extractWstring(sourceFile, fileLocale);
	extractNameTypeValueTrips(sourceFile, vectorToStoreParameters);

	// Extract and ignore parent Generic Data Headers
	bool parentRecordsExtracted = true;
	std::vector<DataHeaderParameter> parentParamVector;
	parentHeaderCount = extractInt(sourceFile);
	parentParamVector.reserve(parentHeaderCount);
	for (int i = 0; i < parentHeaderCount; i++) {
		parentRecordsExtracted &= extractGenericDataHeader(sourceFile, parentParamVector);
	}

	return parentRecordsExtracted;
}

bool extractOneColumnMeta(std::istream & sourceFile, ColumnMetadata & oneColumn) {
	std::wstring dummyWstring;
	extractWstring(sourceFile, dummyWstring);
	oneColumn.setColumnName(dummyWstring);
	unsigned char oneChar = extractIntegralBytes<unsigned char>(sourceFile, 1);
	//sourceFile.read((char*)&oneChar, 1);
	oneColumn.setColumnType((ColumnTypeEnum)oneChar);
	oneColumn.setColumnTypeSize(extractInt(sourceFile));
	return true;
}

bool extractDataSetMeta(std::istream & sourceFile, std::vector<ColumnMetadata> & vectorToStoreColumnMeta) {
	unsigned int dataSetColumnCount = extractUint(sourceFile);
	vectorToStoreColumnMeta.reserve(dataSetColumnCount);
	for (unsigned int i = 0; i < dataSetColumnCount; i++) {
		ColumnMetadata newColumnMeta;
		extractOneColumnMeta(sourceFile, newColumnMeta);
		vectorToStoreColumnMeta.push_back(newColumnMeta);
	}
	return true;
}

unsigned int extractDataSet(std::istream & sourceFile, DataSet & oneDataSet) {
	extractDataSetMeta(sourceFile, oneDataSet.setColumnsMetadata());

	// Calculate the width of the rows' data.
	int sumOfColumnSizes = 0; // Represents the total bytewidth of the fixed-width rows.
	std::vector<ColumnMetadata>::const_iterator oneCM;
	oneCM = oneDataSet.getColumnsMetadata().begin();
	std::vector<ColumnMetadata>::const_iterator metaDataEnd;
	metaDataEnd = oneDataSet.getColumnsMetadata().end();
	std::cout << "\t\tColumns and widths: \n";
	for (; oneCM < metaDataEnd; oneCM++) {
		int columnSize = oneCM->getColumnSize();
		sumOfColumnSizes += columnSize;
		std::cout << "\t\t\t";
		std::wcout << oneCM->getColumnName();
		std::cout << " (";
		std::cout << columnSize;
		std::cout << ")\n";
	}

	unsigned int thisSetRowCount = extractUint(sourceFile);
	std::cout << "\t\tNumber of rows: " << thisSetRowCount << "\n";
	oneDataSet.setRowCount(thisSetRowCount);

	// Extract the data set's rows. The DataSet class' buffer is
	// one-dimensional, so the reserve size should be the row
	// count by the sum of the column widths.
	unsigned long long byteCount = thisSetRowCount*sumOfColumnSizes;
	oneDataSet.setFlattenedDataElementsReserve(byteCount);

	// TODO: Implement a more OOP-based allocation.
	unsigned char * buff;
	if (byteCount > 0) {
		buff = new unsigned char[byteCount];
		sourceFile.read((char*)buff, byteCount);
		oneDataSet.setFlattenedDataElements(buff, &buff[byteCount]);
		delete [] buff;
	}

	// Done: Figure out why the bytewidth is SOMETIMES out-of-sync.
	//
	//       By the newest Command Console Generic Data File Format documentation:
	//
	//       "When [the current data set] is the last data set in the data
	//        group the value shall be 1 byte past the end of the data set. This
	//        way the size of the data set may be determined."
	//
	//       Therefore, the null byte is part of the spec.
	unsigned int currentPos = (unsigned int)sourceFile.tellg();
	unsigned int nextDataSetPos = oneDataSet.getNextDataSetPos();
	if (currentPos != nextDataSetPos) {
		if (nextDataSetPos - currentPos > 1) {
			// TODO: define and throw an exception.
		}
		sourceFile.seekg(nextDataSetPos);
	}

	return thisSetRowCount;
}
