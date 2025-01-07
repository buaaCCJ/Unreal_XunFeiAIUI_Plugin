#pragma once
/*
* ������д(iFly Auto Transform)�����ܹ�ʵʱ�ؽ�����ת���ɶ�Ӧ�����֡�
*/
#include "XunFei_AIUI.h"

#include "AIUITest.h"
#define FRAME_LEN	640
#define BUFFER_SIZE	4096


extern FAIUITester AIUITester;
extern HANDLE  thread_hdl_;
extern unsigned int thread_id_;

// ʶ��״̬����
enum {
	EVT_START = 0,
	EVT_STOP,
	EVT_QUIT,
	EVT_TOTAL
};
static HANDLE events[EVT_TOTAL] = { NULL,NULL,NULL };

class FXunFeiIAT
{
public:
	FXunFeiIAT();
	FXunFeiIAT(FString);
	void SetStart();
	void SetStop();
	void StartIATThread() const;

	static FXunFeiIAT & GetInstance()
	{
		static FXunFeiIAT xunfeiIAT(FString("static xunfeispeech be created !"));
		return xunfeiIAT;
	}

	static void beginMicLoop();
};
