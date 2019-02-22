/*
 * LogMessageMgr.h
 *
 *  Created on: Sep 7, 2016
 *      Author: johnwcurry
 */

#pragma once
#include <stdio.h>
#include <sys/types.h>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <map>
#include "LogFileMgr.h"
#include "LogMessageEnt.h"


class LogMessageMgr
{
public:
	static void InitializeMessageMgr(std::string filePath);
	static void TerminateMessageMgr();
	void xsprintf(int Level, const char *format, ...);
	void xsprintf( const char uuid[],int Level, const char *format, ...);
private:
	LogMessageMgr(std::string filePath);
	virtual ~LogMessageMgr();

public:
	void SetMsgLevel(int iLevel)	{m_iMsgLevel = iLevel;};

public:
	static LogMessageMgr *pLogMessageMgr;
private:
	void StartThread(std::string filePath);
	static void MainThread(LogMessageMgr *pMgr);
	void ProcessMessages();
	std::string TimeAsString();
	std::string TimeAsString(int iCount);

public:
	static const int PRT_ALWAYS;
	static const int ERR_ALWAYS;

private:
	std::string _filePath;
	bool _doAbort;
	bool _aborted;
	static int m_iMsgLevel;
	std::vector<std::string> m_WorkList;
	LogFileMgr *pLogFileMgr;
	std::mutex m_Mutex;
	std::map<std::string,LogMessageEnt *> m_msgMap;

};


