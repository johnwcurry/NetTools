/*
 * McastReceive.cpp
 *
 *  Created on: Sep 12, 2016
 *      Author: johnwcurry
 */

#include "McastReceive.h"
#include <string.h>
#include <cstring>

const size_t McastReceive::MAX_QUEUE_LENGTH = 2000;

McastReceive::McastReceive(std::string localIP)
{
	m_localIP = localIP;
	m_doAbort = false;
	m_aborted = false;
	m_connected = false;
	m_socket = INVALID_SOCKET;
	m_statusValues.SetCategory("RXMCAST");
	m_LastActivity = time(nullptr);
	m_connected = Initialize();
	std::thread(McastReceive::MainThread, this).detach();

}

McastReceive::~McastReceive()
{
	m_doAbort = true;
	while (!m_aborted)
		usleep(100 * 1000);
	if (m_socket != INVALID_SOCKET)
		close(m_socket);
	FlushRxQueue();
	m_pMsg->xsprintf(LogMessageMgr::PRT_ALWAYS,"McastReceive has terminated");
}
/**
 * returns true if a packet was received.
 */
bool McastReceive::CheckForReceive()
{
	static const char szWho[] = "McastReceive::CheckForReceive";
	bool bRetVal = false;
	bool bHaveData = false;

	McRxEvtStatus mbStatus = CheckForReadEvent();
	switch (mbStatus)
	{
	case es_SelectFail:	//select failed:  pListen marked as failed
		break;

	case es_Exception:  //an exception occurred
		m_connected = false;	//mark the connection as failed
		break;

	case es_Event:		//A read event occurred
		bHaveData = true;		//Have data from OWL
		break;

	case es_UnknownEvent:
		m_connected = false;	//mark the connection as failed
		break;

	default:
		break;
	}

	if (bHaveData)		//is there some data available?
	{
		EncryptorEnt *pEnt = DataRx();
		if (pEnt == nullptr)
		{
			m_connected = false;	//mark the connection as failed
		}
		else
		{
			m_LastActivity = time(nullptr);
			bRetVal = true;
			switch(pEnt->MsgStatus)
			{
			case EncryptorEnt::EnMsgStatus::StatusOK:
				m_statusValues.UpdateEntry("GOOD PACKET",1);
				StackRx(pEnt);
				break;
			case  EncryptorEnt::EnMsgStatus::StatusBadStructure:
				m_statusValues.UpdateEntry("BAD STRUCTURE",1);
				delete pEnt;
				break;
			case  EncryptorEnt::EnMsgStatus::StatusChecksumFail:
				m_statusValues.UpdateEntry("CHECKSUM",1);
				delete pEnt;
				break;
			case  EncryptorEnt::EnMsgStatus::StatusUuidFail:
				m_statusValues.UpdateEntry("UUID",1);
				delete pEnt;
				break;
			case  EncryptorEnt::EnMsgStatus::StatusUnknown:
				m_pMsg->xsprintf("423D0F19-FCA1-45FD-9170-39B62CA4D5E4",0,"%s Unknown Status", szWho);
				m_statusValues.UpdateEntry("STATUS_UNKNOWN",1);
				delete pEnt;
				break;

			default:
				m_pMsg->xsprintf("{AC94E96F-8E65-48F0-A6DD-4307038046E9}",0,"%s undefined Status", szWho);
				m_statusValues.UpdateEntry("STATUS_UNDEFINED",1);
				delete pEnt;
				break;
			}
		}
	}
	return (bRetVal);
}

