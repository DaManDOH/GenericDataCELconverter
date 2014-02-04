#ifndef __GENERIC_CEL_FILE_COLUMN_META_DATA__
#define __GENERIC_CEL_FILE_COLUMN_META_DATA__

#include <string>

/*
*/
enum ColumnTypeEnum {
	BYTE_COL,
	UBYTE_COL,
	SHORT_COL,
	USHORT_COL,
	INT_COL,
	UINT_COL,
	FLOAT_COL,
	STRING_COL,
	WSTRING_COL
};

/*
*/
class ColumnMetadata {

public:
	/*
	*/
	const std::wstring & getColumnName() const;

	/*
	*/
	ColumnTypeEnum getColumnType() const;

	/*
	*/
	std::string getColumnTypeAsStr() const;

	/*
	*/
	int getColumnSize() const;

	/*
	*/
	void setColumnName(const std::wstring &);

	/*
	*/
	void setColumnType(ColumnTypeEnum);

	/*
	*/
	void setColumnTypeFromStr(const std::string &);

	/*
	*/
	void setColumnTypeSize(int);

private:
	/*
	*/
	static std::string convTypeEnumToStr(ColumnTypeEnum);

	/*
	*/
	static ColumnTypeEnum convStrToTypeEnum(const std::string &);

	std::wstring _name;
	ColumnTypeEnum _type;
	int _typeSize;
};

#endif /* __GENERIC_CEL_FILE_COLUMN_META_DATA__ */

