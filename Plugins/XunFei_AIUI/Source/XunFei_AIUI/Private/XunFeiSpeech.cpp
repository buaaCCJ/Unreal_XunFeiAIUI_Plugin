#include "XunFeiSpeech.h"
#include <synchapi.h>
#include "SpeechRecoginzer.h"
#include "IATTask.h"
#include  "windows.h"
#include "winnt.h"

FAIUITester AIUITester;
HANDLE  thread_hdl_;
unsigned int thread_id_;
static void iat_cb(char *data, unsigned long len, void *user_para);
FXunFeiIAT::FXunFeiIAT()
{
}

FXunFeiIAT::FXunFeiIAT(FString Log)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Log);
}

void FXunFeiIAT::SetStart()
{
	SetEvent(events[EVT_START]);
	return;
}

void FXunFeiIAT::SetStop()
{
	SetEvent(events[EVT_STOP]);
	return;
}
//得到翻译结果
void on_result(const char *result, char is_last)
{

}
//开始说话
void on_speech_begin()
{
	UE_LOG(LogTemp,Log,TEXT("Start Listening...\n"));
}
//说完
void on_speech_end(int reason)
{
	if (reason == END_REASON_VAD_DETECT)
		printf("\nSpeaking done \n");
	else
		printf("\nRecognizer error %d\n", reason);
}


/* the record call back */
void iat_cb(char *data, unsigned long len, void *user_para)
{
	
	AIUITester.writeThread->WriteAudio(data, len);
}

void FXunFeiIAT::beginMicLoop()
{
	int errcode;
	int i = 0;
	struct speech_rec iat;
	DWORD waitres;
	char isquit = 0;

	struct speech_rec_notifier recnotifier = {
		on_result,
		on_speech_begin,
		on_speech_end
	};
	/*for (i = 0; i < EVT_TOTAL; ++i) {
		events[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
	}*/
	errcode = FSpeechRecoginzer::GetInstance().sr_init(&iat, SR_MIC, DEFAULT_INPUT_DEVID, &recnotifier, iat_cb);
	/*if (errcode) {
		UE_LOG(LogTemp, Log, TEXT("speech recognizer init failed\n"));
		return;
	}*/

	while (1) {
		waitres = WaitForMultipleObjects(EVT_TOTAL, events, FALSE, INFINITE);
		switch (waitres) {
		case WAIT_FAILED:
		case WAIT_TIMEOUT:
			UE_LOG(LogTemp,Log,TEXT("Why it happened !?\n"));
			break;
		case WAIT_OBJECT_0 + EVT_START:
			errcode = FSpeechRecoginzer::GetInstance().sr_start_listening(&iat);
			if (errcode) {
				UE_LOG(LogTemp, Log, TEXT("start listen failed %d\n"), errcode);
				isquit = 1;
			}
			break;
		case WAIT_OBJECT_0 + EVT_STOP:
			errcode = FSpeechRecoginzer::GetInstance().sr_stop_listening(&iat);
			if (errcode) {
				UE_LOG(LogTemp, Log, TEXT("stop listening failed %d\n"), errcode);
				isquit = 1;
			}
			break;
		case WAIT_OBJECT_0 + EVT_QUIT:
			FSpeechRecoginzer::GetInstance().sr_stop_listening(&iat);
			isquit = 1;
			break;
		default:
			break;
		}
		if (isquit)
			break;
	}

//exit:

	for (i = 0; i < EVT_TOTAL; ++i) {
		if (events[i])
			CloseHandle(events[i]);
	}

	FSpeechRecoginzer::GetInstance().sr_uninit(&iat);
}

//单独线程来运行语音识别与交互
unsigned int __stdcall begin(void * paramptr)
{
	FXunFeiIAT::beginMicLoop();
	return 0;
}

void FXunFeiIAT::StartIATThread() const
{
	//创建audio event
	for (int i = 0; i < EVT_TOTAL; ++i) {
		events[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
	
	if (thread_hdl_ == NULL) {
		thread_hdl_ = (HANDLE)_beginthreadex(NULL, 0, begin, (LPVOID)nullptr, 0, &thread_id_);
	}
	
}
