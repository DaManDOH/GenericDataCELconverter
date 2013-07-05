#include "ColumnMetadata.h"

const std::wstring & ColumnMetadata::getColumnMetaName() const {
	return _name;
}

ColumnMetatypeEnumType ColumnMetadata::getColumnMetaType() const {
	return _type;
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

