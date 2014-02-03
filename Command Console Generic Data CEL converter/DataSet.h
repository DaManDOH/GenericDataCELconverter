#ifndef __GENERIC_CEL_FILE_DATA_SET__
#define __GENERIC_CEL_FILE_DATA_SET__

#include <fstream>
#include <string>
#include <vector>

#include "ColumnMetadata.h"
#include "DataHeaderParameter.h"

class DataSet {
public:

	/*
	*/
	const std::wstring & getDataSetName() const;

	/*
	*/
	unsigned int getRowCount() const;

	/*
	*/
	unsigned int getFirstDataElementPos() const;

	/*
	*/
	unsigned int getNextDataSetPos() const;

	/*
	*/
	const std::vector<DataHeaderParameter> & getHeaderParams() const;

	/*
	*/
	const std::vector<ColumnMetadata> & getColumnsMetadata() const;

	/*
	*/
	const std::vector<unsigned char> & getFlattenedDataElements() const;

	/*
	*/
	float getNextDataElementAsFloat();

	/*
	*/
	short int getNextDataElementAsShortInt();	

	/*
	*/
	std::wstring & setDataSetName();

	/*
	*/
	void setRowCount(unsigned int);

	/*
	*/
	void setFirstDataElementPos(unsigned int);

	/*
	*/
	void setNextDataSetPos(unsigned int);

	/*
	*/
	std::vector<DataHeaderParameter> & setHeaderParams();

	/*
	*/
	std::vector<ColumnMetadata> & setColumnsMetadata();

	/*
	*/
	void setFlattenedDataElementsReserve(unsigned long long);

	/*
	*/
	void setFlattenedDataElements(const unsigned char *, const unsigned char *);

	/*
	*/
	void setFlattenedDataElements(std::vector<unsigned char> const &);

private:
	std::wstring _name;
	unsigned int _startPos;
	unsigned int _posNext;
	unsigned int _rows;
	std::vector<DataHeaderParameter> _params;
	std::vector<ColumnMetadata> _columnMeta;
	std::vector<unsigned char> _data; // Stores all columns' bytes.

	friend std::ostream & operator<<(std::ostream &, const DataSet &);
};

/*
*/
std::ostream & operator<<(std::ostream &, const DataSet &);

#endif /* __GENERIC_CEL_FILE_DATA_SET__ */

