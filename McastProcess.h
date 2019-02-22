/*
 * McastProcess.h
 *
 *  Created on: Sep 28, 2016
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
class McastProcess : public NetCommon
{
public:
	McastProcess();
	virtual ~McastProcess();

private:
	void ClearAllocations();
	void Initialize();
	void SetLastErrTime();
	void SetBoxDateTime(BoxDate* pDate);
	void SendStatus();
	bool CheckForReceive();

private:
	bool m_doAbort;
	bool m_aborted;
	bool m_bInitialized;
	LogMessageMgr *m_pMsg;
	McastReceive *m_pMcastReceive;
	McastSend *m_pMcastSend;
	NraTimer m_InitializeTimer;
	NraTimer m_SendTimer;
	StatusInfo m_StatusInfo;
	GetIfAddress::IfDefinitions *m_pIfDefinitions;
	static void MainThread(McastProcess *pMc);
};


