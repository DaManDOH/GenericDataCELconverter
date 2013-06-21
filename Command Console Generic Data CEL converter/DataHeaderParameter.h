#ifndef __GENERIC_CEL_FILE_DATA_HEADER_PARAMETER_TRIPS__
#define __GENERIC_CEL_FILE_DATA_HEADER_PARAMETER_TRIPS__

#include <string>

class DataHeaderParameter {

public:
	const std::wstring & getParamName();
	const std::string & getParamValue();
	const std::wstring & getParamType();
	void setParamName(const std::wstring &);
	void setParamValue(const std::string &);
	void setParamType(const std::wstring &);

private:
	std::wstring _name;
	std::string _value;
	std::wstring _type;

};

#endif /* __GENERIC_CEL_FILE_DATA_HEADER_PARAMETER_TRIPS__ */