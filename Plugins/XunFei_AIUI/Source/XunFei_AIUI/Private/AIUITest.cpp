#include "AIUITest.h"
#include "jsoncpp/json/json.h"
#include "Misc/Paths.h"
#include "Windows/WindowsPlatformTime.h"
//for ivw
/*
#include "msp_cmn.h"

#ifdef _WIN64
#pragma comment(lib,"msc_x64.lib") //x64
#else
#pragma comment(lib,"msc.lib") //x86
#endif
*/
using namespace VA;
static const string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
void FWriteAudioThread::WriteAudio(char audioBuffer[], int len) {

	if (len > 0)
	{
		Buffer* buffer = Buffer::alloc(len);
		memcpy(buffer->data(), audioBuffer, len);

		const wchar_t * para = L"data_type=audio,sample_rate=16000,msc.lng=117.14430236816406,msc.lat=31.831058502197266,"
			L"rec_user_data={\"sceneInfo\":{},\"recHotWords\":\"返回|主界面|通知栏|图片|百度|本地|主页|主目录|根目录|下载|库|退出|主题|设置|网络硬盘|新建|搜索|刷新|视图|窗口\"}";
		unsigned long len = ::WideCharToMultiByte(CP_UTF8, NULL, para, -1, NULL, NULL, NULL, NULL);
		char *mbcbuffer = new char[len];
		::WideCharToMultiByte(CP_UTF8, NULL, para, -1, mbcbuffer, len, NULL, NULL);
		IAIUIMessage * writeMsg = IAIUIMessage::create(AIUIConstant::CMD_WRITE,
			0, 0, (const char*)mbcbuffer, buffer);
		delete[] mbcbuffer;

		if (NULL != mAgent)
		{
			mAgent->sendMessage(writeMsg);

		}
#ifdef _MSC_VER
		Sleep(40);
#else
		usleep(40 * 1000);
#endif
		writeMsg->destroy();
	}
}
/* 循环写入测试音频，每次写1280B */
bool FWriteAudioThread::threadLoop()
{
	char audio[1280];
	int len1 = mFileHelper->read(audio, 1280);

	if (len1 > 0)
	{
		Buffer* buffer = Buffer::alloc(len1);
		memcpy(buffer->data(), audio, len1);

		const wchar_t * para = L"data_type=audio,sample_rate=16000,msc.lng=117.14430236816406,msc.lat=31.831058502197266,"
			L"rec_user_data={\"sceneInfo\":{},\"recHotWords\":\"返回|主界面|通知栏|图片|百度|本地|主页|主目录|根目录|下载|库|退出|主题|设置|网络硬盘|新建|搜索|刷新|视图|窗口\"}";
		unsigned long len = ::WideCharToMultiByte(CP_UTF8, NULL, para, -1, NULL, NULL, NULL, NULL);
		char *mbcbuffer = new char[len];
		::WideCharToMultiByte(CP_UTF8, NULL, para, -1, mbcbuffer, len, NULL, NULL);
		IAIUIMessage * writeMsg = IAIUIMessage::create(AIUIConstant::CMD_WRITE,
			0, 0, (const char*)mbcbuffer, buffer);
		delete[] mbcbuffer;

		if (NULL != mAgent)
		{
			mAgent->sendMessage(writeMsg);
		}
#ifdef _MSC_VER
		Sleep(40);
#else
		usleep(40 * 1000);
#endif
		writeMsg->destroy();
	}
	else {
		if (mRepeat)
		{
			mFileHelper->rewindReadFile();
		}
		else {
			IAIUIMessage * stopWrite = IAIUIMessage::create(AIUIConstant::CMD_STOP_WRITE,
				0, 0, "data_type=audio,sample_rate=16000");

			if (NULL != mAgent)
			{
				mAgent->sendMessage(stopWrite);
			}
			stopWrite->destroy();

			mFileHelper->closeReadFile();
			mRun = false;
		}
	}

	return mRun;
}


