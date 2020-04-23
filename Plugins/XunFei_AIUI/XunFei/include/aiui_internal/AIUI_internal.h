/*
 * 	AIUI.h
 *
 *  Created on: 2017年2月17日
 *      Author: hj
 */

#ifndef AIUI_API_INTERNAL_H_
#define AIUI_API_INTERNAL_H_

#include "aiui/AIUIConstant.h"
#include "aiui/AIUIErrorCode.h"
#include "aiui/AIUIType.h"
#include "aiui/AIUICommon.h"
#include "aiui/AIUI.h"

#include <string>
#include <map>

using namespace std;

void callJava_SetNetType();

void callJava_setGpsPos();

namespace aiui {

class IAIUIEvent;

/**
 * AIUI事件类，业务结果、SDK内部状态变化等都通过事件抛出。
 */
class AIUIEvent : public IAIUIEvent
{
public:
	/**
	 * 事件类型，具体取值参见AIUIConstant类。
	 */
	int eventType;

	/**
	 * 扩展参数1。
	 */

	int arg1;
	/**
	 * 扩展参数2。
	 */

	int arg2;

	/**
	 * 描述信息。
	 */
	string info;

	/**
	 * 附带数据。
	 */
	DataBundle* data;

	AIUIEvent(int msgType, int arg1, int arg2);

	AIUIEvent(int msgType, int arg1, int arg2, const string& info, DataBundle* data = NULL);

	~AIUIEvent();

	/**
	 * 释放data指针。
	 */
	void release();

public:
	inline int getEventType() const
	{
		return eventType;
	}

	inline int getArg1() const
	{
		return arg1;
	}

	inline int getArg2() const
	{
		return arg2;
	}

	inline const char* getInfo() const
	{
		return info.c_str();
	}

	inline IDataBundle* getData() const
	{
		return data;
	}

// 	inline void releaseData() { release(); }
// 	inline void destroy() { 
// 		if (data) release();
// 		delete this;
// 	}
};


/**
 * AIUI消息类，用于向SDK写入数据、发送命令。
 */
class AIUIMessage : public IAIUIMessage
{
public:
	/**
	 * 消息类型，具体取值参见AIUIConstant类。
	 */
	int msgType;

	/**
	 * 扩展参数1。
	 */
	int arg1;

	/**
	 * 扩展参数2。
	 */
	int arg2;

	/**
	 * 业务参数。
	 */
	string params;

	/**
	 * 附带数据。
	 */
	Buffer* data;

	AIUIMessage(int msgType, int arg1, int arg2, const string& params, Buffer* data = NULL);

	AIUIMessage(int msgType);

	~AIUIMessage();

	/**
	 * 释放data指针。
	 */
	void release();

public:
	int getMsgType() const
	{
		return msgType;
	}

	int getArg1() const
	{
		return arg1;
	}

	int getArg2() const
	{
		return arg2;
	}

	const char* getParams() const
	{
		return params.c_str();
	}

	Buffer* getData() const
	{
		return data;
	}

	void releaseData()
	{
		return release();
	}

	void destroy()
	{
		delete this;
	}
};


class AIUIAgentImpl;

/**
 * AIUI代理，单例对象，应用通过代理与AIUI交互。
 */
class AIUIAgent : public IAIUIAgent
{
private:
	static const char* TAG;

	static AIUIAgent* instance;

	const AIUIListener& mListener;

	AIUIAgentImpl* mAgentImplPtr;

	AIUIAgent(const string& params, const AIUIListener& listener);

public:
	~AIUIAgent();

	/**
	 * 创建Agent单例对象，AIUI开始工作。
	 * 注：该方法总是返回非空对象，非空并不代表创建过程中无错误发生。
	 *
	 * @param params 参数设置
	 * @param listener 监听器
	 * @return AIUIAgent单例对象
	 */
	static AIUIAgent* createAgent(const string& params, const AIUIListener& listener);

	/**
	 * 发送消息给AIUI，消息中可以包含命令、参数和数据，具体格式参见AIUIMessage。
	 *
	 * @param msg AIUI消息
	 */
	void sendMessage(const AIUIMessage& message);

	/**
	 * 销毁AIUIAgent对象，AIUI停止工作。
	 */
	void destroy();

public:
	inline void sendMessage(const IAIUIMessage* message)
	{
		sendMessage(*(static_cast<const AIUIMessage*>(message)));
	}
};


/**
 * 内部设置类，不对外开放。
 */
class AIUISetting_Internal
{
public:
	/**
	 * 设置设备的uniqueId，不设置则由SDK根据设备信息生成。
	 */
	AIUIEXPORT static void setUniqueId(const char* uniqueId);

	/**
	 * 设置版本类型。
	 */
	static void setVersionType(VersionType type);

	/**
	 * 设置系统信息，如网络类型，IMEI等。
	 */
	AIUIEXPORT static void setSystemInfo(const char* key, const char* val);

	/**
	 * 设置GPS坐标。
	 */
	static void setGpsPos(float lng, float lat);
};

}


#endif /* AIUI_API_H_ */
