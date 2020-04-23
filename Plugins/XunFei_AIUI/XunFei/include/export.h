#pragma once
#include "stdafx.h"
typedef void(*OnRecText)(const char * text,int len);
//替换的dll 库中必须实现export 出的方法,否则无法与unity 交互
EXTERN_C_START
//导出create 方法，用于初始化环境使用
DLL_API bool create(char* dir);
//开启麦克风开始语音交互
DLL_API void startMic();
//关闭麦克风停止语音交互
DLL_API void stopMic();
//销毁
DLL_API void destroy();
//设置语音命令回调函数
DLL_API void setAnswerCallBack(OnRecText callBack);
DLL_API void setSpeechingCallBack(OnRecText callBack);
EXTERN_C_END