﻿#ifndef __GENERIC_CEL_FILE_DATA_SET__
#define __GENERIC_CEL_FILE_DATA_SET__

#include <fstream>
#include <string>
#include <vector>

#include "ColumnMetadata.h"
#include "DataHeaderParameter.h"

class DataSet {
public:
	const std::wstring & getDataSetName() const;
	unsigned int getDataRowCount() const;
	unsigned int getFirstDataElementPos() const;
	unsigned int getNextDataSetPos() const;
	const std::vector<DataHeaderParameter> & getHeaderParams() const;
	const std::vector<ColumnMetadata> & getColumnsMetadata() const;
	const std::vector<unsigned char> & getFlattenedDataElements() const;
	void getFlattenedDataElementsAsFloat(std::vector<float> &) const;

	std::wstring & setDataSetName();
	void setDataRowCount(unsigned int);
	void setFirstDataElementPos(unsigned int);
	void setNextDataSetPos(unsigned int);
	std::vector<DataHeaderParameter> & setHeaderParams();
	std::vector<ColumnMetadata> & setColumnsMetadata();
	std::vector<unsigned char> & setFlattenedDataElements();

private:
	std::wstring name;
	unsigned int startPos;
	unsigned int posNext;
	std::vector<DataHeaderParameter> params;
	std::vector<ColumnMetadata> columnMeta;
	std::vector<unsigned char> data;
};

std::ostream & operator<<(std::ostream &, const DataSet &);

#endif /* __GENERIC_CEL_FILE_DATA_SET__ */

