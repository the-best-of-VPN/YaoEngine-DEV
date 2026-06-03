#pragma once
#include "../YaoLogcore.h"
#include<iostream>
#include<chrono>
namespace YaoEngine {
	namespace LogSystem {
		class base_formatter {
		public:
			virtual ~base_formatter()=default;

			
			virtual void Log(const char*) = 0;
		};

		class string_formatter :public base_formatter {
		public:
			string_formatter(char str) { m_Str = str; }
			virtual ~string_formatter() {}
			virtual void Log(const char*)			   override { std::cout << m_Str; }

		private:
			char m_Str;
		};
		
		class Timer_formatter :public base_formatter {
		public:
			Timer_formatter(Timetype t = Timetype::None) { m_time = t; };
			virtual ~Timer_formatter() {};


			
			virtual void Log(const char*)			   override 
			{
				auto time = std::chrono::system_clock::now();
				std::time_t t = std::chrono::system_clock::to_time_t(time);
				std::tm localTime;
#ifdef _WIN32
				localtime_s(&localTime, &t); 
#else
				localtime_r(&t, &localTime);
#endif
				// 分别取出年月日时分秒
				int year = localTime.tm_year + 1900; // 从1900开始
				int month = localTime.tm_mon + 1;     // 从0开始
				int day = localTime.tm_mday;
				int hour = localTime.tm_hour;
				int minute = localTime.tm_min;
				int second = localTime.tm_sec;
				switch (m_time)
				{
				case Timetype::Year:
					std::cout << year;
					break;
				case Timetype::Month:
					std::cout << month;
					break;
				case Timetype::Day:
					std::cout << day;
					break;
				case Timetype::Hour:
					std::cout << hour;
					break;
				case Timetype::Min:
					std::cout << minute;
					break;
				case Timetype::Seconds:
					std::cout << second;
					break;
				default:
					std::cout << "error code";
					break;
				}
			}
		public:
			Timetype m_time;
		};

		class Level_formatter	:public base_formatter {
		public:
			Level_formatter(Level l = Level::None) { m_level = l; }
			virtual ~Level_formatter() {};

			
			virtual void Log(const char*)			   override { std::cout << LevelToString(m_level); }
		private:
			Level m_level;
		};

		/*class EntityId_formatter :public base_formatter {
		public:
			
			virtual void Log(const char*)			   override;
		};*/
		class msg_formatter :public base_formatter {
		public:
			msg_formatter() {};
			virtual ~msg_formatter() {};

			
			virtual void Log(const char*msg)			   override { std::cout << msg; }


		};
	}
}