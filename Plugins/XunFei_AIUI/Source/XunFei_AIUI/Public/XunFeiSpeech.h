#pragma once
/*
* 语音听写(iFly Auto Transform)技术能够实时地将语音转换成对应的文字。
*/
#include "XunFei_AIUI.h"

#include "AIUITest.h"
#define FRAME_LEN	640
#define BUFFER_SIZE	4096


extern FAIUITester AIUITester;
extern HANDLE  thread_hdl_;
extern unsigned int thread_id_;

// 识别状态类型
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