unsigned int __stdcall FWriteAudioThread::WriteProc(void * paramptr)
{
	FWriteAudioThread * self = (FWriteAudioThread *)paramptr;

	while (1) {
		if (!self->threadLoop())
			break;
	}

	return 0;
}

FWriteAudioThread::FWriteAudioThread(IAIUIAgent* agent, const FString& audioPath, bool repeat) :
	mAgent(agent), mAudioPath(audioPath), mRepeat(repeat), mRun(true), mFileHelper(NULL)
	, thread_hdl_(NULL)
{
	mFileHelper = new FileUtil::DataFileHelper("");
	mFileHelper->openReadFile("xxxx.pcm", false);
}

FWriteAudioThread::FWriteAudioThread(IAIUIAgent* agent) :
	mAgent(agent) {

}

FWriteAudioThread::~FWriteAudioThread()
{
	if (NULL != mFileHelper)
	{
		delete mFileHelper;
		mFileHelper = NULL;
	}
}

void FWriteAudioThread::stopRun()
{
	mRun = false;
	WaitForSingleObject(thread_hdl_, INFINITE);
	thread_hdl_ = NULL;
}

bool FWriteAudioThread::run()
{
	if (thread_hdl_ == NULL) {
		thread_hdl_ = (HANDLE)_beginthreadex(NULL, 0, WriteProc, (LPVOID)this, 0, &thread_id_);
		return true;
	}

	return false;
}

string mSyncSid;

