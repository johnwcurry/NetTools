/*
 * DataMessageMgr.cpp
 *
 *  Created on: Sep 13, 2016
 *      Author: johnwcurry
 */

#include "DataMessageMgr.h"

DataMessageMgr::DataMessageMgr()
{
	m_pMsg = LogMessageMgr::pLogMessageMgr;

}

DataMessageMgr::~DataMessageMgr()
{
	// TODO Auto-generated destructor stub
}

void DataMessageMgr::IncomingMsg(unsigned char *pData,int dataLength, const char *pFromWhere)
{

}

