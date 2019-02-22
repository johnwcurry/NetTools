/*
 * TcpRxClientCfg.h
 *
 *  Created on: Sep 11, 2016
 *      Author: johnwcurry
 */

#pragma once
#include "TcpRxClientBase.h"

class TcpRxClientCfg: public TcpRxClientBase
{
public:
	TcpRxClientCfg(struct sockaddr_in client_sockaddr);
	virtual ~TcpRxClientCfg();
};