/*
	AIUI 事件回调接口
*/
void FTestListener::onEvent(const IAIUIEvent& event) const
{

	switch (event.getEventType()) {
		/* 状态回调        */
	case AIUIConstant::EVENT_STATE:
	{
		switch (event.getArg1()) {
		case AIUIConstant::STATE_IDLE:
		{
			UE_LOG(LogTemp, Log, TEXT("EVENT_STATE::IDLE"));
		} break;

		case AIUIConstant::STATE_READY:
		{
			UE_LOG(LogTemp, Log, TEXT("EVENT_STATE::READY"));
		} break;

		case AIUIConstant::STATE_WORKING:
		{
			UE_LOG(LogTemp, Log, TEXT("EVENT_STATE::WORKING"));
		} break;
		}
	} break;

	/* 唤醒事件回调        ， 唤醒成功会调用此接口，并且看到唤醒信息               */

	case AIUIConstant::EVENT_WAKEUP:
	{
		UE_LOG(LogTemp, Log, TEXT("EVENT_WAKEUP::%s"), event.getInfo());
	} break;

	/* 休眠事件回调            */
	case AIUIConstant::EVENT_SLEEP:
	{
		UE_LOG(LogTemp, Log, TEXT("EVENT_SLEEP:arg1=::%s"), event.getArg1());
	} break;

	/* VAD事件回调，检测到前后端点  */
	case AIUIConstant::EVENT_VAD:
	{
		switch (event.getArg1()) {
		case AIUIConstant::VAD_BOS:
		{
			UE_LOG(LogTemp, Log, TEXT("EVENT_VAD:BOS"));

		} break;

		case AIUIConstant::VAD_EOS:
		{
			UE_LOG(LogTemp, Log, TEXT("EVENT_VAD:EOS"));
		} break;

		case AIUIConstant::VAD_VOL:
		{
			//						cout << "EVENT_VAD:" << "VOL" << endl;
		} break;
		}
	} break;

	/*
		最重要的结果事件回调，收到文本和语音语义都会返回此事件，里面有结果信息
	*/
	case AIUIConstant::EVENT_RESULT:
	{
		Json::Value bizParamJson;
		Json::Reader reader;
		Json::Reader IATreader;
		Json::Value IATParmJson;
		if (!reader.parse(event.getInfo(), bizParamJson, false)) {
			UE_LOG(LogTemp, Log, TEXT("parse error!:%s"), event.getInfo());
			break;
		}
		Json::Value data = (bizParamJson["data"])[0];
		Json::Value params = data["params"];
		Json::Value content = (data["content"])[0];
		string sub = params["sub"].asString();
		UE_LOG(LogTemp, Log, TEXT("EVENT_RESULT:%s"), sub.c_str());

		if (sub == "nlp")
		{
			Json::Value empty;
			Json::Value contentId = content.get("cnt_id", empty);

			if (contentId.empty())
			{
				UE_LOG(LogTemp, Log, TEXT("Content Id is empty"));
				break;
			}

			string cnt_id = contentId.asString();
			int dataLen = 0;
			const char* buffer = event.getData()->getBinary(cnt_id.c_str(), &dataLen);

			string resultStr;

			if (NULL != buffer)
			{
				resultStr = string(buffer, dataLen);

				/*下面的操作主要是让返回的UTF-8编码的结果转化为GBK格式，以在dos窗口正常显示*/

				//int nRetLen = FileUtil::UTF8ToGBK((unsigned char *)resultStr.c_str(), NULL, NULL);

				//char* lpGBKStr = new char[nRetLen + 1];
				//nRetLen = FileUtil::UTF8ToGBK((unsigned char *)resultStr.c_str(), (unsigned char *)lpGBKStr, nRetLen);

				if (!reader.parse(resultStr, IATParmJson, false)) {
					UE_LOG(LogTemp, Log, TEXT(" parse IAT result error!"));

					break;
				}
				string serviceType = IATParmJson["intent"]["service"].asString();
				string ret;
				string answer;
				/*if(serviceType=="Turing")
				{
					ret = IATParmJson["intent"]["text"].asString();
				}else if(serviceType=="iFlytekQA")
				{
					ret = IATParmJson["intent"]["answer"]["text"].asString();
				}
				else
				{
					ret = IATParmJson["intent"]["text"].asString();
				}*/
				Json::Value empty1;
				Json::Value rc = IATParmJson["intent"].get("rc", empty1);
				ret = IATParmJson["intent"]["text"].asString();

				if (rc.empty())
				{
					break;
				}
				if (rc == 4)
				{
					//nothing return  answer is null , play default dialogue
					OnIATFinished.ExecuteIfBound(UTF8_TO_TCHAR(ret.c_str()), FString(),-1);
					break;
				}
				//
				answer = IATParmJson["intent"]["answer"]["text"].asString();
				int answerId = IATParmJson["intent"]["data"]["result"][0]["answerId"].asInt();
				FString Result(UTF8_TO_TCHAR(ret.c_str()));
				FString Answer(UTF8_TO_TCHAR(answer.c_str()));
				OnIATFinished.ExecuteIfBound(Result, Answer, answerId);
		
			}
		}
		else if (sub == "tts") {
			cout << "tts result" << endl;
			string eventInfo = event.getInfo();
			//cout << eventInfo << endl;
			int nRetLen = FileUtil::UTF8ToGBK((unsigned char *)eventInfo.c_str(), NULL, NULL);

			char* lpGBKStr = new char[nRetLen + 1];
			FileUtil::UTF8ToGBK((unsigned char *)eventInfo.c_str(), (unsigned char *)lpGBKStr, nRetLen);
			UE_LOG(LogTemp, Log, TEXT("tts:lpGBKStr: %d"), lpGBKStr);

			delete[] lpGBKStr;

			string cnt_id = content["cnt_id"].asString();

			int dataLen = 0;
			const char* data1 = event.getData()->getBinary(cnt_id.c_str(), &dataLen);

			UE_LOG(LogTemp, Log, TEXT("data_len: %d"), dataLen);

			int dts = content["dts"].asInt();
			cout << "dts=" << dts << endl;

			string errorInfo = content["error"].asString();
			UE_LOG(LogTemp, Log, TEXT("error info is: %s"), errorInfo.c_str());

			if (2 == dts && "AIUI DATA NULL" == errorInfo)
			{
				/*
					满足这两个条件的是空结果，直接丢弃
				*/
			}
			else if (3 == dts)
			{
				cout << "tts result receive dts = 3 result" << endl;
				mTtsFileHelper->createWriteFile("tts", ".pcm", false);
				mTtsFileHelper->write((const char*)data1, dataLen, 0, dataLen);
				mTtsFileHelper->closeFile();
			}
			else
			{
				if (0 == dts)
				{
					mTtsFileHelper->createWriteFile("tts", ".pcm", false);
				}

				mTtsFileHelper->write((const char*)data1, dataLen, 0, dataLen);

				if (2 == dts)
				{
					mTtsFileHelper->closeFile();
				}
			}
		}

	} break;

	case AIUIConstant::EVENT_CMD_RETURN:
	{
		if (AIUIConstant::CMD_SYNC == event.getArg1())
		{
			int retcode = event.getArg2();
			int dtype = event.getData()->getInt("sync_dtype", -1);

			switch (dtype)
			{
			case AIUIConstant::SYNC_DATA_STATUS:
				break;

			case AIUIConstant::SYNC_DATA_ACCOUNT:
				break;

			case AIUIConstant::SYNC_DATA_SCHEMA:
			{
				string sid = event.getData()->getString("sid", "");
				string tag = event.getData()->getString("tag", "");

				mSyncSid = sid;

				if (AIUIConstant::SUCCESS == retcode)
				{
					cout << "sync schema success." << endl;
				}
				else {
					cout << "sync schema error=" << retcode << endl;
				}

				cout << "sid=" << sid << endl;
				cout << "tag=" << tag << endl;
			} break;

			case AIUIConstant::SYNC_DATA_SPEAKABLE:
				break;

			case AIUIConstant::SYNC_DATA_QUERY:
			{
				if (AIUIConstant::SUCCESS == retcode)
				{
					cout << "sync status success" << endl;
				}
				else {
					cout << "sync status error=" << retcode << endl;
				}
			} break;
			}
		}
		else if (AIUIConstant::CMD_QUERY_SYNC_STATUS == event.getArg1()) {
			int syncType = event.getData()->getInt("sync_dtype", -1);
			if (AIUIConstant::SYNC_DATA_QUERY == syncType)
			{
				string result = event.getData()->getString("result", "");
				cout << "result:" << result << endl;

				if (0 == event.getArg2())
				{
					cout << "sync status:success." << endl;
				}
				else {
					cout << "sync status error:" << event.getArg2() << endl;
				}
			}
		}
	} break;
	case AIUIConstant::EVENT_ERROR:
	{
		UE_LOG(LogTemp, Log, TEXT("IAT__EVENT_ERROR:%d"), event.getArg1());
		int nRetLen = FileUtil::UTF8ToGBK((unsigned char *)event.getInfo(), NULL, NULL);

		char* lpGBKStr = new char[nRetLen + 1];
		nRetLen = FileUtil::UTF8ToGBK((unsigned char *)event.getInfo(), (unsigned char *)lpGBKStr, nRetLen);

		UE_LOG(LogTemp, Log, TEXT("IAT__error info:%s"), lpGBKStr);
	} break;
	}
}

