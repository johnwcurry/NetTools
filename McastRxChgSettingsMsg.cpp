/*
 * McastRxChgSettingsMsg.cpp
 *
 *  Created on: Oct 10, 2016
 *      Author: johnwcurry
 */

#include "McastRxChgSettingsMsg.h"

McastRxChgSettingsMsg::McastRxChgSettingsMsg()
{
	m_pMsg = LogMessageMgr::pLogMessageMgr;

}

McastRxChgSettingsMsg::~McastRxChgSettingsMsg()
{
	// TODO Auto-generated destructor stub
}

void McastRxChgSettingsMsg::ProcessStatsMsg(EncryptorEnt *pDecrypted,EncryptorEnt::EnDataDelete deleteData )
{
	const char szWho[] = "McastRxChgSettingsMsg.ProcessStatsMsg";
	try
	{
		StatusInfo statusInfo;
		bool isValid = statusInfo.ParseFromArray(pDecrypted->pDataPtr,pDecrypted->DataLength);
		if (!isValid)
		{
			m_pMsg->xsprintf(0,"%s invalid data structure",szWho);
		}
		else
		{

		}
		if (deleteData == EncryptorEnt::EnDataDelete::DeleteData)
		{
			delete pDecrypted;
		}

	}catch(...)
	{
		m_pMsg->xsprintf(0,"%s exception caught",szWho);
	}

}

