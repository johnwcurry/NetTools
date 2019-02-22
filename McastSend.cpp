/*
 * McastSend.cpp
 *
 *  Created on: Sep 10, 2016
 *      Author: johnwcurry
 */
#include <string.h>
#include "McastSend.h"
#include "Encryptor.h"

McastSend::McastSend(std::string localIP)
{
	m_localIP = localIP;
	m_pMsg = LogMessageMgr::pLogMessageMgr;
	m_socket = INVALID_SOCKET;
	m_statusValues.SetCategory("TXMCAST");
	FD_ZERO(&m_fdsetRead);
	FD_ZERO(&m_fdsetAll);
	FD_ZERO(&m_fdsetWrite);
	memset(&m_servAddr, 0, sizeof(m_servAddr));
	m_connected = Initialize();

}

McastSend::~McastSend()
{
	if (m_socket != INVALID_SOCKET)
		close(m_socket);
	m_pMsg->xsprintf(LogMessageMgr::PRT_ALWAYS,"McastSend has terminated");
}

bool McastSend::Initialize()
{
	static char szOutputLabel[] = "McastSend::Initialize";
	unsigned char ttl = 1;
	struct in_addr localInterface;
	m_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_socket < 0)
	{
		m_pMsg->xsprintf(LogMessageMgr::ERR_ALWAYS,
				"%s Cannot create socket %s", szOutputLabel,
				MCAST_SEND_ADDR.c_str());
		return (false);
	}
	memset((char *) &m_servAddr, 0, sizeof(m_servAddr));
	m_servAddr.sin_family = AF_INET;
	m_servAddr.sin_addr.s_addr = inet_addr(MCAST_SEND_ADDR.c_str());
	m_servAddr.sin_port = htons(MCAST_SEND_PORT);

	//Set local interface for outbound multicast datagrams.
	// 	 The IP address specified must be associated with a local,
	//	multicast capable interface.

	localInterface.s_addr = inet_addr(m_localIP.c_str());

	if (setsockopt(m_socket, IPPROTO_IP, IP_MULTICAST_IF,
			(char *) &localInterface, sizeof(localInterface)) < 0)

	{
		m_pMsg->xsprintf(LogMessageMgr::ERR_ALWAYS,
				"%s Cannot create local interface %s", szOutputLabel,
				MCAST_SEND_ADDR.c_str());
		return (false);
	}

	int result = setsockopt(m_socket, IPPROTO_IP, IP_MULTICAST_TTL, &ttl,
			sizeof(ttl));
	if (result != SUCCESS)
	{
		m_pMsg->xsprintf(LogMessageMgr::ERR_ALWAYS,
				"%s Cannot set IP_MULTICAST_TTL %s", szOutputLabel,
				MCAST_SEND_ADDR.c_str());
		return (false);
	}

	m_pMsg->xsprintf(LogMessageMgr::PRT_ALWAYS,
			"%s Multicast Sending to %d host %s", szOutputLabel,
			MCAST_SEND_PORT, MCAST_SEND_ADDR.c_str());

	FD_SET((uint32_t )m_socket, &m_fdsetWrite);	//for connect
	FD_SET((uint32_t )m_socket, &m_fdsetRead);	//for data comming back

	return true;
}
/**
 * Send Data via Multicast.
 * If deleteData == EncryptorEnt::EnDeleteData,
 * 	Delete pEntry on exit;
 */
int McastSend::XmitData(EncryptorEnt *pEntry,
		EncryptorEnt::EnDataDelete deleteData)
{
	static char szOutputLabel[] = "McastSend.XmitData";
	int iNumSent = 0;
	int iError = 0;

	if (m_connected)
	{

		iNumSent = sendto(m_socket, pEntry->pDataPtr, pEntry->DataLength, 0,
				(struct sockaddr *) &m_servAddr, sizeof(m_servAddr));

		if (iNumSent == SOCKET_ERROR)
		{
			m_pMsg->xsprintf("58294219-EBE2-4E7F-B40E-522876987006", 0, "%s %s",
					szOutputLabel, strerror(errno));
			m_statusValues.UpdateEntry("SOCKET_ERROR",1);

		}
		else if (iNumSent != (int) pEntry->DataLength)//some # of bytes returned
		{
			m_pMsg->xsprintf("B3474058-D424-4B8E-A4F3-0360C798B66D", 0,
					"%s incomplete transmit", szOutputLabel);
			m_statusValues.UpdateEntry("XMIT_INCOMPLETE",1);
		}
		else
		{
			m_statusValues.UpdateEntry("PACKETS_SENT",1);
		}
	}
	if (deleteData == EncryptorEnt::EnDataDelete::DeleteData)
	{
		delete pEntry;
	}
	return iError;

}