FAIUITester::FAIUITester() : agent(NULL), writeThread(NULL)
{

}

FAIUITester::~FAIUITester()
{
	if (agent) {
		agent->destroy();
		agent = NULL;
	}
}

FTestListener::FTestListener()
{
	mTtsFileHelper = new FileUtil::DataFileHelper(".\\ttsAudio\\");
}

FTestListener::~FTestListener()
{
	if (mTtsFileHelper != NULL)
	{
		delete mTtsFileHelper;
		mTtsFileHelper = NULL;
	}
}


/*
	创建AIUI代理，传入用户设置的参数，通过生成的AIUI代理对象agent给AIUI SDK发命令
*/
void FAIUITester::createAgent()
{
	string appid = "5d08b151";

	Json::Value paramJson;
	Json::Value appidJson;

	appidJson["appid"] = appid;

	std::string fileParam = FileUtil::readFileAsFString(ConfigPath);

	Json::Reader reader;
	//this may be bug

	if (reader.parse(fileParam, paramJson, false))
	{
		paramJson["login"] = appidJson;
		paramJson["vad"]["res_path"] = string(TCHAR_TO_UTF8(*(RootPath + TEXT("assets/vad/meta_vad_16k.jet"))));

		/* 如果配置了使用单路ivw唤醒，那么需要用户自己将支持单路唤醒的msc.dll放在aiui.dll同目录下，并且
			配置好唤醒资源的路径；
			如果设置了单路唤醒，在createAgent()成功之后，就不需要调用wakeUp()接口通过外部发送命令的方式进行唤醒，可以通过写入唤醒音频的方式；
			默认没有设置ivw唤醒。
		*/
		string wakeup_mode = paramJson["speech"]["wakeup_mode"].asString();
		if (wakeup_mode == "ivw")
		{
			/*
				使用单路唤醒时，首先需要调动msc库里面的MSPLogin()
			*/
			//string lgiparams = "appid=5d08b151,engine_start=ivw";
			//MSPLogin(NULL, NULL, lgiparams.c_str());
			string ivw_res_path = paramJson["ivw"]["res_path"].asString();
			if (!ivw_res_path.empty())
			{
				ivw_res_path = "fo|" + ivw_res_path;
				paramJson["ivw"]["res_path"] = ivw_res_path;

			}

			string ivw_lib_path;
#ifdef _WIN64
			ivw_lib_path = "msc_x64.dll";
#else
			ivw_lib_path = "msc.dll";
#endif

			paramJson["ivw"]["msc_lib_path"] = ivw_lib_path;
		}

		Json::FastWriter writer;
		string paramStr = writer.write(paramJson);
		agent = IAIUIAgent::createAgent(paramStr.c_str(), &listener);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("aiui.cfg has something wrong! Crash!"));
	}
}

