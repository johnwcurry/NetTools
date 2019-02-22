/*
 * LogMessageMgr.cpp
 *
 *  Created on: Sep 7, 2016
 *      Author: johnwcurry
 */

#include "LogMessageMgr.h"

#include <unistd.h>
#include <stdarg.h>

LogMessageMgr *LogMessageMgr::pLogMessageMgr = nullptr;
int LogMessageMgr::m_iMsgLevel = 1;
const int LogMessageMgr::PRT_ALWAYS = -102;
const int LogMessageMgr::ERR_ALWAYS = 0;

void LogMessageMgr::InitializeMessageMgr(std::string filePath)
{
	TerminateMessageMgr();
	pLogMessageMgr = new LogMessageMgr(filePath);
}
void LogMessageMgr::TerminateMessageMgr()
{
	if (pLogMessageMgr != nullptr)
	{
		delete pLogMessageMgr;
		pLogMessageMgr = nullptr;
	}
}
//
//Only a single instance of this should be called
//All programs then need to reference LogMessageMgr::pLogMessageMgr
//
LogMessageMgr::LogMessageMgr(std::string filePath)
{
	if (pLogMessageMgr == nullptr)
	{
		_doAbort = false;
		_aborted = false;
		pLogFileMgr = new LogFileMgr(filePath);
		StartThread(filePath);
	}
	else
	{
		pLogMessageMgr->xsprintf(PRT_ALWAYS,"LogMessageMgr::LogMessageMgr has been called!");
	}

}

LogMessageMgr::~LogMessageMgr()
{
	_doAbort = true;
	while (!_aborted)
	{
		usleep(100 * 1000);
	}
	delete pLogFileMgr;
}

void LogMessageMgr::StartThread(std::string filePath)
{
	_doAbort = false;
	_aborted = false;

	std::thread(LogMessageMgr::MainThread, this).detach();

}
std::string LogMessageMgr::TimeAsString(int iCount)
{
	char szCount[100];
	snprintf(szCount,sizeof(szCount),"(%d) ",iCount);
	return TimeAsString() + std::string(szCount);

}
std::string LogMessageMgr::TimeAsString()
{
	auto now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);
	struct tm *parts = std::localtime(&now_c);
	char szTime[1000];
	snprintf(szTime, sizeof(szTime), "%02d-%02d-%02d %02d:%02d:%02d ",
			parts->tm_mon + 1, parts->tm_mday, parts->tm_year - 100,
			parts->tm_hour, parts->tm_min, parts->tm_sec);
	return std::string(szTime);
}
void LogMessageMgr::xsprintf(const char uuid[], int Level, const char *format, ...)
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	try
	{
		bool doPrint = false;
		std::string msgId(uuid);
		LogMessageEnt *pEntry;
		auto search = m_msgMap.find(msgId);

		if (search == m_msgMap.end())
		{
			pEntry = new LogMessageEnt();
			m_msgMap.insert({msgId,pEntry});
		}
		else
			pEntry = search->second;

		if (!pEntry->OkToPrint())
			return;

		if (Level > 0)
		{
			doPrint = Level <= m_iMsgLevel;
		}
		else
		{
			doPrint = abs(Level) <= m_iMsgLevel || Level == PRT_ALWAYS;
		}
		if (doPrint)		//if this level  allowed
		{
			va_list ap;
			char szMsg[10000];
			szMsg[0] = 0;

			va_start(ap, format);
			vsprintf(szMsg, (char *) format, ap);
			va_end(ap);
			if (Level < 0)
			{
				std::string msg = TimeAsString(pEntry->m_eventCount) + szMsg;
				m_WorkList.push_back(msg);
			}
			else
			{
				std::string msg = TimeAsString(pEntry->m_eventCount) + "ERROR " + szMsg;
				m_WorkList.push_back(msg);
			}
		}
	} catch (...)
	{
	}
}
void LogMessageMgr::xsprintf(int Level, const char *format, ...)
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	try
	{
		bool doPrint = false;
		if (Level > 0)
		{
			doPrint = Level <= m_iMsgLevel;
		}
		else
		{
			doPrint = abs(Level) <= m_iMsgLevel || Level == PRT_ALWAYS;
		}
		if (doPrint)		//if this level  allowed
		{
			va_list ap;
			char szMsg[10000];
			szMsg[0] = 0;

			va_start(ap, format);
			vsprintf(szMsg, (char *) format, ap);
			va_end(ap);
			if (Level < 0)
			{
				std::string msg = TimeAsString() + szMsg;
				m_WorkList.push_back(msg);
			}
			else
			{
				std::string msg = TimeAsString() + "ERROR " + szMsg;
				m_WorkList.push_back(msg);
			}
		}
	} catch (...)
	{
	}

}
void LogMessageMgr::MainThread(LogMessageMgr *pMgr)
{
	unsigned int sleepTime = 100 * 1000;
	try
	{
		while (!pMgr->_doAbort)
		{
			usleep(sleepTime);
			pMgr->ProcessMessages();

		}
		pMgr->xsprintf(PRT_ALWAYS,"LogMessageMgr::MainThread is exiting");
		pMgr->ProcessMessages();
	} catch (...)
	{

	}
	pMgr->_aborted = true;
}
void LogMessageMgr::ProcessMessages()
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	try
	{
		FILE *fp = pLogFileMgr->GetFp();
		for (size_t x = 0; x < m_WorkList.size(); x++)
		{
			fprintf(fp, "%s\n", m_WorkList[x].c_str());
		}
		m_WorkList.clear();
		fflush(fp);

	} catch (...)
	{

	}

}

