/*
 * IfCfgEntry.cpp
 *
 *  Created on: Oct 10, 2016
 *      Author: johnwcurry
 */

#include "IfCfgEntry.h"
#include <iostream>
#include <fstream>
const std::string IfCfgEntry::IFCFG_LOC = "/etc/sysconfig/network-scripts/";
IfCfgEntry::IfCfgEntry()
{
	m_pMsg = LogMessageMgr::pLogMessageMgr;


}

IfCfgEntry::~IfCfgEntry()
{
}
void IfCfgEntry::ReadDefaults(std::string cfgFileName)
{
	static char szWho[] = "IfCfgEntry::ReadDefaults";
	m_fileName = cfgFileName;
	std::string fname = IFCFG_LOC + cfgFileName;
	std::string line;
	std::ifstream myfile (fname);
	if (myfile)
	{
		while (getline( myfile, line ).good())
		{

		}
		myfile.close();

	}
	FILE* fp = fopen(fname.c_str(),"r");
	if (fp == nullptr)
	{
		m_pMsg->xsprintf(0,"%s can't read from file %s",szWho,fname.c_str());
		return;
	}


}

