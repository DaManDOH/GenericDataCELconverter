#include "DataHeaderParameter.h"

	const std::wstring & DataHeaderParameter::getParamName() {
		return _name;
	}

	const std::string & DataHeaderParameter::getParamValue() {
		return _value;
	}

	const std::wstring & DataHeaderParameter::getParamType() {
		return _type;
	}

	void DataHeaderParameter::setParamName(const std::wstring & newName) {
		_name = newName;
	}

	void DataHeaderParameter::setParamValue(const std::string & newValue) {
		_value = newValue;
	}

	void DataHeaderParameter::setParamType(const std::wstring & newType) {
		_type = newType;
	}