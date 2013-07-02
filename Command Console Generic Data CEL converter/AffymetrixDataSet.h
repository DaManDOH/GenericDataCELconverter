#ifndef __GENERIC_CEL_FILE_DATA_SET__
#define __GENERIC_CEL_FILE_DATA_SET__

#include <string>
#include <vector>

#include "DataHeaderParameter.h"
#include "ColumnMetadata.h"

class DataSet {
public:
//private:
	std::wstring name;
	unsigned int rowCount;
	unsigned int firstDataElementPos;
	unsigned int nextDataSetPos;
	std::vector<DataHeaderParameter> params;
	std::vector<ColumnMetadata> allColumnsMetadata;
	std::vector<unsigned char> flattenedDataRows;
};

#endif /* __GENERIC_CEL_FILE_DATA_SET__ */
