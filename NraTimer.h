/*
 * NraTimer.h
 *
 *  Created on: Sep 12, 2016
 *      Author: johnwcurry
 */

#pragma once
#include <chrono>

class NraTimer
{
public:
	NraTimer();
	NraTimer(double evtTime,bool forceExpire = false);
	void SetDelay(double evtTime,bool forceExpire = false);
	bool Expired();
	virtual ~NraTimer();

private:
	double m_evtTime;
	bool m_bForceExpire;
	std::chrono::system_clock::time_point m_lastTime;
};

