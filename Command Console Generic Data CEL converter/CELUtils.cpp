#include "CELUtils.h"

int extractIntFromFile(std::istream & sourceFile) {
	return extractIntegralBytesFromFile<int>(sourceFile, 4);
}

unsigned int extractUintFromFile(std::istream & sourceFile) {
	return extractIntegralBytesFromFile<unsigned int>(sourceFile, 4);
}

bool extractStringFromFile(std::istream & sourceFile, std::string & result) {
	int stringLengthPrefix = extractIntFromFile(sourceFile);
	char * buff = new char[stringLengthPrefix];

	sourceFile.read(buff, stringLengthPrefix);
	result.clear();
	result.reserve(stringLengthPrefix+1);
	result.assign(buff, stringLengthPrefix);
	delete [] buff;

	return true;
}

bool extractWstringFromFile(std::istream & sourceFile, std::wstring & result) {
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

bool extractFileHeader(std::istream & sourceFile, int & groupCount, unsigned int & startPos) {
	int magicNum = extractIntegralBytesFromFile<int>(sourceFile, 1);
	if ( magicNum != 59 ) {
		return false;
	}

	int versionNum = extractIntegralBytesFromFile<int>(sourceFile, 1);
	if ( versionNum != 1 ) {
		return false;
	}

	groupCount = extractIntFromFile(sourceFile);
	startPos = extractUintFromFile(sourceFile);

	return true;
}

bool extractOneHeaderTriple (std::istream & sourceFile, DataHeaderParameter & oneDHParameter) {
	std::string dummyString;
	std::wstring dummyWstring;

	extractWstringFromFile(sourceFile, dummyWstring);
	oneDHParameter.setParamName(dummyWstring);
	extractStringFromFile(sourceFile, dummyString);
	oneDHParameter.setParamValue(dummyString);
	extractWstringFromFile(sourceFile, dummyWstring);
	oneDHParameter.setParamType(dummyWstring);

	return true;
}

bool extractNameTypeValueTrips(std::istream & sourceFile, std::vector<DataHeaderParameter> & vectorToStoreParameters) {
	int paramCount = extractIntFromFile(sourceFile);

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

	extractStringFromFile(sourceFile, dataTypeID);
	extractStringFromFile(sourceFile, fileGUID);
	extractWstringFromFile(sourceFile, timeOfCreation);
	extractWstringFromFile(sourceFile, fileLocale);
	extractNameTypeValueTrips(sourceFile, vectorToStoreParameters);

	// Extract and ignore parent Generic Data Headers
	bool parentRecordsExtracted = true;
	std::vector<DataHeaderParameter> parentParamVector;
	parentHeaderCount = extractIntFromFile(sourceFile);
	parentParamVector.reserve(parentHeaderCount);
	for (int i = 0; i < parentHeaderCount; i++) {
		parentRecordsExtracted &= extractGenericDataHeader(sourceFile, parentParamVector);
	}

	return parentRecordsExtracted;
}

bool extractOneColumnMeta(std::istream & sourceFile, ColumnMetadata & oneColumn) {
	std::wstring dummyWstring;
	extractWstringFromFile(sourceFile, dummyWstring);
	oneColumn.setColumnName(dummyWstring);
	unsigned char oneChar = extractIntegralBytesFromFile<unsigned char>(sourceFile, 1);
	//sourceFile.read((char*)&oneChar, 1);
	oneColumn.setColumnType((ColumnTypeEnum)oneChar);
	oneColumn.setColumnTypeSize(extractIntFromFile(sourceFile));
	return true;
}

bool extractDataSetMeta(std::istream & sourceFile, std::vector<ColumnMetadata> & vectorToStoreColumnMeta) {
	unsigned int dataSetColumnCount = extractUintFromFile(sourceFile);
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

	unsigned int thisSetRowCount = extractUintFromFile(sourceFile);
	std::cout << "\t\tNumber of rows: " << thisSetRowCount << "\n";

	// Extract the data set's rows. The DataSet class' buffer is
	// one-dimensional, so the reserve size should be the row
	// count by the sum of the column widths.
	unsigned long long byteCount = thisSetRowCount*sumOfColumnSizes;
	oneDataSet.setFlattenedDataElementsReserve(byteCount);

	// TODO: Figure out how to implement a more OOP-based allocation.
	unsigned char * buff;
	if (byteCount > 0) {
		buff = new unsigned char[byteCount];
		sourceFile.read((char*)buff, byteCount);
		oneDataSet.setFlattenedDataElements(&buff[0], &buff[byteCount-1]);
		delete [] buff;
	}

	// TODO: Figure out why the bytewidth is SOMETIMES out-of-sync.
	unsigned int currentPos = (unsigned int)sourceFile.tellg();
	unsigned int nextDataSetPos = oneDataSet.getNextDataSetPos();
	if (currentPos != nextDataSetPos) {
		std::cerr << std::dec << "\t\t!! ";
		std::cerr << nextDataSetPos - currentPos;
		std::cerr << "-byte correction!!\n";
		sourceFile.seekg(nextDataSetPos);
	}

	return thisSetRowCount;
}
