/*
 * TcpCommon.h
 *
 *  Created on: Sep 11, 2016
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
#include "NetCommon.h"

class TcpCommon: public NetCommon
{
public:
	typedef enum
		{
			enUnknown,
			enFatalError,
			enAddrOK,
			enFailed,
			enPending,
			enListenStart,
			enListenWait,
			enConnected,
			enClosed,
			enOK
		}enStatus;
		typedef enum
		{
			enRxUnknown,
			enRxFailed,
			enRxNoData,
			enRxGood,
			enRxClosed,
			enRxPartial
		}enRxStatus;
	TcpCommon();
	virtual ~TcpCommon();


};


