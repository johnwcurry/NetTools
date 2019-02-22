/*
 * TcpRx.h
 *
 *  Created on: Sep 11, 2016
 *      Author: johnwcurry
 */

#pragma once

#include "TcpCommon.h"
#include "TcpRxClientBase.h"
#include <vector>

class TcpRx: public TcpCommon
{
public:
	TcpRx(std::string interface, int listenPort);
	virtual ~TcpRx();

private:
	static void MainThread(TcpRx *pTcpRx);
	bool CreateListenSocket();
	bool WaitOnAccept();
protected:
	virtual void CreateClient(struct sockaddr_in client_sockaddr) = 0;


private:
	std::string m_interface;
	int	m_listenPort;
	int m_listenSocket;

	struct sockaddr_in m_listenSockaddr;
	bool m_doAbort;
	bool m_aborted;

protected:
	std::vector<TcpRxClientBase> m_clients;
};


