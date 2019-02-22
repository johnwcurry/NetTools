/*
 * LogMessageEnt.cpp
 *
 *  Created on: Sep 10, 2016
 *      Author: johnwcurry
 */

#include "LogMessageEnt.h"

const double LogMessageEnt::INTERVAL_TIME = 5.0;

LogMessageEnt::LogMessageEnt()
{
	m_eventCount = 0;
	m_maxMsgsPerInterval = 2;
	m__intervalMsgCount = 0;
	m_totalMsgChecks = 0;
	m_nraTimer.SetDelay(INTERVAL_TIME);
}

LogMessageEnt::~LogMessageEnt()
{
	// TODO Auto-generated destructor stub
}
bool LogMessageEnt::OkToPrint()
{
	m_totalMsgChecks++;
	if (m_nraTimer.Expired())
	{
		m__intervalMsgCount = 0;
	}
	return ++m__intervalMsgCount <= m_maxMsgsPerInterval;
}



