/*
 * McastProcess.cpp
 *
 *  Created on: Sep 28, 2016
 *      Author: johnwcurry
 */

#include "McastProcess.h"
#include "Encryptor.h"
#include <ctime>
#include <chrono>
#include "McastRxChgSettingsMsg.h"
#include "DictConfig.h"

McastProcess::McastProcess()
{
	m_pMsg = LogMessageMgr::pLogMessageMgr;
	m_doAbort = false;
	m_aborted = false;
	m_bInitialized = false;
	m_pMcastReceive = nullptr;
	m_pMcastSend = nullptr;
	m_pIfDefinitions = nullptr;
	m_InitializeTimer.SetDelay(5.0, true);
	m_SendTimer.SetDelay(2.0, true);
	std::thread(McastProcess::MainThread, this).detach();

}

McastProcess::~McastProcess()
{
	m_pMsg->xsprintf(LogMessageMgr::PRT_ALWAYS,
			"McastProcess destructor called");
	m_doAbort = true;
	while (!m_aborted)
		usleep(100 * 1000);
	ClearAllocations();
	m_pMsg->xsprintf(LogMessageMgr::PRT_ALWAYS, "McastProcess has terminated");
}
void McastProcess::ClearAllocations()
{
	if (m_pMcastReceive != nullptr)
	{
		delete m_pMcastReceive;
		m_pMcastReceive = nullptr;
	}
	if (m_pMcastSend != nullptr)
	{
		delete m_pMcastSend;
		m_pMcastSend = nullptr;
	}
	if (m_pIfDefinitions != nullptr)
	{
		delete m_pIfDefinitions;
		m_pIfDefinitions = nullptr;
	}

}

