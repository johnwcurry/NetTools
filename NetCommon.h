/*
 * NetCommon.h
 *
 *  Created on: Sep 12, 2016
 *      Author: johnwcurry
 */

#pragma once
#include "LogMessageMgr.h"
#include "StatusValues.h"

class NetCommon
{
public:
	NetCommon();
	virtual ~NetCommon();
protected:
	void SetOptionReuse(int socketNumber, const char* pFromWhere);
	bool SetNonblocking(int fd, const char * pFromWhere);
	bool Reverse(char *p);

public:
	static const int SUCCESS;
	static const int SOCKET_ERROR;
	static const int INVALID_SOCKET;
	static const int MCAST_SEND_PORT;
	static const int MCAST_RX_PORT;
	static const int TCP_RX_PORT;
	static const int TCP_TX_PORT;
	static const std::string MCAST_SEND_ADDR;
	static const std::string MCAST_RX_ADDR;
protected:
	LogMessageMgr *m_pMsg;
	StatusValues m_statusValues;

};


