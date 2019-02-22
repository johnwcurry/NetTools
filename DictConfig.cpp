/*
 * DictConfig.cpp
 *
 *  Created on: Oct 11, 2016
 *      Author: johnwcurry
 */

#include "DictConfig.h"
#include <iostream>
#include <fstream>
const std::string DictConfig::SENDER_ETHCFG = "SENDER_ETHCFG";
const std::string DictConfig::RECEIVER_ETHCFG = "RECEIVER_ETHCFG";
const std::string DictConfig::MACHINE_UUID = "MACHINE_UUID";
const std::string DictConfig::MACHINE_NAME = "MACHINE_NAME";
const std::string DictConfig::FILE_LOC = "FILE_LOC";
const std::string DictConfig::LOCALOPTS_FILE = "LOCALOPTS_FILE";
DictConfig::CfgData DictConfig::m_CfgValues[] =
{
	{DictConfig::EnCfgType::SenderEthCfg,DictConfig::SENDER_ETHCFG,"???"},
	{DictConfig::EnCfgType::ReceiverEthCfg,DictConfig::RECEIVER_ETHCFG,"???"},
	{DictConfig::EnCfgType::MachineUuid,DictConfig::MACHINE_UUID,"???"},
	{DictConfig::EnCfgType::MachineName,DictConfig::MACHINE_NAME,"???"},
	{DictConfig::EnCfgType::FileLoc,DictConfig::FILE_LOC,""},
	{DictConfig::EnCfgType::LocalOptsFile,DictConfig::LOCALOPTS_FILE,"???"}
};
UuidHandler::Uuid DictConfig::uuidSecWall_;

DictConfig::DictConfig()
{


}

DictConfig::~DictConfig()
{

}
std::string DictConfig::GetCfgEntryAsString(EnCfgType type)
{
	static const char szWho[] = "DictConfig::GetCfgEntryAsString";
	std::string value = "";
	if (!GetCfgEntry(type,value))
	{
		m_pMsg->xsprintf(0,"%s config entry not found for index %d",szWho,type);
	}
	return value;


}
bool DictConfig::GetCfgEntry(EnCfgType type, std::string &value )
{
	bool success = false;
	int numElements = sizeof(m_CfgValues)/sizeof(m_CfgValues[0]);
	for (int x = 0; x < numElements; x++)
	{
		if (m_CfgValues[x].cfgType == type)
		{
			success = true;
			value = m_CfgValues[x].cfgValue;
			break;
		}
	}
	return success;
}
void DictConfig::ReadConfigFile(const std::string cfgFile)
{
	static char szWho[] = "DictConfig::ReadConfigFile";
	std::string line;
	std::ifstream myfile(cfgFile);
	if (myfile)
	{
		while (getline(myfile, line).good())
		{
			AddValue(line);
		}
		myfile.close();
		PopulateCfg();

	}
	else
	{
		m_pMsg->xsprintf(0,"%s can't open file %s",szWho,cfgFile.c_str());
	}
}
void DictConfig::PopulateCfg()
{
	static char szWho[] = "DictConfig::PopulateCfg";
	int numElements = sizeof(m_CfgValues)/sizeof(m_CfgValues[0]);
	for (int x = 0; x < numElements; x++)
	{
		std::string value;
		if (!GetKeyValue(m_CfgValues[x].cfgName,m_CfgValues[x].cfgValue))
		{
			m_pMsg->xsprintf(0,"%s element not found: %s",szWho,m_CfgValues[x].cfgName.c_str());
		}
	}
	std::string uuidValue;
	GetCfgEntry(EnCfgType::MachineUuid,uuidValue);
	if (!UuidHandler::uuidFromString(uuidValue,uuidSecWall_))
	{
		m_pMsg->xsprintf(0,"%s unable to convert UUID",szWho);
	}
}

