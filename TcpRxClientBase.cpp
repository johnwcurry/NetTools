/*
 * TcpRxClientBase.cpp
 *
 *  Created on: Sep 11, 2016
 *      Author: johnwcurry
 */

#include "TcpRxClientBase.h"

TcpRxClientBase::TcpRxClientBase(struct sockaddr_in client_sockaddr)
{
	const char szWho[] = "TcpRxClientBase::TcpRxClientBase";
	m_connected = true;
	m_client_sockaddr = client_sockaddr;
	char *client_ip = inet_ntoa(m_client_sockaddr.sin_addr);
	int client_port = ntohs(m_client_sockaddr.sin_port);
	m_pMsg->xsprintf(LogMessageMgr::PRT_ALWAYS,"%s connected %s:%d",szWho,client_ip,client_port);

}

TcpRxClientBase::~TcpRxClientBase()
{
	// TODO Auto-generated destructor stub
}

