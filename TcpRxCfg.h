/*
 * TcpRxCfg.h
 *
 *  Created on: Sep 11, 2016
 *      Author: johnwcurry
 */

#ifndef TCPRXCFG_H_
#define TCPRXCFG_H_

#include "TcpRx.h"

class TcpRxCfg: public TcpRx
{
public:
	TcpRxCfg(std::string interface, int listenPort);
	virtual ~TcpRxCfg();
private:
	virtual void CreateClient(struct sockaddr_in client_sockaddr);
};

#endif /* TCPRXCFG_H_ */
