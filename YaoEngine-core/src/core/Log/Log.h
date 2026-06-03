#pragma once 
#include"YaoLog/Logger.h"
namespace YaoEngine
{
	namespace LogSystem {
		class YLog {
		public:
			static void Init() {
				YaoEngine->parseformat("[%Y-%M-%D, %h:%m:%s]:%v");
				client->parseformat("[%Y-%M-%D, %h:%m:%s]:%v");
			
			}

			static Logger* GetYaoEngineLog() { return YaoEngine; }
			static Logger* GetclientLog() { return client; }

		private:
			static Logger* YaoEngine;
			static Logger* client;
		};


	}
}
#define YaoInfo(msg, ...)\
 YaoEngine::LogSystem::YLog::GetYaoEngineLog()->SetLevel(YaoEngine::LogSystem::Level::Info);\
 YaoEngine::LogSystem::YLog::GetYaoEngineLog()->Log(msg, ##__VA_ARGS__);
#define Yaoerror(msg, ...)\
 YaoEngine::LogSystem::YLog::GetYaoEngineLog()->SetLevel(YaoEngine::LogSystem::Level::Error);\
 YaoEngine::LogSystem::YLog::GetYaoEngineLog()->Log(msg, ##__VA_ARGS__);
#define YaoWarning(msg, ...)\
 YaoEngine::LogSystem::YLog::GetYaoEngineLog()->SetLevel(YaoEngine::LogSystem::Level::Warn);\
 YaoEngine::LogSystem::YLog::GetYaoEngineLog()->Log(msg, ##__VA_ARGS__);
#define YaoDebug(msg, ...)\
 YaoEngine::LogSystem::YLog::GetYaoEngineLog()->SetLevel(YaoEngine::LogSystem::Level::Debug);\
 YaoEngine::LogSystem::YLog::GetYaoEngineLog()->Log(msg, ##__VA_ARGS__);
#define YaoTrace(msg, ...)\
 YaoEngine::LogSystem::YLog::GetYaoEngineLog()->SetLevel(YaoEngine::LogSystem::Level::Trace);\
 YaoEngine::LogSystem::YLog::GetYaoEngineLog()->Log(msg, ##__VA_ARGS__);
#define clientInfo(msg, ...)\
 YaoEngine::LogSystem::YLog::GetclientLog()->SetLevel(YaoEngine::LogSystem::Level::Info);\
 YaoEngine::LogSystem::YLog::GetclientLog()->Log(msg, ##__VA_ARGS__);
#define clienterror(msg, ...)\
 YaoEngine::LogSystem::YLog::GetclientLog()->SetLevel(YaoEngine::LogSystem::Level::Error);\
 YaoEngine::LogSystem::YLog::GetclientLog()->Log(msg, ##__VA_ARGS__);
#define clientWarning(msg, ...)\
 YaoEngine::LogSystem::YLog::GetclientLog()->SetLevel(YaoEngine::LogSystem::Level::Warn);\
 YaoEngine::LogSystem::YLog::GetclientLog()->Log(msg, ##__VA_ARGS__);
#define clientDebug(msg, ...)\
 YaoEngine::LogSystem::YLog::GetclientLog()->SetLevel(YaoEngine::LogSystem::Level::Debug);\
 YaoEngine::LogSystem::YLog::GetclientLog()->Log(msg, ##__VA_ARGS__);		