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
namespace YaoEngine {
#define YaoInfo(msg, ...)\
 LogSystem::YLog::GetYaoEngineLog()->SetLevel(LogSystem::Level::Info);\
 LogSystem::YLog::GetYaoEngineLog()->Log(msg, ##__VA_ARGS__);
#define Yaoerror(msg, ...)\
 LogSystem::YLog::GetYaoEngineLog()->SetLevel(LogSystem::Level::Error);\
 LogSystem::YLog::GetYaoEngineLog()->Log(msg, ##__VA_ARGS__);
#define YaoWarning(msg, ...)\
 LogSystem::YLog::GetYaoEngineLog()->SetLevel(LogSystem::Level::Warn);\
 LogSystem::YLog::GetYaoEngineLog()->Log(msg, ##__VA_ARGS__);
#define YaoDebug(msg, ...)\
 LogSystem::YLog::GetYaoEngineLog()->SetLevel(LogSystem::Level::Debug);\
 LogSystem::YLog::GetYaoEngineLog()->Log(msg, ##__VA_ARGS__);
#define YaoTrace(msg, ...)\
 LogSystem::YLog::GetYaoEngineLog()->SetLevel(LogSystem::Level::Trace);\
 LogSystem::YLog::GetYaoEngineLog()->Log(msg, ##__VA_ARGS__);
#define clientInfo(msg, ...)\
 LogSystem::YLog::GetclientLog()->SetLevel(LogSystem::Level::Info);\
 LogSystem::YLog::GetclientLog()->Log(msg, ##__VA_ARGS__);
#define clienterror(msg, ...)\
 LogSystem::YLog::GetclientLog()->SetLevel(LogSystem::Level::Error);\
 LogSystem::YLog::GetclientLog()->Log(msg, ##__VA_ARGS__);
#define clientWarning(msg, ...)\
 LogSystem::YLog::GetclientLog()->SetLevel(LogSystem::Level::Warn);\
 LogSystem::YLog::GetclientLog()->Log(msg, ##__VA_ARGS__);
#define clientDebug(msg, ...)\
 LogSystem::YLog::GetclientLog()->SetLevel(LogSystem::Level::Debug);\
 LogSystem::YLog::GetclientLog()->Log(msg, ##__VA_ARGS__);	
}
#define YAO_LOG_INIT(msg,...)\
do{\
 YaoInfo(msg, ##__VA_ARGS__);\
}while(0);