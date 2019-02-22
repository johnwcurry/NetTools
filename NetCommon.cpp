/*
 * NetCommon.cpp
 *
 *  Created on: Sep 12, 2016
 *      Author: johnwcurry
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include "NetCommon.h"
const int NetCommon::SUCCESS = 0;
const int NetCommon::SOCKET_ERROR = -1;
const int NetCommon::INVALID_SOCKET = -1;
const int NetCommon::MCAST_SEND_PORT = 49165;
const std::string  NetCommon::MCAST_SEND_ADDR = "239.0.1.20";
const int NetCommon::MCAST_RX_PORT = 49166;
const std::string  NetCommon::MCAST_RX_ADDR = "239.0.1.21";
const int NetCommon::TCP_RX_PORT = 49167;
const int NetCommon::TCP_TX_PORT = 49168;
NetCommon::NetCommon()
{
	m_pMsg = LogMessageMgr::pLogMessageMgr;
}

NetCommon::~NetCommon()
{
	// TODO Auto-generated destructor stub
}
void NetCommon::SetOptionReuse(int socketNumber, const char* pFromWhere)
{
	const char szWho[] = "NetCommon::SetOptionReuse";
	int enable = 1;
	if (setsockopt(socketNumber, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
	{
		m_pMsg->xsprintf(0,"%s %s failed",szWho,pFromWhere);
	}
}
bool NetCommon::SetNonblocking(int fd, const char * pFromWhere)
{
	const char szWho[] = "NetCommon::SetNonblocking";
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == SOCKET_ERROR)
    {
    	m_pMsg->xsprintf(0,"%s unable to set non Blocking(F_GETFL) from %s",szWho,pFromWhere);
    	return false;
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == SOCKET_ERROR)
    {
    	m_pMsg->xsprintf(0,"%s unable to set non Blocking(O_NONBLOCK) from %s",szWho,pFromWhere);
    	return false;
    }
    return true;

}

