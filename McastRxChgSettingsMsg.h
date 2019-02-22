/*
 * McastRxStatsMsg.h
 *
 *  Created on: Oct 10, 2016
 *      Author: johnwcurry
 */

#pragma once
#include <unistd.h>
#include "LogMessageMgr.h"
#include "McastReceive.h"
#include "McastSend.h"
#include "GetIfAddress.h"
#include "NraTimer.h"
#include "NetCommon.h"
#include "src-gen/StatusMessage.pb.h"
class McastRxChgSettingsMsg
{
public:
	McastRxChgSettingsMsg();
	virtual ~McastRxChgSettingsMsg();
	void ProcessStatsMsg(EncryptorEnt *pDecrypted,EncryptorEnt::EnDataDelete deleteData );

private:
	LogMessageMgr *m_pMsg;
};


