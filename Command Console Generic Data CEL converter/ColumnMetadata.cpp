#include "ColumnMetadata.h"

#include <algorithm>
#include <exception>

std::string ColumnMetadata::convMetaTypeEnumToMetaTypeStr(ColumnMetatypeEnumType colType) {
	std::string retval;
	switch (colType) {
	case BYTE_COL:
		retval = "BYTE_COL";
		break;
	case UBYTE_COL:
		retval = "UBYTE_COL";
		break;
	case SHORT_COL:
		retval = "SHORT_COL";
		break;
	case USHORT_COL:
		retval = "USHORT_COL";
		break;
	case INT_COL:
		retval = "INT_COL";
		break;
	case UINT_COL:
		retval = "UINT_COL";
		break;
	case FLOAT_COL:
		retval = "FLOAT_COL";
		break;
	case STRING_COL:
		retval = "STRING_COL";
		break;
	case WSTRING_COL:
		retval = "WSTRING_COL";
		break;
	default:
		break;
	}
	return retval;
}

ColumnMetatypeEnumType ColumnMetadata::convMetaTypeStrToMetaTypeEnum(const std::string & colType) {
	std::string upperedColType = colType;
	std::transform(upperedColType.begin(), upperedColType.end(), upperedColType.begin(), ::toupper);
	if (upperedColType.compare("BYTE_COL") == 0) {
		return BYTE_COL;
	}
	if (upperedColType.compare("UBYTE_COL") == 0) {
		return UBYTE_COL;
	}
	if (upperedColType.compare("SHORT_COL") == 0) {
		return SHORT_COL;
	}
	if (upperedColType.compare("USHORT_COL") == 0) {
		return USHORT_COL;
	}
	if (upperedColType.compare("INT_COL") == 0) {
		return INT_COL;
	}
	if (upperedColType.compare("UINT_COL") == 0) {
		return UINT_COL;
	}
	if (upperedColType.compare("FLOAT_COL") == 0) {
		return FLOAT_COL;
	}
	if (upperedColType.compare("STRING_COL") == 0) {
		return STRING_COL;
	}
	if (upperedColType.compare("WSTRING_COL") == 0) {
		return WSTRING_COL;
	}
	throw std::invalid_argument('\"' + colType + "\" is an invalid column type name.");
}

const std::wstring & ColumnMetadata::getColumnMetaName() const {
	return _name;
}

ColumnMetatypeEnumType ColumnMetadata::getColumnMetaType() const {
	return _type;
}

std::string ColumnMetadata::getColumnMetaTypeAsStr() const {
	return convMetaTypeEnumToMetaTypeStr(this->getColumnMetaType());
}

int ColumnMetadata::getColumnMetaTypeSize() const {
	return _typeSize;
}

void ColumnMetadata::setColumnMetaName(const std::wstring & newName) {
	_name = newName;
}

void ColumnMetadata::setColumnMetaType(ColumnMetatypeEnumType newColType) {
	_type = newColType;
}

void ColumnMetadata::setColumnMetaTypeSize(int newColTypeSize) {
	_typeSize = newColTypeSize;
}

void ColumnMetadata::setColumnMetaTypeFromStr(const std::string & newColTypeStr) {
	_type = convMetaTypeStrToMetaTypeEnum(newColTypeStr);
}