McastReceive::McRxEvtStatus McastReceive::CheckForReadEvent(void)
{
	char szFunction[] = "McastReceive::CheckForReadEvent";
	timeval tWaitTime;
	tWaitTime.tv_sec = 0;
	tWaitTime.tv_usec = 0;
	fd_set fdRead;
	fd_set ExceptFds;
	McRxEvtStatus mbReturn = es_NoEvent;

	memmove(&fdRead, &m_fdsetRead, sizeof(fdRead));
	memmove(&ExceptFds, &m_fdsetRead, sizeof(fdRead));
	int nNumberOfEvents = select( FD_SETSIZE, &fdRead, 0, &ExceptFds,
			&tWaitTime);
	int nErrorCode = errno;
	if (nNumberOfEvents < 0)
	{
		mbReturn = es_SelectFail;
		m_pMsg->xsprintf(0, "%s:  select() ERROR, error = %d (%s)", szFunction,
				nErrorCode, std::strerror(nErrorCode));

	}
	else if (nNumberOfEvents > 0) // some events here
	{
		if (FD_ISSET(m_socket, &ExceptFds))	//exception on connect
		{
			mbReturn = es_Exception;
			int iLevel = SOL_SOCKET;
			int iOptName = SO_ERROR;
			int iOptVal;

			socklen_t iOptLen = sizeof(iOptVal);

			getsockopt(m_socket, iLevel, iOptName, (char *) &iOptVal, &iOptLen);

			m_pMsg->xsprintf(0, "%s: Exception  error = %d (%s)", szFunction,
					iOptVal, std::strerror(nErrorCode));
		}
		else if (FD_ISSET(m_socket, &fdRead)) //if something here
			mbReturn = es_Event;
		else
			mbReturn = es_UnknownEvent;	//an event, but not for the named socket

	}
	return (mbReturn);
}

bool McastReceive::SetRxBufferLength()
{
	const char szOutputLabel[] = "McastReceive::SetRxBufferLength";
	int iBufLen = 0;
	int iBufNew = 0;
	int iBufDesired = 2000000;
	unsigned int iOptLen = sizeof(iBufLen);

	int iOptLenSet = sizeof(iBufDesired);
	int iRetVal;

	iRetVal = getsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char*) &iBufLen,
			&iOptLen);
	if (iRetVal != 0)	//if error
	{
		m_pMsg->xsprintf(0, "%s FAIL(A)", szOutputLabel);
		return false;
	}
	iRetVal = setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char*) &iBufDesired,
			iOptLenSet);
	if (iRetVal != 0)	//if error
	{
		m_pMsg->xsprintf(0, "%s FAIL(B)", szOutputLabel);
		return false;
	}
	iRetVal = getsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char*) &iBufNew,
			&iOptLen);
	if (iRetVal != 0)	//if error
	{
		m_pMsg->xsprintf(0, "%s FAIL(C)", szOutputLabel);
		return false;
	}
	m_pMsg->xsprintf(-1, "%s: old %d new %d", szOutputLabel, iBufLen, iBufNew);

	return true;

}
bool McastReceive::Initialize()
{
	const char szOutputLabel[] = "McastReceive::Initialize";
	struct sockaddr_in servAddr;
	struct ip_mreq group;

	FD_ZERO(&m_fdsetRead);
	FD_ZERO(&m_fdsetAll);
	if (m_socket != INVALID_SOCKET)
	{
		close(m_socket);
	}
	m_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_socket == INVALID_SOCKET)
	{
		m_pMsg->xsprintf(0, "%s Cannot create socket %s", szOutputLabel,
				MCAST_RX_ADDR.c_str());
		return (false);
	}

	//Enable SO_REUSEADDR to allow multiple instances of this
	//application to receive copies of the multicast datagrams.
	int reuse = 1;
	if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse,
			sizeof(reuse)) < 0)
	{
		m_pMsg->xsprintf(0, "%s Cannot Set ReUse %s", szOutputLabel,
				MCAST_RX_ADDR.c_str());
		close(m_socket);
		m_socket = INVALID_SOCKET;
		return (false);
	}
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(MCAST_RX_PORT);
	servAddr.sin_addr.s_addr = INADDR_ANY;
	if (bind(m_socket, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
	{
		m_pMsg->xsprintf(0, "%s Cannot bind port %d host %s", szOutputLabel,
				MCAST_RX_PORT, MCAST_RX_ADDR.c_str());
		close(m_socket);
		m_socket = INVALID_SOCKET;
		return (false);
	}

	if (!SetNonblocking(m_socket, szOutputLabel))
	{
		m_pMsg->xsprintf(0, "%s Cannot unblock port %d host %s", szOutputLabel,
				MCAST_RX_PORT, MCAST_RX_ADDR.c_str());
		close(m_socket);
		m_socket = INVALID_SOCKET;
		return (false);
	}

	//Join the multicast group 226.1.1.1 on the local 203.106.93.94
	//interface. Note that this IP_ADD_MEMBERSHIP option must be
	// called for each local interface over which the multicast
	// datagrams are to be received.

	group.imr_multiaddr.s_addr = inet_addr(MCAST_RX_ADDR.c_str());
	group.imr_interface.s_addr = inet_addr(m_localIP.c_str());
	if (setsockopt(m_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *) &group,
			sizeof(group)) < 0)
	{
		m_pMsg->xsprintf(0, "%s Cannot add membership port %d host %s",
				szOutputLabel, MCAST_RX_PORT, MCAST_RX_ADDR.c_str());
		close(m_socket);
		m_socket = INVALID_SOCKET;
		return (false);
	}
	SetRxBufferLength();
	FD_SET(m_socket, &m_fdsetRead);
	FD_SET(m_socket, &m_fdsetAll);
	m_pMsg->xsprintf(LogMessageMgr::PRT_ALWAYS,
			"%s Multicast Listening to %d host %s", szOutputLabel,
			MCAST_RX_PORT, MCAST_RX_ADDR.c_str());
	return true;

}
/**
 * Get First element on the stack.
 * Returns nullptr if no element available.
 */
