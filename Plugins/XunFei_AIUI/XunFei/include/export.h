#pragma once
#include "stdafx.h"
typedef void(*OnRecText)(const char * text,int len);
//�滻��dll ���б���ʵ��export ���ķ���,�����޷���unity ����
EXTERN_C_START
//����create ���������ڳ�ʼ������ʹ��
DLL_API bool create(char* dir);
//������˷翪ʼ��������
DLL_API void startMic();
//�ر���˷�ֹͣ��������
DLL_API void stopMic();
//����
DLL_API void destroy();
//������������ص�����
DLL_API void setAnswerCallBack(OnRecText callBack);
DLL_API void setSpeechingCallBack(OnRecText callBack);
EXTERN_C_END