void FAIUITester::setRootDir(FString Path)
{
	FString temp = FPaths::ProjectContentDir() / TEXT("Xunfei");
	TArray<FString> lines;
	//bug: just for small files 
	temp.ParseIntoArray(lines, TEXT("/"), true);
	FString newPath = lines[0];
	for (int i = 1; i < lines.Num(); i++)
	{
		newPath.Append("\\");
		newPath.Append(lines[i]);
	}
	newPath.Append("\\");
	UE_LOG(LogTemp, Log, TEXT("newPath: %s"), TCHAR_TO_ANSI(*newPath));

	RootPath = newPath;
	AIUISetting::setAIUIDir(TCHAR_TO_UTF8(*newPath));
}

/*
	外部唤醒接口，通过发送CMD_WAKEUP命令使AIUI SDK转入唤醒状态，
	唤醒状态下就可以写入文本或语义，得到结果
*/
void FAIUITester::wakeup()
{
	if (NULL != agent)
	{
		IAIUIMessage * wakeupMsg = IAIUIMessage::create(AIUIConstant::CMD_WAKEUP);
		agent->sendMessage(wakeupMsg);
#ifdef BUG_TEST
		delete wakeupMsg;///  BUG TEST.
#else
		wakeupMsg->destroy();
#endif
	}
}

/*启动AIUI SDK, 该接口与stop()接口对应，只有在调用了stop()接口停止了AIUI SDK的情况下，
需要再次启动AIUI SDK时调用该接口
*/
void FAIUITester::start()
{
	if (NULL != agent)
	{
		IAIUIMessage * startMsg = IAIUIMessage::create(AIUIConstant::CMD_START);
		agent->sendMessage(startMsg);
		startMsg->destroy();
	}
}


/*停止AIUI      SDK，停止之后处于空闲状态  */
void FAIUITester::stop()
{
	if (NULL != agent)
	{
		IAIUIMessage *stopMsg = IAIUIMessage::create(AIUIConstant::CMD_STOP);
		agent->sendMessage(stopMsg);
		stopMsg->destroy();
	}
}


