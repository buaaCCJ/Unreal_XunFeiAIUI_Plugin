/*
 * FileUtil.cpp
 *
 *  Created on: 2017年2月21日
 *      Author: hj
 */

#include "FileUtil.h"
#include "XunFei_AIUI.h"
#include <stdio.h>
#include <sstream>

#include "aiui/AIUIType.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Interfaces/IPluginManager.h"

#if defined(_MSC_VER)
#include <windows.h>
#include <time.h>
#else
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <sys/time.h>
#endif

using namespace std;


const FString FileUtil::SURFFIX_PCM = ".pcm";

const FString FileUtil::SURFFIX_TXT = ".txt";

const FString FileUtil::SURFFIX_CFG = ".cfg";

const FString FileUtil::SURFFIX_LOG = ".log";

#if defined(_MSC_VER)

bool FileUtil::exist(const FString &path)
{
	WIN32_FIND_DATA result;
	HANDLE hFile;
	hFile =FindFirstFile(*path, &result);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;
	
	FindClose(hFile);
	return true;
}

bool FileUtil::isDirectory(const FString &path)
{
	WIN32_FIND_DATA result;
	HANDLE hFile;
	hFile =FindFirstFile(*path, &result);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;
	FindClose(hFile);
	if (result.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		return true;	
	return false;
}

bool FileUtil::makeDirIfNoExist(const FString &path)
{
	BOOL fret = CreateDirectory(*path, NULL);
	if (fret == FALSE && ERROR_ALREADY_EXISTS != GetLastError())
	{
		return false;
	}
	return true;
}

bool FileUtil:: delFile(const FString &filePath)
{
	//WIN32_FIND_DATA findData;
	//HANDLE hFinder;
	//FString findPath = filePath;
	//if (findPath.back() == '\\') {
	//	findPath.erase(findPath.end()-1);
	//}

	//hFinder = FindFirstFile(findPath.c_str(), &findData);
	//if (hFinder == INVALID_HANDLE_VALUE)
	//	return false;

	//if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
	//	if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) {
	//		FindClose(hFinder);
	//		return false;
	//	}

	//	FString newpath = findPath+"\\*";
	//	WIN32_FIND_DATA fda;
	//	BOOL fret;

	//	HANDLE hf = FindFirstFile(newpath.c_str(), &fda);
	//	if (hf == INVALID_HANDLE_VALUE) {
	//		FindClose(hFinder);
	//		return false;
	//	}

	//	FString fname;
	//	while (1) {
	//		fname = findPath + "\\" + fda.cFileName;
	//		if (fda.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
	//			if (strcmp(fda.cFileName, ".") != 0 && strcmp(fda.cFileName, "..") != 0 ) {
	//				// recurse the delete.
	//				delFile(fname.c_str());
	//			}
	//		} else {
	//			DeleteFile(fname.c_str());
	//		}
	//		fret = FindNextFile(hf, &fda);
	//		if (fret == FALSE)
	//			break;
	//	}

	//	FindClose(hf);
	//	RemoveDirectory(findPath.c_str());

	//} else {
	//	DeleteFile(findPath.c_str());
	//}

	//FindClose(hFinder);
	return true;
}


#else
bool FileUtil::exist(const FString &path)
{
	struct stat statInfo;

	if (stat(*path, &statInfo) == 0)
	{
		return true;
	}

	return false;
}

bool FileUtil::isDirectory(const FString & path)
{
	struct stat statInfo;

	if (stat(*path, &statInfo) == 0)
	{
		if (S_ISDIR(statInfo.st_mode))
		{
			return true;
		} else {
			return false;
		}
	}

	return false;
}

bool FileUtil::makeDirIfNoExist(const FString & path)
{
	FString::size_type sepPos = path.find_last_of("/");
	if (sepPos == FString::npos)
	{
		return false;
	}

	FString dirPath = path.substr(0, sepPos);
	if (exist(dirPath))
	{
		return true;
	}

	int ret = mkdir(dirPath.c_str(), S_IRWXU | S_IRGRP | S_IWGRP| S_IROTH | S_IWOTH);

	return 0 == ret ? true : false;
}

bool FileUtil::delFile(const FString & filePath)
{
	if (!exist(filePath))
	{
		return false;
	}

	if (!isDirectory(filePath))
	{
		if (remove(*filePath) == 0)
		{
			return true;
		} else {
			return false;
		}
	}

	DIR* dir;
	struct dirent* ent;

	if ((dir = opendir(*filePath)) == NULL)
	{
		return false;
	}

	while ((ent = readdir(dir)) != NULL)
	{
		if (strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0)
		{
			continue;
		} else if (4 == ent->d_type || 8 == ent->d_type) {
			FString fullSubPath;

			int len = filePath.size();
			if (filePath[len - 1] != '/')
			{
				fullSubPath.append(filePath).append("/").append(ent->d_name);
			} else {
				fullSubPath.append(filePath).append(ent->d_name);
			}

			delFile(fullSubPath);
		}
	}

	if (remove(*filePath) == 0)
	{
		return true;
	}

	return false;
}
#endif

int FileUtil::UTF8ToGBK(unsigned char * lpUTF8Str,unsigned char * lpGBKStr,int nGBKStrLen)
{
	wchar_t * lpUnicodeStr = NULL;
	int nRetLen = 0;

	if(!lpUTF8Str)
		return 0;

	nRetLen = MultiByteToWideChar(CP_UTF8, 0, (char *)lpUTF8Str, -1, NULL, NULL);
	lpUnicodeStr = new WCHAR[nRetLen + 1];
	nRetLen = MultiByteToWideChar(CP_UTF8, 0, (char *)lpUTF8Str, -1, lpUnicodeStr, nRetLen);
	if(!nRetLen)
		return 0;

	nRetLen = WideCharToMultiByte(CP_ACP, 0, lpUnicodeStr, -1, NULL, NULL, NULL, NULL);

	if(!lpGBKStr)
	{
		if(lpUnicodeStr)
			delete []lpUnicodeStr;
		return nRetLen;
	}

	nRetLen = WideCharToMultiByte(CP_ACP,0,lpUnicodeStr,-1,(char *)lpGBKStr,nRetLen,NULL,NULL);

	if(lpUnicodeStr)
		delete []lpUnicodeStr;

	return nRetLen;
}

std::string FileUtil::readFileAsFString(const FString & filePath)
{
	FString RetPath = FPaths::ProjectContentDir() / TEXT("Xunfei/") + filePath;
	//FString BaseDir = IPluginManager::Get().FindPlugin("XunFei_AIUI")->GetBaseDir();
	//FString RetPath = FPaths::Combine(*BaseDir, TEXT("XunFei/"), filePath);
	UE_LOG(LogTemp, Log, TEXT("XunFei_Plugin config path: %s"), *RetPath);
	if (!exist(RetPath))
	{
		return "";
	}
	fstream file(*RetPath, ios::in | ios::binary);
	stringstream sstream;

	sstream << file.rdbuf();
	file.close();

	string str(sstream.str());
	sstream.clear();

	return str;
}

Buffer* FileUtil::readFileAsBuffer(const FString &filePath)
{
	if (!exist(filePath))
	{
		return NULL;
	}

	fstream fs;
	fs.open(*filePath, ios::in | ios::binary);

	if (!fs.is_open())
	{
		return NULL;
	}

	fs.seekg(0, ios::end);
	size_t fileSize = fs.tellg();
	fs.seekg(0, ios::beg);

	Buffer* buffer = Buffer::alloc(fileSize);
	if (NULL == buffer)
	{
		fs.close();
		return NULL;
	}

	char* dataAddr = (char*) buffer->data();

	fs.read(dataAddr, fileSize);
	fs.close();

	return buffer;
}

FString FileUtil::getCurrentTime()
{
	time_t t = time(NULL);
	tm* tm = localtime(&t);

	char buffer[20];
	sprintf(buffer, "%04d-%02d-%02d-%02d-%02d-%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
			tm->tm_hour, tm->tm_min, tm->tm_sec);

	return FString(buffer);
}

FileUtil::DataFileHelper::DataFileHelper(const FString& fileDir): mFileDir(fileDir)
{
	mMutex = CreateMutex(NULL, FALSE, NULL);
}

FileUtil::DataFileHelper::~DataFileHelper()
{
	closeFile();
	CloseHandle(mMutex);
}

bool FileUtil::DataFileHelper::openReadFile(const FString& filePath, bool inCurrentDir)
{
	FString fullPath;

	if (inCurrentDir)
	{
		fullPath = mFileDir + filePath;
	} else {
		fullPath = filePath;
	}

	if (!exist(fullPath))
	{
		return false;
	}

	WaitForSingleObject(mMutex, INFINITE);

	if (mIn.is_open())
	{
		mIn.close();
	}

	mIn.open(*fullPath, ios::in | ios::binary);

	ReleaseMutex(mMutex);

	return true;
}

int FileUtil::DataFileHelper::read(char* buffer, int bufferLen)
{
	WaitForSingleObject(mMutex, INFINITE);

	if (!mIn.is_open())
	{
		ReleaseMutex(mMutex);
		return -1;
	}

	mIn.read(buffer, bufferLen);

	ReleaseMutex(mMutex);

	return mIn.gcount();
}

FString FileUtil::DataFileHelper::readLine()
{
	WaitForSingleObject(mMutex, INFINITE);

	if (!mIn.is_open())
	{
		ReleaseMutex(mMutex);
		return "";
	}

	std::string line;
	getline(mIn, line);

	ReleaseMutex(mMutex);
	return FString(line.c_str());
}

void FileUtil::DataFileHelper::rewindReadFile()
{
	WaitForSingleObject(mMutex, INFINITE);

	if (!mIn.is_open())
	{
		ReleaseMutex(mMutex);
		return;
	}

	if (mIn.eof())
	{
		mIn.clear();
	}

	mIn.seekg(0, ios::beg);

	ReleaseMutex(mMutex);
}

void FileUtil::DataFileHelper::closeReadFile()
{
	WaitForSingleObject(mMutex, INFINITE);

	if (mIn.is_open())
	{
		mIn.close();
	}
	ReleaseMutex(mMutex);
}

void FileUtil::DataFileHelper::createWriteFile(const FString& filename, const FString& suffix,
		bool append, bool inCurrentDir)
{
	FString fullPath;

	if (inCurrentDir)
	{
		makeDirIfNoExist(mFileDir);
		fullPath = mFileDir + filename + suffix;
	} else {
		fullPath = filename + suffix;
	}

	WaitForSingleObject(mMutex, INFINITE);

	if (append)
	{
		mOut.open(*fullPath, ios::out | ios::binary | ios::app);
	} else {
		mOut.open(*fullPath, ios::out | ios::binary);
	}
	ReleaseMutex(mMutex);
}

void FileUtil::DataFileHelper::createWriteFile(const FString& suffix, bool append)
{
	FString filename = getCurrentTime();
	createWriteFile(filename, suffix, append, true);
}

bool FileUtil::DataFileHelper::write(const char* data, int dataLen, int offset, int writeLen)
{
	if (offset + writeLen > dataLen)
	{
		return false;
	}

	WaitForSingleObject(mMutex, INFINITE);

	if (!mOut.is_open())
	{
		ReleaseMutex(mMutex);
		return false;
	}

	const char* start = data + offset;
	mOut.write(start, writeLen);

	ReleaseMutex(mMutex);

	return true;
}

void FileUtil::DataFileHelper::closeWriteFile()
{
	WaitForSingleObject(mMutex, INFINITE);

	if (mOut.is_open())
	{
		mOut.flush();
		mOut.close();
	}

	ReleaseMutex(mMutex);
}

void FileUtil::DataFileHelper::closeFile()
{
	closeReadFile();
	closeWriteFile();
}


