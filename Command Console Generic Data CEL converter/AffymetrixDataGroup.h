#ifndef __GENERIC_CEL_FILE_DATA_GROUP__
#define __GENERIC_CEL_FILE_DATA_GROUP__

#include <string>
#include <vector>

#include "AffymetrixDataSet.h"

class DataGroup {
public:
//private:
	unsigned int nextDataGroupPos;
	unsigned int dataSetStartPos;
	int numberOfDataSets;
	std::wstring dataGroupName;
	std::vector<DataSet> dataGroupDataSets;
};

#endif /* __GENERIC_CEL_FILE_DATA_GROUP__ */
