#pragma once

#ifdef __cplusplus
#include <iostream>

extern "C" {
#endif // __cplusplus

#pragma warning(disable:4996)
	/*
	* 语音听写(iFly Auto Transform)技术能够实时地将语音转换成对应的文字。
	*/

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <errno.h>
#include <process.h>

#include "msp_cmn.h"
#include "msp_errors.h"
#include "speech_sample.h"
#include "ISE.h"

#define FRAME_LEN	640 
#define	BUFFER_SIZE	4096
////普通话
//#define MARDRAIN \
//    "sub = iat, domain = iat, language = zh_cn, accent = mandarin, sample_rate = 16000, result_type = plain, result_encoding = gb2312"
////粤语
//#define CANTONESE \
//    "sub = iat, domain = iat, language = zh_cn, accent = cantonese, sample_rate = 16000, result_type = plain, result_encoding = gb2312"
////四川话
//#define LMZ \
//    "sub = iat, domain = iat, language = zh_cn, accent = lmz, sample_rate = 16000, result_type = plain, result_encoding = gb2312"
////河南话
//#define HENANESE \
//    "sub = iat, domain = iat, language = zh_cn, accent = henanese, sample_rate = 16000, result_type = plain, result_encoding = gb2312"
//



		
	//声明全局属性
	struct speech_rec iat;
	//当前的错误编码  正常为0
	int errcode;
	//是否初始化成功
	boolean isInitalSuccess = false;
	enum {
		EVT_START = 0,
		EVT_STOP,
		EVT_QUIT,
		EVT_TOTAL
	};


	inline void isInitSuccess() {
		if (!isInitalSuccess)return;
	}

	Resoult mResult = NULL;
	SpeechBegin mSpeechBegin = NULL;
	SpeechEnd mSpeechEnd = NULL;

	static HANDLE events[EVT_TOTAL] = { NULL,NULL,NULL };

	static COORD begin_pos = { 0, 0 };
	static COORD last_pos = { 0, 0 };
	//用来显示请求后的结果
	static char * lastWord = NULL;

	static void show_result(char *string, char is_over)
	{
		//lastWord = string;
	/*	printf("当前识别结果  : [%s]" , string);
		if (mResult) {
			mResult(string);
		}*/
	}



	static char *g_result = NULL;
	static unsigned int g_buffersize = BUFFER_SIZE;

	void on_result(const char *result, char is_last)
	{
		std::cout << "--on_result--result--"<< result << std::endl;
		std::cout << "--on_result----is_last--" << is_last << std::endl;
		if (result&&mResult) {
			mResult((char* )result);
		}
		//转接证
		/*if (result) {
			size_t left = g_buffersize - 1 - strlen(g_result);
			size_t size = strlen(result);
			if (left < size) {
				g_result = (char*)realloc(g_result, g_buffersize + BUFFER_SIZE);
				if (g_result)
					g_buffersize += BUFFER_SIZE;
				else {
					printf("mem alloc failed\n");
					return;
				}
			}
			strncat(g_result, result, size);
			show_result(g_result, is_last);
		}*/
	}
	void on_speech_begin()
	{
		std::cout << "--on_speech_begin--" << std::endl;
		if (g_result)
		{
			free(g_result);
		}
		g_result = (char*)malloc(BUFFER_SIZE);
		g_buffersize = BUFFER_SIZE;
		memset(g_result, 0, g_buffersize);

		if (mSpeechBegin)
		{
			
			mSpeechBegin();
		}
		std::cout << "Start Listening...\n" << std::endl;
	}
	void on_speech_end(int reason)
	{
		std::cout << "--on_speech_end--" << std::endl;
		if (reason == END_REASON_VAD_DETECT) {
			std::cout << "\nSpeaking done \n" << std::endl;
		}
		if (mSpeechEnd)
		{
			//显示结果

			mSpeechEnd();
		}
		else
		 std::cout << "\nRecognizer error %d\n" << std::endl;


	}


	//用来进行停止当前的语音识别
	 void onStopRecoginer(){
		 isInitSuccess();
		 if (errcode = sr_stop_listening(&iat)) {
			 std::cout << "top listening failed -------%d\n"<< errcode << std::endl;
		 }
	 
	 }

	 //用来销毁当前的语音识别
	 void onDestoryISE(){
		 isInitSuccess();
		 sr_uninit(&iat);
	 }
	


	 struct speech_rec_notifier recnotifier = {
		 on_result,
		 on_speech_begin,
		 on_speech_end
	 };
	 void onStartRecognier(Resoult result, SpeechBegin begin, SpeechEnd end,const char * accent) {

		 if (!&iat) {
			 sr_uninit(&iat);
		 }
		 char  c[200];
		 sprintf(c,"sub = iat, domain = iat, language = zh_cn, accent = %s, sample_rate = 16000, result_type = plain, result_encoding = gb2312", accent);
		 std::cout << "--当前--" << c << "---\n" << std::endl;
		 std::cout << "--错误吗--" << errcode << "\n" << std::endl;
		 errcode = sr_init(&iat, c, SR_MIC, DEFAULT_INPUT_DEVID, &recnotifier);
		 if (errcode) {
			 printf("speech recognizer init failed\n");
			 isInitalSuccess = false;
			 return;
		 }
		 isInitalSuccess = true;
		 //isInitSuccess();
		 mResult = result;
		 mSpeechBegin = begin;
		 mSpeechEnd = end;
		 if (errcode = sr_start_listening(&iat)) {
			 printf("stop listening failed %d\n", errcode);
		 }
	 }


	 ////用来处理当前开始的状态
	 //void Recognier_IsBegin(SpeechBegin begin);
	 // void  Recognier_IsEnd(SpeechEnd end);
	 //用来初始化ISE的语音合成



	 void onInitalISE() {
		const char* login_params = "appid = 58d4949a, work_dir = ."; // 登录参数，appid与msc库绑定,请勿随意改动
		int	ret = MSPLogin(NULL, NULL, login_params); //第一个参数是用户名，第二个参数是密码，均传NULL即可，第三个参数是登录参数	
		if (MSP_SUCCESS != ret) {
			std::cout << "MSPLogin failed , Error code %d.\n" << ret << std::endl;
			return; //登录失败，退出登录
		}
	 }


	///* main thread: start/stop record ; query the result of recgonization.
	//* record thread: record callback(data write)
	//* helper thread: ui(keystroke detection)
	//*/
	//int main(int argc, char* argv[])
	//{
	//	int			ret = MSP_SUCCESS;
	//	int			upload_on = 1; //是否上传用户词表
	//	const char* login_params = "appid = 58d8b9b7, work_dir = ."; // 登录参数，appid与msc库绑定,请勿随意改动
	//	int aud_src = 0;

	//	/*
	//	* sub:				请求业务类型
	//	* domain:			领域
	//	* language:			语言
	//	* accent:			方言
	//	* sample_rate:		音频采样率
	//	* result_type:		识别结果格式
	//	* result_encoding:	结果编码格式
	//	*
	//	* 详细参数说明请参阅《iFlytek MSC Reference Manual》
	//	*/
	//	const char* session_begin_params = "sub = iat, domain = iat, language = zh_cn, accent = mandarin, sample_rate = 16000, result_type = plain, result_encoding = gb2312";

	//	/* 用户登录 */
	//	ret = MSPLogin(NULL, NULL, login_params); //第一个参数是用户名，第二个参数是密码，均传NULL即可，第三个参数是登录参数	
	//	if (MSP_SUCCESS != ret) {
	//		printf("MSPLogin failed , Error code %d.\n", ret);
	//		goto exit; //登录失败，退出登录
	//	}

	//	printf("\n########################################################################\n");
	//	printf("## 语音听写(iFly Auto Transform)技术能够实时地将语音转换成对应的文字。##\n");
	//	printf("########################################################################\n\n");
	//	printf("演示示例选择:是否上传用户词表？\n0:不使用\n1:使用\n");

	//	scanf("%d", &upload_on);
	//	if (upload_on)
	//	{
	//		printf("上传用户词表 ...\n");
	//		ret = upload_userwords();
	//		if (MSP_SUCCESS != ret)
	//			goto exit;
	//		printf("上传用户词表成功\n");
	//	}

	//	printf("音频数据在哪? \n0: 从文件读入\n1:从MIC说话\n");
	//	scanf("%d", &aud_src);
	//	if (aud_src != 0) {
	//		demo_mic(session_begin_params);
	//	}
	//	else {
	//		//iflytek02音频内容为“中美数控”；如果上传了用户词表，识别结果为：“中美速控”。;
	//		demo_file("wav/iflytek02.wav", session_begin_params);
	//	}
	//exit:
	//	printf("按任意键退出 ...\n");
	//	_getch();
	//	MSPLogout(); //退出登录

	//	return 0;
	//}



#ifdef __cplusplus
}
#endif // __cplusplus