void McastProcess::Initialize()
{
	const char szWho[] = "McastProcess::Initialize";
	if (m_bInitialized || !m_InitializeTimer.Expired())
		return;
	ClearAllocations();
	DictConfig dictConfig;
	GetIfAddress getIfAddress;
	m_pIfDefinitions = getIfAddress.GetAddresses();
	if (m_pIfDefinitions->Definitions.size() == 0)
	{
		m_pMsg->xsprintf("3B795D16-A8DD-4DFE-9738-1A13D73966D9", 0,
				"%s no Interface Definitions", szWho);
		return;
	}
	m_pMcastSend = new McastSend(
			m_pIfDefinitions->Definitions[0].InterfaceAddress);
	if (!m_pMcastSend->Connected())
		return;
	m_pMcastReceive = new McastReceive(
			m_pIfDefinitions->Definitions[0].InterfaceAddress);
	if (!m_pMcastReceive->Connected())
		return;
	m_StatusInfo.set_secwalluuid(dictConfig.GetCfgEntryAsString(DictConfig::EnCfgType::MachineUuid));
	m_StatusInfo.set_secwallname(dictConfig.GetCfgEntryAsString(DictConfig::EnCfgType::MachineName));
	m_StatusInfo.set_newsecwallnamepresent(false);
	m_StatusInfo.set_ipaddrrx(
			m_pIfDefinitions->Definitions[0].InterfaceAddress);
	m_StatusInfo.set_ipaddrtx(
			m_pIfDefinitions->Definitions[1].InterfaceAddress);
	m_StatusInfo.set_netmaskrx(
			m_pIfDefinitions->Definitions[0].InterfaceNetMask);
	m_StatusInfo.set_netmasktx(
			m_pIfDefinitions->Definitions[1].InterfaceNetMask);
	m_StatusInfo.set_multicastlistenaddr(MCAST_RX_ADDR);
	m_StatusInfo.set_multicastlistenport(MCAST_RX_PORT);
	m_StatusInfo.set_tcplistenportrx(TCP_RX_PORT);
	m_StatusInfo.set_tcpsendporttx(TCP_TX_PORT);
	m_StatusInfo.set_gatewayrx("0.0.0.0");
	m_StatusInfo.set_gatewaytx("0.0.0.0");
	m_StatusInfo.set_newiprxpresent(false);
	m_StatusInfo.set_newiptxpresent(false);
	m_StatusInfo.set_newportrxpresent(false);
	m_StatusInfo.set_newporttxpresent(false);
	m_StatusInfo.set_newrxmaskpresent(false);
	m_StatusInfo.set_newtxmaskpresent(false);
	SetLastErrTime();
	m_bInitialized = true;
}
void McastProcess::SetBoxDateTime(BoxDate* pDate)
{
	auto now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);
	struct tm *parts = std::localtime(&now_c);
	pDate->set_year(parts->tm_year - 100);
	pDate->set_month(parts->tm_mon + 1);
	pDate->set_day(parts->tm_mday);
	pDate->set_hour(parts->tm_hour);
	pDate->set_minute(parts->tm_min);
	pDate->set_second(parts->tm_sec);
	pDate->set_newtimepresent(false);
}
void McastProcess::SetLastErrTime()
{
	PortStatus *pStatus = m_StatusInfo.mutable_mcaststatus();
	SetBoxDateTime(pStatus->mutable_lasterrtime());
}
void McastProcess::SendStatus()
{
	if (!m_bInitialized || !m_InitializeTimer.Expired())
		return;

	DictConfig dictConfig;
	SetBoxDateTime(m_StatusInfo.mutable_currentdate());
	PortStatus *pStatus = m_StatusInfo.mutable_mcaststatus();
	pStatus->set_sent(pStatus->sent() + 1);
	std::string encoded;
	m_StatusInfo.SerializeToString(&encoded);
	Encryptor encryptor;
	EncryptorEnt *pEncrypted = encryptor.DoEncrypt(&encoded,EncryptorEnt::EnMessageType::StatusMsg,dictConfig.GetUuid());
	m_pMcastSend->XmitData(pEncrypted, EncryptorEnt::EnDataDelete::DeleteData);
}
bool McastProcess::CheckForReceive()
{
	const char szWho[] = "McastProcess::CheckForReceive";
	PortStatus *pStatus = m_StatusInfo.mutable_mcaststatus();
	bool validDataPresent = true;
	EncryptorEnt *pData;
	if (!m_pMcastReceive->GetStackedData(pData))
		return false;
	pStatus->set_received(pStatus->received() + 1);
	Encryptor encryptor;
	EncryptorEnt *pDecrypted = encryptor.DoDecrypt(pData,	EncryptorEnt::EnDataDelete::DeleteData);
	switch (pDecrypted->MsgStatus)
	{
	case EncryptorEnt::EnMsgStatus::StatusOK:
	{
		switch (pDecrypted->MessageType)
		{
		case EncryptorEnt::EnMessageType::ChgSettingsMsg:
		{
			McastRxChgSettingsMsg mcastRxStatsMsg;
			mcastRxStatsMsg.ProcessStatsMsg(pDecrypted,	EncryptorEnt::EnDataDelete::DeleteData);
		}
			break;
		default:
			pStatus->set_unknownmsg(pStatus->unknownmsg() + 1);
			SetLastErrTime();
			m_pMsg->xsprintf(0, "%s unknown message type %d", szWho,
					pDecrypted->MessageType);
			delete pDecrypted;
		}

	}
		break;
	case EncryptorEnt::EnMsgStatus::StatusBadStructure:
		pStatus->set_badpkt(pStatus->badpkt() + 1);
		SetLastErrTime();
		break;

	case EncryptorEnt::EnMsgStatus::StatusChecksumFail:
		pStatus->set_crcerr(pStatus->crcerr() + 1);
		SetLastErrTime();
		break;

	case EncryptorEnt::EnMsgStatus::StatusUuidFail:
		pStatus->set_uuiderr(pStatus->uuiderr() + 1);
		SetLastErrTime();
		break;

	case EncryptorEnt::EnMsgStatus::StatusUnknown:
		pStatus->set_unknownerr(pStatus->unknownerr() + 1);
		SetLastErrTime();
		break;
	}
	return validDataPresent;

}
void McastProcess::MainThread(McastProcess *pMc)
{
	unsigned int sleepTime = 100 * 1000;
	NraTimer xmitTimer(2.0, true);
	NraTimer retryTimer(10.0, true);
	try
	{

		while (!pMc->m_doAbort)
		{
			usleep(sleepTime);
			if (!pMc->m_bInitialized)
			{
				pMc->Initialize();
				continue;
			}
			pMc->SendStatus();
			pMc->CheckForReceive();
		}
	} catch (...)
	{
	}
	pMc->m_aborted = true;
}

