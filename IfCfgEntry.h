/*
 * IfCfgEntry.h
 *
 *  Created on: Oct 10, 2016
 *      Author: johnwcurry
 */

#pragma once
#include <stdio.h>
#include <sys/types.h>
#include "LogMessageMgr.h"

class IfCfgEntry
{
public:
	IfCfgEntry();
	virtual ~IfCfgEntry();
	void ReadDefaults(std::string cfgFileName);

private:
	static const std::string IFCFG_LOC;
	LogMessageMgr *m_pMsg;
	std::string m_fileName;

};


