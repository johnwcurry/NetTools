/*
 * StatusValues.h
 *
 *  Created on: Oct 3, 2016
 *      Author: johnwcurry
 */

#pragma once
#include  <cstddef>
#include <cstdint>
#include <map>
class StatusValues
{
public:
	typedef struct
	{
		int32_t errCount;
		time_t lastEventTime;

	}StatsEntry;
	StatusValues();
	virtual ~StatusValues();
	void SetCategory(std::string category){m_category = category;}
	void UpdateEntry(std::string entryName,int32_t errCount);
	void UpdateEntry(const char *pName,int32_t errCount);

public:
	std::map<std::string,StatsEntry>m_statsEntries;
	std::string m_category;
};


