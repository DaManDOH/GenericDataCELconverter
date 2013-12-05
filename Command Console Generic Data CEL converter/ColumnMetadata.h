#ifndef __GENERIC_CEL_FILE_COLUMN_META_DATA__
#define __GENERIC_CEL_FILE_COLUMN_META_DATA__

#include <string>

enum ColumnMetatypeEnumType {
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

class ColumnMetadata {

public:
	const std::wstring & getColumnMetaName() const;
	ColumnMetatypeEnumType getColumnMetaType() const;
	std::string getColumnMetaTypeAsStr() const;
	int getColumnMetaTypeSize() const;
	void setColumnMetaName(const std::wstring &);
	void setColumnMetaType(ColumnMetatypeEnumType);
	void setColumnMetaTypeFromStr(const std::string &);
	void setColumnMetaTypeSize(int);

private:
	static std::string convMetaTypeEnumToMetaTypeStr(ColumnMetatypeEnumType);
	static ColumnMetatypeEnumType convMetaTypeStrToMetaTypeEnum(const std::string &);
	std::wstring _name;
	ColumnMetatypeEnumType _type;
	int _typeSize;

};

#endif /* __GENERIC_CEL_FILE_COLUMN_META_DATA__ */

