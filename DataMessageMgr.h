/*
 * DataMessageMgr.h
 *
 *  Created on: Sep 13, 2016
 *      Author: johnwcurry
 */

#pragma once
#include <deque>
#include <mutex>
#include "LogMessageMgr.h"
class DataMessageMgr
{
public:
	typedef struct
		{
			size_t msgLength;
			unsigned char *pMsg;
		}DataEntry;

	DataMessageMgr();
	virtual ~DataMessageMgr();
	void IncomingMsg(unsigned char *pData,int dataLength, const char *pFromWhere);

private:
	std::deque<DataEntry> m_dataQue;
	std::mutex m_Mutex;
	LogMessageMgr *m_pMsg;

};

