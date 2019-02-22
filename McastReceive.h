/*
 * McastReceive.h
 *
 *  Created on: Sep 12, 2016
 *      Author: johnwcurry
 */

#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <deque>
#include <mutex>
#include "NetCommon.h"
#include "NraTimer.h"
#include "EncryptorEnt.h"
#include "StatusValues.h"

class McastReceive: public NetCommon
{
public:

	typedef enum
	{
		es_Unknown, es_SelectFail,	//the select statement failed
		es_Exception,	//an exception occurred
		es_NoEvent,		//No event transpired
		es_Event,		//a legit event occurred in the select statement
		es_UnknownEvent	//an event occurred, but I can't identify it
	} McRxEvtStatus;

	McastReceive(std::string localIP);
	virtual ~McastReceive();
	bool GetStackedData(EncryptorEnt  *&pEntry);
	bool Connected()
	{
		return m_connected;
	}
public:
	time_t m_LastActivity;

private:
	bool Initialize();
	EncryptorEnt *DataRx();
	void StackRx(EncryptorEnt *pRx);
	void FlushRxQueue();
	bool SetRxBufferLength();
	McRxEvtStatus CheckForReadEvent(void);
	bool CheckForReceive();
	static void MainThread(McastReceive *pMrx);

private:
	static const size_t MAX_QUEUE_LENGTH;
	std::string m_localIP;
	int m_socket;
	bool m_connected;
	bool m_doAbort;
	bool m_aborted;
	fd_set m_fdsetRead; /* results of the select() */
	fd_set m_fdsetAll; /* holds all the handles */
	fd_set m_fdsetWrite; /* results of the connect() */
	std::deque<EncryptorEnt *> m_dataQue;
	std::recursive_mutex m_Mutex;
};

