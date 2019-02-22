/*
 * GetIfAddress.h
 *
 *  Created on: Sep 6, 2016
 *      Author: johnwcurry
 */
#pragma once
#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <string>
#include <vector>
#include "LogMessageMgr.h"

class GetIfAddress
{
public:
	typedef struct
	{
		int InterfaceType;
		std::string InterfaceName;
		std::string InterfaceAddress;
		std::string InterfaceNetMask;
	} IfDefinition;
	typedef struct
	{
		std::vector<IfDefinition> Definitions;
	} IfDefinitions;
	GetIfAddress();
	virtual	~GetIfAddress();
	IfDefinitions *GetAddresses();
private:
	LogMessageMgr *m_pMsg;
	static const std::string m_localHost;
};

