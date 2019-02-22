/*
 * TcpRx.cpp
 *
 *  Created on: Sep 11, 2016
 *      Author: johnwcurry
 */

#include <string.h>
#include "TcpRx.h"

TcpRx::TcpRx(std::string interface, int listenPort)
{
	m_interface = interface;
	m_listenPort = listenPort;
	m_listenSocket = INVALID_SOCKET;
	m_doAbort = false;
	m_aborted = false;
	std::thread(TcpRx::MainThread, this).detach();

}

TcpRx::~TcpRx()
{
	// TODO Auto-generated destructor stub
}
bool TcpRx::CreateListenSocket()
{
	char szWho[] = "TcpRx::MainThread";
	m_listenSocket = socket(AF_INET , SOCK_STREAM , 0);
	if (m_listenSocket == INVALID_SOCKET)
	{
		m_pMsg->xsprintf(0,"%s can't create Listen Socket",szWho);
		m_aborted = true;
		return false;
	}
	memset(&m_listenSockaddr, 0, sizeof(m_listenSockaddr));
	if (inet_aton(m_interface.c_str(), &m_listenSockaddr.sin_addr) == 0)
	{
		m_pMsg->xsprintf(0,"%s can't convert IP address %s",szWho,m_interface.c_str());
		return false;
	}
	m_listenSockaddr.sin_family = AF_INET;
	m_listenSockaddr.sin_port = htons( m_listenSocket );
	if( bind(m_listenSocket,(struct sockaddr *)&m_listenSockaddr , sizeof(m_listenSockaddr)) < 0)
	{
		m_pMsg->xsprintf(0,"%s can't bind IP address %s port %d",szWho,m_interface.c_str(),m_listenSocket);
		return false;
	}
	int errCode = listen(m_listenSocket,2);
	if (errCode == SOCKET_ERROR)
	{
		m_pMsg->xsprintf(0,"%s can't listen on %s port %d code %d %s",szWho,m_interface.c_str(),m_listenSocket,
				errno,strerror(errno));
		return false;
	}
	return true;

}
bool TcpRx::WaitOnAccept()
{
	char szWho[] = "TcpRx::WaitOnAccept";
	int sockSize = sizeof(struct sockaddr_in);
	struct sockaddr_in client_sockaddr;
	int iClientSock = accept(m_listenSocket,(struct sockaddr *)&client_sockaddr,(socklen_t*)&sockSize);
	if (iClientSock == SOCKET_ERROR)
	{
		m_pMsg->xsprintf(0,"%s accept failed %s port %d code %d %s",szWho,m_interface.c_str(),m_listenSocket,
				errno,strerror(errno));
		return false;
	}
	//char *client_ip = inet_ntoa(client_sockaddr.sin_addr);
	//int client_port = ntohs(client_sockaddr.sin_port);

	return true;
}

void TcpRx::MainThread(TcpRx *pTcpRx)
{
	char szWho[] = "TcpRx::MainThread";

	pTcpRx->m_listenSocket = socket(AF_INET , SOCK_STREAM , 0);
	if (!pTcpRx->CreateListenSocket())
	{
		pTcpRx->m_aborted = true;
		return;
	}
	try
	{
		while (!pTcpRx->m_doAbort)
		{



		}

	}
	catch(...)
	{

	}
	pTcpRx->m_pMsg->xsprintf(LogMessageMgr::PRT_ALWAYS,"%s has terminated",szWho);
	pTcpRx->m_aborted = true;

}


