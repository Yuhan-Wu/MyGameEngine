#pragma once

#include <string>
#include <map>

using namespace std;

class MaskParser
{

public:
	MaskParser() {
		maskMap["Pawn"] = 0x15;
		maskMap["OverlapAll"] = 0x10;
		maskMap["Static"] = 0x11;
		maskMap["UI"] = 0x00;
		maskMap["Player"] = 0x17;
	};

	unsigned int ParseSomeMask(const string& value)
	{
		typename map<string, unsigned int>::const_iterator iValue = maskMap.find(value);

		if (iValue == maskMap.end())
			throw runtime_error("No such enum value.");
		return iValue->second;


	}

private:
	map <string, unsigned int> maskMap;
};