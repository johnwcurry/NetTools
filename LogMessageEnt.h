/*
 * LogMessageEnt.h
 *
 *  Created on: Sep 10, 2016
 *      Author: johnwcurry
 */

#pragma once
#include "NraTimer.h"

class LogMessageEnt
{
public:
	LogMessageEnt();
	virtual ~LogMessageEnt();

public:
	int m_eventCount;
	int m_maxMsgsPerInterval;
	int m_totalMsgChecks;
	bool OkToPrint();

private:
	static const double INTERVAL_TIME;
	int m__intervalMsgCount;
	NraTimer m_nraTimer;
};

