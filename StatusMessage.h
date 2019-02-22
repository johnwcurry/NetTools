/*
 * StatusMessage.h
 *
 *  Created on: Sep 27, 2016
 *      Author: johnwcurry
 */

#pragma once
#include "EncryptorEnt.h"
#include "src-gen/StatusMessage.pb.h"

class StatusMessage
{
public:
	StatusMessage();
	virtual ~StatusMessage();

	void SetIpAddrSender(std::string addr);
	void SetIpAddrSenReceiver(std::string addr);
	void SetPortNumberSender(int port);
	void SetPortNumberReceiver(int port);
	EncryptorEnt * Serialize();

private:
	StatusInfo *pMainInfo;
};


