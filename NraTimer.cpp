/*
 * NraTimer.cpp
 *
 *  Created on: Sep 12, 2016
 *      Author: johnwcurry
 */

#include "NraTimer.h"
NraTimer::NraTimer()
{
	m_evtTime = 1.0;
	m_bForceExpire = false;
	m_lastTime = std::chrono::system_clock::now();

}
NraTimer::NraTimer(double evtTime,bool forceExpire/* = false*/)
{
	m_evtTime = evtTime;
	m_bForceExpire = forceExpire;
	m_lastTime = std::chrono::system_clock::now();

}

NraTimer::~NraTimer()
{
	// TODO Auto-generated destructor stub
}
void NraTimer::SetDelay(double evtTime,bool forceExpire/* = false*/)
{
	m_evtTime = evtTime;
	m_bForceExpire = forceExpire;
	m_lastTime = std::chrono::system_clock::now();


}
bool NraTimer::Expired()
{
	std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - m_lastTime;
	if (elapsed_seconds.count() >= m_evtTime || m_bForceExpire)
	{
		m_bForceExpire = false;
		m_lastTime = std::chrono::system_clock::now();
		return true;
	}
	return false;
}

