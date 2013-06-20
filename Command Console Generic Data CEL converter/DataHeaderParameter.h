#ifndef __GENERIC_CEL_FILE_DATA_HEADER_PARAMETER_TRIPS__
#define __GENERIC_CEL_FILE_DATA_HEADER_PARAMETER_TRIPS__

#include <string>

class DataHeaderParameter {

public:
	const std::wstring & getParamName();
	const std::wstring & getParamValue();
	const std::wstring & getParamType();
	void setParamName(const std::wstring &);
	void setParamValue(const std::wstring &);
	void setParamType(const std::wstring &);

private:
	std::wstring _name;
	std::wstring _value;
	std::wstring _type;

};

#endif /* __GENERIC_CEL_FILE_DATA_HEADER_PARAMETER_TRIPS__ */