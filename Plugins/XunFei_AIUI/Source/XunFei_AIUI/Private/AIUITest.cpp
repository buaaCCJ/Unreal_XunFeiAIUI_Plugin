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
			L"rec_user_data={\"sceneInfo\":{},\"recHotWords\":\"����|������|֪ͨ��|ͼƬ|�ٶ�|����|��ҳ|��Ŀ¼|��Ŀ¼|����|��|�˳�|����|����|����Ӳ��|�½�|����|ˢ��|��ͼ|����\"}";
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
/* ѭ��д�������Ƶ��ÿ��д1280B */
bool FWriteAudioThread::threadLoop()
{
	char audio[1280];
	int len1 = mFileHelper->read(audio, 1280);

	if (len1 > 0)
	{
		Buffer* buffer = Buffer::alloc(len1);
		memcpy(buffer->data(), audio, len1);

		const wchar_t * para = L"data_type=audio,sample_rate=16000,msc.lng=117.14430236816406,msc.lat=31.831058502197266,"
			L"rec_user_data={\"sceneInfo\":{},\"recHotWords\":\"����|������|֪ͨ��|ͼƬ|�ٶ�|����|��ҳ|��Ŀ¼|��Ŀ¼|����|��|�˳�|����|����|����Ӳ��|�½�|����|ˢ��|��ͼ|����\"}";
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
	AIUI �¼��ص��ӿ�
*/
void FTestListener::onEvent(const IAIUIEvent& event) const
{

	switch (event.getEventType()) {
		/* ״̬�ص�        */
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

	/* �����¼��ص�        �� ���ѳɹ�����ô˽ӿڣ����ҿ���������Ϣ               */

	case AIUIConstant::EVENT_WAKEUP:
	{
		UE_LOG(LogTemp, Log, TEXT("EVENT_WAKEUP::%s"), event.getInfo());
	} break;

	/* �����¼��ص�            */
	case AIUIConstant::EVENT_SLEEP:
	{
		UE_LOG(LogTemp, Log, TEXT("EVENT_SLEEP:arg1=::%s"), event.getArg1());
	} break;

	/* VAD�¼��ص�����⵽ǰ��˵�  */
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
		����Ҫ�Ľ���¼��ص����յ��ı����������嶼�᷵�ش��¼��������н����Ϣ
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

				/*����Ĳ�����Ҫ���÷��ص�UTF-8����Ľ��ת��ΪGBK��ʽ������dos����������ʾ*/

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
					�����������������ǿս����ֱ�Ӷ���
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
		}else if(sub == "iat")
		{
			Json::Value empty;
			Json::Value contentId = content.get("cnt_id", empty);
			string cnt_id = contentId.asString();
			int dataLen = 0;
			const char* buffer = event.getData()->getBinary(cnt_id.c_str(), &dataLen);
			if (contentId.empty()) {
				cout << "Content Id is empty" << endl;
				break;
			}
			string resultStr = string(buffer, dataLen);
			Json::Value resultJson;
			
			if (reader.parse(resultStr, resultJson, false)) {
				const auto& text = resultJson["text"];
				const auto& ws = text["ws"];

				FString temp;

				for (const auto& item : ws) {
					const auto& cw = item["cw"][0];
					temp += FString(UTF8_TO_TCHAR(cw["w"].asString().c_str()));
				}
				OnIATFinished.ExecuteIfBound(temp, temp, -1);

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

		UE_LOG(LogTemp, Log, TEXT("IAT__error info:%s"), UTF8_TO_TCHAR(lpGBKStr));
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
	����AIUI����������û����õĲ�����ͨ�����ɵ�AIUI�������agent��AIUI SDK������
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

		/* ���������ʹ�õ�·ivw���ѣ���ô��Ҫ�û��Լ���֧�ֵ�·���ѵ�msc.dll����aiui.dllͬĿ¼�£�����
			���úû�����Դ��·����
			��������˵�·���ѣ���createAgent()�ɹ�֮�󣬾Ͳ���Ҫ����wakeUp()�ӿ�ͨ���ⲿ��������ķ�ʽ���л��ѣ�����ͨ��д�뻽����Ƶ�ķ�ʽ��
			Ĭ��û������ivw���ѡ�
		*/
		string wakeup_mode = paramJson["speech"]["wakeup_mode"].asString();
		if (wakeup_mode == "ivw")
		{
			/*
				ʹ�õ�·����ʱ��������Ҫ����msc�������MSPLogin()
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
	�ⲿ���ѽӿڣ�ͨ������CMD_WAKEUP����ʹAIUI SDKת�뻽��״̬��
	����״̬�¾Ϳ���д���ı������壬�õ����
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

/*���AIUI SDK, �ýӿ���stop()�ӿڶ�Ӧ��ֻ���ڵ�����stop()�ӿ�ֹͣ��AIUI SDK������£�
��Ҫ�ٴ����AIUI SDKʱ���øýӿ�
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


/*ֹͣAIUI      SDK��ֹ֮ͣ���ڿ���״̬  */
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
	д����Ƶ�ӿ�
	�����µ��߳�д����Ƶ����Ƶ���ļ���ȡ
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

/*��������д����Ƶ���߳�*/
void FAIUITester::stopWriteThread()
{
	if (writeThread) {
		writeThread->stopRun();
		delete writeThread;
		writeThread = NULL;
	}
}

/*
	״̬���ýӿڣ�����CMD_RESET�����AIUI SDK֮��SDKԭ���Ļ���״̬�ᱻ����Ϊ�ǻ���״̬��
	��Ҫ���»��Ѳ����ٴ�д����Ƶ���ı�
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
	�ı�����ӿ�
*/
void FAIUITester::writeText()
{
	if (NULL != agent)
	{
		const wchar_t * text = L"�鸽������ʳ��";

		/*������ı���Ҫת��Ϊutf-8����*/
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
	��ѯ����ͬ��״̬
*/
void FAIUITester::querySyncStatus()
{
	if (NULL != agent)
	{
		Json::Value paramJson;

		/* ����������Ҫ��Ӧͬ���ϴ�������sid */
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

		const wchar_t * text = L"{\"name\":\"����Ա\", \"alias\":\"����\"}\n{\"name\":\"���ʦ\", \"alias\":\"���\"}\n{\"name\":\"��Ʒ����\", \"alias\":\"��Ʒ\"}";
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
		//const wchar_t * text = TEXT("�ƴ�Ѷ��");
		/*������ı���Ҫת��Ϊutf-8����*/
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
		 arg1ȡֵ˵����
			START	��ʼ�ϳ�	�ϳɷ����ˣ����������
			CANCEL	ȡ���ϳ�	��
			PAUSE	��ͣ����	��
			RESUME	�ָ�����	��
		*/

		/*
		�ϳɲ���ʾ����
		String params = "vcn=xiaoyan,speed=50,pitch=50,volume=50"

		�����ֶ�˵����
			vcn	�����ˣ���xiaoyan
			speed	�ٶȣ�0-100
			pitch	�����0-100
			volume	������0-100
			ent	���棬Ĭ��aisound�������Ҫ�Ϻõ�Ч���������ó�xtts
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
	cout << "demoʾ��Ϊ����������ö�Ӧ�ĺ�����ʹ��AIUI SDK����ı���⣬������⣬�ı��ϳɵȹ��ܣ��磺" << endl;
	cout << "c�������AIUI�������AIUI SDK��������ͨ�����������Ϣ�ķ�ʽ����, ���Ե�һ����������������" << endl;
	cout << "w��������ⲿ���������AIUI��AIUIֻ���ڻ��ѹ���ſ��Խ�����" << endl;
	cout << "wr������ζ�ȡ����pcm��Ƶ�ļ���д��sdk��sdk�᷵���ƶ�ʶ�����д����������" << endl;
	cout << "wrt����ַ����ı�д��sdk��sdk�᷵���ƶ�ʶ�����������" << endl;
	cout << "stts������ϳ�ʾ�������غϳɵ���Ƶ��demo����Ƶ����Ϊ���ص�pcm�ļ���" << endl;
	cout << "help�����ʾ��demo�ṩ��ʾ���Ľ��ܣ�" << endl;

	if (detail)
	{
		cout << "����c���������������ؽ��Ϊ��" << endl;
		cout << "EVENT_STATE:READY" << endl;
		cout << "����w���������������ؽ��Ϊ: " << endl;
		cout << "EVENT_WAKEUP:{\"CMScore\":-1,\"angle\":-1,\"beam\":0,\"channel\":-1,\"power\":-1}" << endl;
		cout << "EVENT_STATE:WORKING" << endl;

		cout << "��д�����壬�ϳɵȽ����onEvent�������ú����ǽ���ص�������ϸ�о���" << endl;
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