/*
	写入音频接口
	开启新的线程写入音频，音频从文件读取
*/
void FAIUITester::write(bool repeat)
{
	if (agent == NULL)
		return;

	if (writeThread != NULL) {
		writeThread->stopRun();
		delete writeThread;
		writeThread = NULL;
	}
	writeThread = new FWriteAudioThread(agent, TEST_AUDIO_PATH, repeat);
	writeThread->run();
}
void FAIUITester::initWrite() {
	if (agent == NULL)
		return;
	writeThread = new FWriteAudioThread(agent);
	//write(false);
}

/*主动销毁写入音频的线程*/
void FAIUITester::stopWriteThread()
{
	if (writeThread) {
		writeThread->stopRun();
		delete writeThread;
		writeThread = NULL;
	}
}

/*
	状态重置接口，发送CMD_RESET命令给AIUI SDK之后，SDK原本的唤醒状态会被重置为非唤醒状态，
	需要重新唤醒才能再次写入音频或文本
*/
void FAIUITester::reset()
{
	if (NULL != agent)
	{
		IAIUIMessage * resetMsg = IAIUIMessage::create(AIUIConstant::CMD_RESET);
		agent->sendMessage(resetMsg);
		resetMsg->destroy();
	}
}

/*
	文本语义接口
*/
void FAIUITester::writeText()
{
	if (NULL != agent)
	{
		const wchar_t * text = L"查附近的美食。";

		/*传入的文本需要转化为utf-8编码*/
		unsigned long len = ::WideCharToMultiByte(CP_UTF8, NULL, text, -1, NULL, NULL, NULL, NULL) - 1;
		char *mbcbuffer = new char[len];
		::WideCharToMultiByte(CP_UTF8, NULL, text, -1, mbcbuffer, len, NULL, NULL);

		Buffer* buffer = Buffer::alloc(len);
		memcpy(buffer->data(), mbcbuffer, len);

		IAIUIMessage * writeMsg = IAIUIMessage::create(AIUIConstant::CMD_WRITE,
			0, 0, "data_type=text", buffer);

		delete[] mbcbuffer;
		agent->sendMessage(writeMsg);
		writeMsg->destroy();
	}
}

/*
	查询数据同步状态
*/
void FAIUITester::querySyncStatus()
{
	if (NULL != agent)
	{
		Json::Value paramJson;

		/* 参数包含需要对应同步上传操作的sid */
		cout << "query sid is " << mSyncSid << endl;
		paramJson["sid"] = mSyncSid;

		Json::FastWriter writer;
		string paramStr = writer.write(paramJson);

		IAIUIMessage * querySyncMsg = IAIUIMessage::create(AIUIConstant::CMD_QUERY_SYNC_STATUS,
			AIUIConstant::SYNC_DATA_SCHEMA, 0, paramStr.c_str(), NULL);

		agent->sendMessage(querySyncMsg);

		querySyncMsg->destroy();
	}
}

string FAIUITester::encode(const unsigned char* bytes_to_encode, unsigned int in_len)
{
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i < 4); i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while ((i++ < 3))
			ret += '=';

	}

	return ret;
}

