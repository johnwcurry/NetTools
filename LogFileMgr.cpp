/*
 * LogFileMgr.cpp
 *
 *  Created on: Sep 7, 2016
 *      Author: johnwcurry
 */

#include "LogFileMgr.h"
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <list>


const std::string LogFileMgr::LOGNAME = "log";
const std::string LogFileMgr::LOGEXT = ".txt";
const size_t LogFileMgr::MAX_FILE_STORAGE =1000000 * 30;
const __off_t LogFileMgr::MAX_FILE_LENGTH =1000000 * 2;
const double LogFileMgr::FILE_CHECK_TIME = 5.0;

LogFileMgr::LogFileMgr(std::string filePath)
{
	m_filePath = filePath;
	m_fp = NULL;
	m_tCheck  = std::chrono::system_clock::now();

}

LogFileMgr::~LogFileMgr()
{
	if (m_fp != NULL)
		fclose(m_fp);
}
FILE * LogFileMgr::GetFp()
{
	if (m_fp == NULL)
	{
		m_currentFileName = GetLogFileName();
		m_fp = fopen(m_currentFileName.c_str(),"a+");
		return m_fp;
	}
	std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - m_tCheck;
	if (elapsed_seconds.count() < FILE_CHECK_TIME )
		return m_fp;
	m_tCheck  = std::chrono::system_clock::now();
	fflush(m_fp);
	struct stat st;
	if(stat(m_currentFileName.c_str(), &st) == 0)
	{
		if (st.st_size > MAX_FILE_LENGTH)
		{
			fclose(m_fp);
			m_fp = NULL;
		}
	}
	else
	{
		fclose(m_fp);
		m_fp = NULL;
	}
	return (m_fp == NULL) ? GetFp() : m_fp;



}
std::string LogFileMgr::GetLogFileName()
{
	std::list<LogFileMgr::FileSize> fileSizes;
	size_t totalFileSize = GetFilesInDirectory (fileSizes);

	for (auto x = fileSizes.size(); x > 1 && totalFileSize > MAX_FILE_STORAGE ; x++ )
	{
		std::list<LogFileMgr::FileSize>::iterator it=fileSizes.begin();
		totalFileSize -= it->fileSize;
		unlink(it->fileName.c_str());
		fileSizes.pop_front();
	}
	if (fileSizes.size() > 0)
	{
		std::list<LogFileMgr::FileSize>::iterator it= fileSizes.end();
		it--;
		if (it->fileSize < MAX_FILE_LENGTH)
			return it->fileName;
	}
	return CreateNewFileName();
}

std::string LogFileMgr::CreateNewFileName()
{
	auto now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);
	struct tm *parts = std::localtime(&now_c);

	char szName[4096];
	memset(szName, 0, sizeof(szName));
	snprintf(szName, sizeof(szName), "%s%02d%02d%02d-%02d%02d%02d%s",LOGNAME.c_str(),
			parts->tm_year - 100, parts->tm_mon + 1, parts->tm_mday,
			parts->tm_hour, parts->tm_min, parts->tm_sec,
			LOGEXT.c_str());
	std::string fullPath = m_filePath + "/" + szName;

	char buf[80];
	strftime(buf, sizeof(buf), "%Y-%m-%d %X", parts);
	FILE *fp = fopen(fullPath.c_str(),"w");
	fprintf(fp,"%s file %s created\n",buf,fullPath.c_str());
	fclose( fp);
	return fullPath;
}
bool LogFileMgr::GetFileTime(std::string fileName, std::chrono::system_clock::time_point &tpOut)
{
	std::string fullName = "";
	std::string name = "";
	std::size_t found = fileName.find_last_of("/");
	if (found == std::string::npos)
	{
		return false;
	}
	fullName = fileName.substr(found + 1);
	found = fullName.find(LOGEXT);
	if (found == std::string::npos)
	{
		return false;
	}
	std::string partialName = fullName.substr(0,found);
	found = partialName.find_last_of(LOGNAME);
	if (found == std::string::npos)
	{
		return false;
	}
	std::string bodyText = partialName.substr(found + 1);
	struct tm tmTime;
	memset(&tmTime,0, sizeof(tmTime));
	int iCount = sscanf(bodyText.c_str(),"%02d%02d%02d-%02d%02d%02d",
			&tmTime.tm_year,&tmTime.tm_mon,&tmTime.tm_mday,&tmTime.tm_hour,&tmTime.tm_min,&tmTime.tm_sec);
	tmTime.tm_year += 100;
	tmTime.tm_mon -= 1;
	if (iCount != 6)
	{
		return false;
	}

	tpOut = std::chrono::system_clock::from_time_t(std::mktime(&tmTime));


	return true;

}
size_t  LogFileMgr::GetFilesInDirectory (std::list<LogFileMgr::FileSize> &fileSizes)
{
	fileSizes.clear();
	std::list<std::string>fileList;
	size_t totalFileSizes = 0;
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(m_filePath.c_str())) == NULL)
    {
        return 0;
    }

    while ((dirp = readdir(dp)) != NULL)
    {
    	std::string fileName = dirp->d_name;
    	if (fileName.find(LOGNAME) != std::string::npos && fileName.find(LOGEXT) != std::string::npos)
    		fileList.push_back(m_filePath + "/" + fileName);

    }
    closedir(dp);
    fileList.sort();
    for (std::list<std::string>::iterator it = fileList.begin(); it != fileList.end(); it++)
    {
    	LogFileMgr::FileSize fileSizeEntry;
    	fileSizeEntry.fileName =  *it;
    	struct stat st;
    	if(stat(fileSizeEntry.fileName.c_str(), &st) == 0)
    	{
    		fileSizeEntry.fileSize = st.st_size;
    		totalFileSizes += st.st_size;
    		fileSizes.push_back(fileSizeEntry);
    	}
    }
    return totalFileSizes;
}


