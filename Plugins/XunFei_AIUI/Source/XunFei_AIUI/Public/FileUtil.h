#pragma once
/*
 * FileUtil.h
 *
 *  Created on: 2017��2��21��
 *      Author: hj
 */

#ifndef FILEUTIL_H_
#define FILEUTIL_H_

#include <iostream>
#include <fstream>

#include <aiui/AIUIType.h>
#include  "windows.h"
using namespace std;
using namespace aiui;

#include <windows.h>
#include "XunFei_AIUI.h"

/**
 * �ļ������ࡣ
 *
 * @author <a href="http://www.xfyun.cn">Ѷ�ɿ���ƽ̨</a>
 */
class FileUtil
{
public:
	static const FString SURFFIX_PCM;

	static const FString SURFFIX_TXT;

	static const FString SURFFIX_CFG;

	static const FString SURFFIX_LOG;

	static bool exist(const FString &path);

	static bool isDirectory(const FString &path);

	/**
	 * ��Ŀ¼·��������ʱ����Ŀ¼��
	 *
	 * @param path ·������"/"��β
	 */
	static bool makeDirIfNoExist(const FString &path);

	static bool delFile(const FString &filePath);

	static std::string readFileAsFString(const FString &filePath);

	static FString getCurrentTime();

	/**
	 * �·���Buffer����ȡ�ļ���Buffer��Bufferʹ����֮����Ҫʹ�����ֶ��ͷš�
	 *
	 * @return Buffer��ַ���ļ���ȡ���󷵻�NULL
	 */
	static Buffer* readFileAsBuffer(const FString &filePath);

	static int UTF8ToGBK(unsigned char * lpUTF8Str, unsigned char * lpGBKStr, int nGBKStrLen);

	class DataFileHelper
	{
	private:
		HANDLE mMutex;

		const FString mFileDir;

		fstream mIn;

		fstream mOut;

	public:
		DataFileHelper(const FString& fileDir);

		~DataFileHelper();

		bool openReadFile(const FString& filePath, bool inCurrentDir = true);

		int read(char* buffer, int bufferLen);

		FString readLine();

		void rewindReadFile();

		void closeReadFile();

		void createWriteFile(const FString& suffix, bool append);

		void createWriteFile(const FString& filename, const FString& suffix, bool append, bool inCurrentDir = true);

		bool write(const char* data, int dataLen, int offset, int writeLen);

		void closeWriteFile();

		void closeFile();
	};

};



#endif /* FILEUTIL_H_ */