void FAIUITester::syncSchema()
{
	if (NULL != agent)
	{
		Json::Value paramsJson;
		paramsJson["tag"] = "tag_abc";

		Json::Value dataParamJson;
		dataParamJson["id_name"] = "uid";
		dataParamJson["res_name"] = "IFLYTEK.telephone_contact";

		const wchar_t * text = L"{\"name\":\"程序员\", \"alias\":\"开发\"}\n{\"name\":\"设计师\", \"alias\":\"设计\"}\n{\"name\":\"产品经理\", \"alias\":\"产品\"}";
		unsigned long len = ::WideCharToMultiByte(CP_UTF8, NULL, text, -1, NULL, NULL, NULL, NULL) - 1;
		char *mbcbuffer = new char[len];
		::WideCharToMultiByte(CP_UTF8, NULL, text, -1, mbcbuffer, len, NULL, NULL);

		string rawDataBase64 = encode((const unsigned char*)mbcbuffer, (unsigned int)len);

		delete[] mbcbuffer;

		Json::Value dataJson;
		dataJson["param"] = dataParamJson;
		dataJson["data"] = rawDataBase64;

		Json::FastWriter writer;

		string dataStr = writer.write(dataJson);
		string params = writer.write(paramsJson);

		Buffer* dataBuffer = Buffer::alloc(dataStr.length());
		dataStr.copy((char*)dataBuffer->data(), dataStr.length());

		IAIUIMessage * syncMsg = IAIUIMessage::create(AIUIConstant::CMD_SYNC,
			AIUIConstant::SYNC_DATA_SCHEMA, 0, params.c_str(), dataBuffer);

		agent->sendMessage(syncMsg);

		syncMsg->destroy();
	}
}


void FAIUITester::startTts()
{
	if (NULL != agent)
	{
		const wchar_t * text = L"NIHAO";
		//const wchar_t * text = TEXT("科大讯飞");
		/*传入的文本需要转化为utf-8编码*/
		unsigned long len = ::WideCharToMultiByte(CP_UTF8, NULL, text, -1, NULL, NULL, NULL, NULL) - 1;
		char *mbcbuffer = new char[len];
		::WideCharToMultiByte(CP_UTF8, NULL, text, -1, mbcbuffer, len, NULL, NULL);

		Buffer* textData = Buffer::alloc(len);
		memcpy(textData->data(), mbcbuffer, len);

		if (mbcbuffer != NULL)
		{
			delete[] mbcbuffer;
			mbcbuffer = NULL;
		}

		/*
		 arg1取值说明：
			START	开始合成	合成发音人，语速语调等
			CANCEL	取消合成	无
			PAUSE	暂停播放	无
			RESUME	恢复播放	无
		*/

		/*
		合成参数示例：
		String params = "vcn=xiaoyan,speed=50,pitch=50,volume=50"

		参数字段说明：
			vcn	发音人，如xiaoyan
			speed	速度，0-100
			pitch	语调，0-100
			volume	音量，0-100
			ent	引擎，默认aisound，如果需要较好的效果，可设置成xtts
		*/

		string paramStr = "vcn=xiaoyan";
		paramStr += ",speed=50";
		paramStr += ",pitch=50";
		paramStr += ",volume=50";

		cout << "params is " << paramStr << endl;

		IAIUIMessage * ttsMsg = IAIUIMessage::create(AIUIConstant::CMD_TTS,
			AIUIConstant::START, 0, paramStr.c_str(), textData);

		agent->sendMessage(ttsMsg);

		ttsMsg->destroy();
	}
}

void FAIUITester::pauseTts()
{
	if (NULL != agent)
	{
		IAIUIMessage * ttsMsg = IAIUIMessage::create(AIUIConstant::CMD_TTS,
			AIUIConstant::PAUSE, 0, "", NULL);

		agent->sendMessage(ttsMsg);

		ttsMsg->destroy();
	}
}

void FAIUITester::resumeTts()
{
	if (NULL != agent)
	{
		IAIUIMessage * ttsMsg = IAIUIMessage::create(AIUIConstant::CMD_TTS,
			AIUIConstant::RESUME, 0, "", NULL);

		agent->sendMessage(ttsMsg);

		ttsMsg->destroy();
	}
}

void FAIUITester::cancelTts()
{
	if (NULL != agent)
	{
		IAIUIMessage * ttsMsg = IAIUIMessage::create(AIUIConstant::CMD_TTS,
			AIUIConstant::CANCEL, 0, "", NULL);

		agent->sendMessage(ttsMsg);

		ttsMsg->destroy();
	}
}