bool McastReceive::GetStackedData(EncryptorEnt  *&pEntry)
{
	std::lock_guard<std::recursive_mutex> lock(m_Mutex);
	bool valuePresent = false;
	pEntry = nullptr;
	if (m_dataQue.size() > 0)
	{
		pEntry = m_dataQue.front();
		m_dataQue.pop_front();
		valuePresent = true;
	}
	return valuePresent;

}
/**
 * add new element to end of stack.
 */
void McastReceive::StackRx(EncryptorEnt *pRx)
{
	const char szWho[] = "McastReceive::StackRx";
	std::lock_guard<std::recursive_mutex> lock(m_Mutex);
	try
	{
		if (m_dataQue.size() > MAX_QUEUE_LENGTH)
		{
			m_pMsg->xsprintf(0,
					"%s Max Queue Length %d exceeded.  Flushing entries", szWho,
					MAX_QUEUE_LENGTH);
			FlushRxQueue();
		}
		m_dataQue.push_back(pRx);
	} catch (...)
	{

	}

}
void McastReceive::FlushRxQueue()
{
	std::lock_guard<std::recursive_mutex> lock(m_Mutex);
	try
	{
		for (EncryptorEnt *pEnt : m_dataQue)
		{
			if (pEnt != nullptr)
			{
				delete pEnt;
			}
		}
		m_dataQue.clear();
	} catch (...)
	{

	}

}
/**
 * Returns nullptr if error.
 * Otherwise, returns decrypted packet.
 */
EncryptorEnt *McastReceive::DataRx()
{
	const char szWho[] = "McastReceive::DataRx";
	unsigned char szBuf[70000];
	int iLength = recv(m_socket, (char *) szBuf, sizeof(szBuf), 0);
	if (iLength == SOCKET_ERROR)
	{
		m_pMsg->xsprintf(0, "%s error %d %s %s", szWho, errno,
				MCAST_RX_ADDR.c_str(), strerror(errno));
		return nullptr;
	}
	if (iLength == 0)
	{
		m_pMsg->xsprintf(0, "%s Sender has closed the connection %s ", szWho,
				MCAST_RX_ADDR.c_str());
		return nullptr;
	}
	EncryptorEnt *pEnt = new EncryptorEnt(szBuf,iLength,EncryptorEnt::EnMessageType::StatusMsg);

	return pEnt;
}

void McastReceive::MainThread(McastReceive *pMrx)
{
	try
	{
		unsigned int sleepTime = 100 * 1000;
		NraTimer reconnectTimer(5, true);
		while (!pMrx->m_doAbort)
		{
			usleep(sleepTime);
			if (!pMrx->m_connected)
			{
				if (reconnectTimer.Expired())
				{
					pMrx->m_connected = pMrx->Initialize();
				}
				continue;
			}
			pMrx->CheckForReceive();
		}

	} catch (...)
	{

	}
	pMrx->m_aborted = true;

}

