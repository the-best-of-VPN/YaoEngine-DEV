#pragma once 
#include<vector>
#include"./formatter/basic_formatter.h"
#include"./YaoLogcore.h"
namespace YaoEngine {
	namespace LogSystem {


		class Logger {
		public:
			Logger(const char* name,Level l=Level::None);
			~Logger();

			void SetLevel(Level);
			void parseformat(const char*);
			void Log(const char*,...);
			void Resetformat(const char*);

		private:
			std::vector<base_formatter*> m_formatterarray;
			Level m_level;
			std::string m_name;

		};
	}
}