void FAIUITester::destory()
{
	stopWriteThread();

	if (NULL != agent)
	{
		agent->destroy();
		agent = NULL;
	}
}

void FAIUITester::showIntroduction(bool detail)
{
	cout << "demo示例为输入命令，调用对应的函数，使用AIUI SDK完成文本理解，语义理解，文本合成等功能，如：" << endl;
	cout << "c命令，创建AIUI代理，与AIUI SDK交互都是通过代理发送消息的方式进行, 所以第一步必须是输入该命令；" << endl;
	cout << "w命令，发送外部唤醒命令唤醒AIUI，AIUI只有在唤醒过后才可以交互；" << endl;
	cout << "wr命令，单次读取本地pcm音频文件，写入sdk，sdk会返回云端识别的听写，语义结果；" << endl;
	cout << "wrt命令，字符串文本写入sdk，sdk会返回云端识别的语义结果；" << endl;
	cout << "stts命令，单合成示例，返回合成的音频，demo将音频保存为本地的pcm文件；" << endl;
	cout << "help命令，显示本demo提供的示例的介绍；" << endl;

	if (detail)
	{
		cout << "输入c命令后，正常情况返回结果为：" << endl;
		cout << "EVENT_STATE:READY" << endl;
		cout << "输入w命令后，正常情况返回结果为: " << endl;
		cout << "EVENT_WAKEUP:{\"CMScore\":-1,\"angle\":-1,\"beam\":0,\"channel\":-1,\"power\":-1}" << endl;
		cout << "EVENT_STATE:WORKING" << endl;

		cout << "听写，语义，合成等结果在onEvent函数，该函数是结果回调，请仔细研究。" << endl;
	}
}

void FAIUITester::readCmd()
{
	showIntroduction(false);

	cout << "input cmd:" << endl;

	string cmd;
	while (true)
	{
		cin >> cmd;

		if (cmd == "c")
		{
			cout << "createAgent" << endl;
			createAgent();
		}
		else if (cmd == "w") {
			cout << "wakeup" << endl;
			wakeup();
		}
		else if (cmd == "s") {
			cout << "start" << endl;
			start();
		}
		else if (cmd == "st") {
			cout << "stop" << endl;
			stop();
		}
		else if (cmd == "d") {
			cout << "destroy" << endl;
			destory();
		}
		else if (cmd == "r") {
			cout << "reset" << endl;
			reset();
		}
		else if (cmd == "e") {
			cout << "exit" << endl;
			break;
		}
		else if (cmd == "wr") {
			cout << "write" << endl;
			write(false);
		}
		else if (cmd == "wrr") {
			cout << "write repeatly" << endl;
			write(true);
		}
		else if (cmd == "swrt") {
			cout << "stopWriteThread" << endl;
			stopWriteThread();
		}
		else if (cmd == "wrt") {
			cout << "writeText" << endl;
			writeText();
		}
		else if (cmd == "q") {
			destory();
			break;
		}
		else if (cmd == "ssch") {
			syncSchema();
		}
		else if (cmd == "qss") {
			cout << "querySyncStatus" << endl;
			querySyncStatus();
		}
		else if (cmd == "stts") {
			cout << "startTts" << endl;
			startTts();
		}
		else if (cmd == "ptts") {
			cout << "pauseTts" << endl;
			pauseTts();
		}
		else if (cmd == "rtts") {
			cout << "resumeTts" << endl;
			resumeTts();
		}
		else if (cmd == "ctts") {
			cout << "cancelTts" << endl;
			cancelTts();
		}
		else if (cmd == "help") {
			showIntroduction(true);
		}
		else {
			cout << "invalid cmd, input again." << endl;
		}
	}
}
void FAIUITester::test()
{
	//		AIUISetting::setSaveDataLog(true);
	AIUISetting::setAIUIDir(TEST_ROOT_DIR);
	AIUISetting::initLogger(LOG_DIR);

	readCmd();
}