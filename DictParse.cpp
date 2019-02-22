/*
 * DictParse.cpp
 *
 *  Created on: Oct 11, 2016
 *      Author: johnwcurry
 */

#include "DictParse.h"

const char DictParse::COMMENT_CHAR = '#';
DictParse::DictParse()
{
	m_pMsg =  LogMessageMgr::pLogMessageMgr;

}

DictParse::~DictParse()
{
	// TODO Auto-generated destructor stub
}
bool DictParse::AddValue(std::string& strIn)
{
	bool success = false;
	std::string strKey;
	std::string strValue;
	if (ParseLine(strIn,strKey,strValue))
	{
		m_keyMap.insert({strKey,strValue});
		success = true;
	}
	return success;
}
bool DictParse::GetKeyValue(const std::string& strIn,std::string &strValue)
{
	bool success = false;
	auto idx = m_keyMap.find(strIn);
	if (idx != m_keyMap.end())
	{
		strValue = idx->second;
		success = true;
	}
	return success;
}

bool DictParse::trim(std::string& strIn, std::string &strOut )
{
	size_t first = strIn.find_first_not_of(' ');
	if (first == std::string::npos)
		return false;
	size_t last = strIn.find_last_not_of(' ');
	strOut = strIn.substr(first, (last-first+1));
	return true;
}
bool DictParse::ParseLine(std::string& strIn, std::string &strKey,std::string &strValue)
{
	strKey.clear();
	strValue.clear();
	std::string sKey;
	std::string sValue;
	std::string *pStr = &sKey;
	for(auto i = strIn.cbegin(); i != strIn.cend() && *i != COMMENT_CHAR; ++i)
	{
		if (*i < ' ')
			continue;
		switch(*i)
		{
		case '=':
			pStr = &sValue;
			break;

		default:
			*pStr += *i;
			break;
		}
	}
	trim(sKey,strKey);
	trim(sValue,strValue);
	return strKey.length() > 0 && strValue.length() > 0;
}

