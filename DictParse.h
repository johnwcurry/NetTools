/*
 * DictParse.h
 *
 *  Created on: Oct 11, 2016
 *      Author: johnwcurry
 */

#pragma once
#include <stdio.h>
#include <sys/types.h>
#include <unordered_map>
#include "LogMessageMgr.h"

class DictParse
{
public:
	DictParse();
	virtual ~DictParse();
	bool trim(std::string& strIn, std::string &strOut );
	bool AddValue(std::string& strIn);
	bool GetKeyValue(const std::string& strIn,std::string &strValue);

private:
	bool ParseLine(std::string& strIn, std::string &strKey,std::string &strValue);

protected:
	LogMessageMgr *m_pMsg;


private:
	static const char COMMENT_CHAR;
	std::unordered_map<std::string, std::string> m_keyMap;

};


