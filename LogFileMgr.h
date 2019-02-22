/*
 * LogFileMgr.h
 *
 *  Created on: Sep 7, 2016
 *      Author: johnwcurry
 */
#pragma once
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <chrono>
#include <ctime>
#include <list>

class LogFileMgr
{
public:
	typedef struct
	{
		std::string fileName;
		size_t fileSize;
	}FileSize;
	LogFileMgr(std::string filePath);
	virtual ~LogFileMgr();
public:
	FILE * GetFp();
	std::string GetLogFileName();

private:
	std::string CreateNewFileName();
	void ParseFileName(std::string fileName);
	bool GetFileTime(std::string fileName, std::chrono::system_clock::time_point &tpOut);
	size_t GetFilesInDirectory (std::list<LogFileMgr::FileSize> &fileSizes);

private:
	static const std::string LOGNAME;
	static const std::string LOGEXT;
	static const size_t MAX_FILE_STORAGE;
	static const __off_t MAX_FILE_LENGTH;
	static const double FILE_CHECK_TIME;
	std::string m_filePath;
	std::string m_currentFileName;
	std::chrono::system_clock::time_point m_tCheck;
	FILE *m_fp;



};

