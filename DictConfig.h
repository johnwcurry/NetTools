/*
 * DictConfig.h
 *
 *  Created on: Oct 11, 2016
 *      Author: johnwcurry
 */

#pragma once

#include "DictParse.h"
#include "UuidHandler.h"

class DictConfig: public DictParse
{
public:
	enum class EnCfgType
	{
		SenderEthCfg,
		ReceiverEthCfg,
		MachineUuid,
		MachineName,
		FileLoc,
		LocalOptsFile
	};
	typedef struct
	{
		EnCfgType cfgType;
		const std::string cfgName;
		std::string cfgValue;
	}CfgData;
	DictConfig();
	virtual ~DictConfig();
	void ReadConfigFile(const std::string cfgFile);
	bool GetCfgEntry(EnCfgType type, std::string &value );
	std::string GetCfgEntryAsString(EnCfgType type);
	UuidHandler::Uuid GetUuid() const {return uuidSecWall_;}



private:
	static CfgData m_CfgValues[];
	static UuidHandler::Uuid uuidSecWall_;
	static const std::string SENDER_ETHCFG;
	static const std::string RECEIVER_ETHCFG;
	static const std::string MACHINE_UUID;
	static const std::string MACHINE_NAME;
	static const std::string FILE_LOC;
	static const std::string LOCALOPTS_FILE;
private:
	void PopulateCfg();
};


