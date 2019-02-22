/*
 * TcpRxClientBase.h
 *
 *  Created on: Sep 11, 2016
 *      Author: johnwcurry
 */

#pragma once
#include "TcpCommon.h"

class TcpRxClientBase: public TcpCommon
{
public:
	TcpRxClientBase(struct sockaddr_in client_sockaddr);
	virtual ~TcpRxClientBase();
	bool Connected() {return m_connected;};

protected:
	struct sockaddr_in m_client_sockaddr;
	bool m_connected;

};


