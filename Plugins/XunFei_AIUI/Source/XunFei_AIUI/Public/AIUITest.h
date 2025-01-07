#pragma once
#ifndef AIUIAGENTTEST_H_
#define AIUIAGENTTEST_H_

#include "XunFei_AIUI.h"

#include <aiui/AIUI.h>
#include <string>
#include <iostream>

#include "FileUtil.h"


#ifdef _MSC_VER
#include <windows.h>
#include <process.h>
#define TEST_ROOT_DIR ".\\AIUI\\"
#define CFG_FILE_PATH TEST_ROOT_DIR##"cfg\\aiui.cfg"
#define TEST_AUDIO_PATH TEST_ROOT_DIR##"audio\\test.pcm"
//#define TEST_AUDIO_PATH TEST_ROOT_DIR##"audio\\1.m4a"
#define LOG_DIR TEST_ROOT_DIR##"log"
#define SYNC_PARAM_PATH TEST_ROOT_DIR##"test\\syncstat.json"
#else
#include <unistd.h>
#define TEST_ROOT_DIR "./AIUI"
#define CFG_FILE_PATH TEST_ROOT_DIR##"/cfg/aiui.cfg"
#define TEST_AUDIO_PATH TEST_ROOT_DIR##"/test/audio/test.pcm"
#define LOG_DIR TEST_ROOT_DIR##"/log"
#endif
DECLARE_DELEGATE_ThreeParams(FIATDelegateSignature, FString,FString,int);

using namespace aiui;
#include  "windows.h"
using namespace std;

class FWriteAudioThread
{
private:
	IAIUIAgent* mAgent;

	FString mAudioPath;

	bool mRepeat;

	bool mRun;

	FileUtil::DataFileHelper* mFileHelper;

	HANDLE  thread_hdl_;
	unsigned int  thread_id_;


private:
	bool threadLoop();

	static unsigned int __stdcall WriteProc(void * paramptr);


public:
	FWriteAudioThread(IAIUIAgent* agent);
	void WriteAudio(char audioBuffer[], int len);
	FWriteAudioThread(IAIUIAgent* agent, const FString& audioPath, bool repeat);

	~FWriteAudioThread();

	void stopRun();

	bool run();
};

class FTestListener : public IAIUIListener
{
private:
	FileUtil::DataFileHelper* mTtsFileHelper;

public:

	//GENERATED_BODY()
	FTestListener();

	~FTestListener();

	void onEvent(const IAIUIEvent& event) const;
	FIATDelegateSignature OnIATFinished;
};

class FAIUITester
{
public:
	IAIUIAgent* agent;

	/* AIUI�¼��ص������� */
	FTestListener listener;
	string encode(const unsigned char* bytes_to_encode, unsigned int in_len);

public:
	FAIUITester();

	~FAIUITester();
private:

	void showIntroduction(bool detail);

	void start();

	void stop();

	void reset();


	void stopWriteThread();

	/* ��������ӿ� */
	void write(bool repeat);

	/* �ı�����ӿ� */
	void writeText();

	/* ��̬�ϴ���Դ���� */
	void syncSchema();

	void querySyncStatus();

	/* ��ʼtts */
	void startTts();

	/* ��ͣtts */
	void pauseTts();

	/* �����ϴε�tts */
	void resumeTts();

	/* ȡ������tts */
	void cancelTts();

public:
	void initWrite();
	FWriteAudioThread * writeThread;
	/*����AIUI����*/
	void createAgent();

	void setRootDir(FString Path);
	/* ���ѽӿ� */
	void wakeup();
	/*����*/
	void destory();
	void readCmd();
	void test();
public:
	FString RootPath  ;
	FString ConfigPath = TEXT("cfg/aiui.cfg");
};
//static FAIUITester * AIUITester = new FAIUITester();

#endif /* AIUIAGENTTEST_H_ */

