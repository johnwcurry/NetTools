/*
 * StatusMessage.cpp
 *
 *  Created on: Sep 27, 2016
 *      Author: johnwcurry
 */

#include "StatusMessage.h"

StatusMessage::StatusMessage()
{
	pMainInfo = new StatusInfo;

}

StatusMessage::~StatusMessage()
{
	delete pMainInfo;
}
void StatusMessage::SetIpAddrSender(std::string addr)
{
	pMainInfo->set_ipaddrtx(addr);
}
void StatusMessage::SetIpAddrSenReceiver(std::string addr)
{
	pMainInfo->set_ipaddrrx(addr);
}
void StatusMessage::SetPortNumberSender(int port)
{
	pMainInfo->set_porttx(port);
}
void StatusMessage::SetPortNumberReceiver(int port)
{
	pMainInfo->set_portrx(port);
}
EncryptorEnt * StatusMessage::Serialize()
{
	std::string strOut;
	pMainInfo->SerializeToString(&strOut);
	int iLen = strOut.length();
	EncryptorEnt *pEnt = new EncryptorEnt(strOut.c_str(),iLen,EncryptorEnt::EnMessageType::StatusMsg);

	/*
	FILE *fp = fopen("/boxdev/statsdat.bin","wb");
	fwrite(pEnt->pData,sizeof(unsigned char),pEnt->DataLength,fp);
	fclose(fp);
	*/


	//StatusInfo statsInfo;
	//statsInfo.ParseFromArray(pEnt->pData,pEnt->DataLength);

	//std::string saddRx = statsInfo.ipaddrrx();


	return pEnt;
}

