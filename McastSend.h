/*
 * McastSend.h
 *
 *  Created on: Sep 10, 2016
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
#include "EncryptorEnt.h"
#include "NetCommon.h"
#include "LogMessageMgr.h"


class McastSend : public NetCommon
{
public:
	McastSend(std::string localIP);
	virtual ~McastSend();
	int  XmitData(EncryptorEnt *pEntry,EncryptorEnt::EnDataDelete deleteData);
	bool Connected() {return m_connected;}

private:
	bool Initialize();

private:

	std::string m_localIP;
	int m_socket;
	bool m_connected;
	struct sockaddr_in m_servAddr;
	fd_set			m_fdsetRead; /* results of the select() */
	fd_set			m_fdsetAll;  /* holds all the handles */
	fd_set			m_fdsetWrite; /* results of the connect() */
	LogMessageMgr *m_pMsg;
};


