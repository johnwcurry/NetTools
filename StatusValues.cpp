/*
 * StatusValues.cpp
 *
 *  Created on: Oct 3, 2016
 *      Author: johnwcurry
 */

#include "StatusValues.h"

StatusValues::StatusValues()
{


}

StatusValues::~StatusValues()
{

}
void StatusValues::UpdateEntry(std::string entryName,int32_t errCount)
{
	auto currentEntry = m_statsEntries.find(entryName);
	if (currentEntry != m_statsEntries.end())
	{
		currentEntry->second.errCount += errCount;
		currentEntry->second.lastEventTime = time(nullptr);
	}
	else
	{
		StatsEntry nStats{errCount,time(nullptr)};
		m_statsEntries[entryName] = nStats;
	}
}
void StatusValues::UpdateEntry(const char *pName,int32_t errCount)
{
	std::string sName(pName);
	UpdateEntry(sName,errCount);
}

