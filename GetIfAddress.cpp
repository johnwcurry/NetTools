/*
 * GetIfAddress.cpp
 *
 *  Created on: Sep 6, 2016
 *      Author: johnwcurry
 */

#include "GetIfAddress.h"
const std::string GetIfAddress::m_localHost = "127.0.0.1";

GetIfAddress::GetIfAddress()
{
	m_pMsg = LogMessageMgr::pLogMessageMgr;

}

GetIfAddress::~GetIfAddress()
{
	// TODO Auto-generated destructor stub
}
GetIfAddress::IfDefinitions* GetIfAddress::GetAddresses()
{
	static const char szWho[] = "GetIfAddress::GetAddresses";
	struct ifaddrs * ifAddrStruct = nullptr;
	struct ifaddrs * ifa = nullptr;
	GetIfAddress::IfDefinitions *pDefinitions = new GetIfAddress::IfDefinitions;

	getifaddrs(&ifAddrStruct);

	for (ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next)
	{
		if (!ifa->ifa_addr)
		{
			continue;
		}
		if (ifa->ifa_addr->sa_family == AF_INET)
		{ // check it is IP4
			// is a valid IP4 Address
			void * tmpAddrPtr = &((struct sockaddr_in *) ifa->ifa_addr)->sin_addr;
			char addressBuffer[INET_ADDRSTRLEN];
			memset(addressBuffer,0,sizeof(addressBuffer));
			inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			if (m_localHost.compare(addressBuffer) == 0)
				continue;
			GetIfAddress::IfDefinition ifDefinition;
			ifDefinition.InterfaceType = AF_INET;
			ifDefinition.InterfaceName = ifa->ifa_name;
			ifDefinition.InterfaceAddress = addressBuffer;
			tmpAddrPtr = &((struct sockaddr_in *) ifa->ifa_netmask)->sin_addr;
			inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			ifDefinition.InterfaceNetMask = addressBuffer;
			pDefinitions->Definitions.push_back(ifDefinition);
			m_pMsg->xsprintf(m_pMsg->PRT_ALWAYS,"%s: %s %s",szWho,ifDefinition.InterfaceName.c_str(),ifDefinition.InterfaceAddress.c_str());

			//printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
		}
		else if (ifa->ifa_addr->sa_family == AF_INET6)
		{ // check it is IP6
			// is a valid IP6 Address
			void * tmpAddrPtr = &((struct sockaddr_in6 *) ifa->ifa_addr)->sin6_addr;
			char addressBuffer[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
			GetIfAddress::IfDefinition ifDefinition;
			ifDefinition.InterfaceType = AF_INET6;
			ifDefinition.InterfaceName = ifa->ifa_name;
			ifDefinition.InterfaceAddress = addressBuffer;
			inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			ifDefinition.InterfaceNetMask = addressBuffer;
			//pDefinitions->Definitions.push_back(ifDefinition);

			//printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
		}
	}
	if (ifAddrStruct != nullptr)
		freeifaddrs(ifAddrStruct);

	return pDefinitions;

}

