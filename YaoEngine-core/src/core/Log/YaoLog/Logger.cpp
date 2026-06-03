#include"Logger.h"
#include<Mysystem/Windowtool/Windowstool.h>
namespace YaoEngine {
	namespace LogSystem {
		Logger::Logger(const char *name, Level l)
		{
			m_name = name;
			m_level = l;
			m_formatterarray = {};
		}
		Logger::~Logger()
		{

		}

		void Logger::parseformat(const char *fmt)
		{
			if (fmt == nullptr)
			{
				return;
			}
			std::string formatstr = fmt;
			for (int i = 0; i < formatstr.size(); i++)
			{
			if (formatstr[i]!='%')
			{
				m_formatterarray.emplace_back(new string_formatter(formatstr[i]));
				
			}
				else if (formatstr[i] == '%' && i + 1 < formatstr.size() && formatstr[i + 1] == 'Y')
				{
					m_formatterarray.emplace_back(new Timer_formatter(Timetype::Year));
					i++;
				}
				else if(formatstr[i] == '%' && i + 1 < formatstr.size() && formatstr[i + 1] == 'M')
				{
					m_formatterarray.emplace_back(new Timer_formatter(Timetype::Month));
					i++;
				}
				else if (formatstr[i] == '%' && i + 1 < formatstr.size() && formatstr[i + 1] == 'D')
				{
					m_formatterarray.emplace_back(new Timer_formatter(Timetype::Day));
					i++;
				}
				else if (formatstr[i] == '%' && i + 1 < formatstr.size() && formatstr[i + 1] == 'h')
				{
					m_formatterarray.emplace_back(new Timer_formatter(Timetype::Hour));
					i++;
				}
				else if (formatstr[i] == '%' && i + 1 < formatstr.size() && formatstr[i + 1] == 'm')
				{
					m_formatterarray.emplace_back(new Timer_formatter(Timetype::Min));
					i++;
				}
				else if (formatstr[i] == '%' && i + 1 < formatstr.size() && formatstr[i + 1] =='s')
				{
					m_formatterarray.emplace_back(new Timer_formatter(Timetype::Seconds));
					i++;
				}
				else if (formatstr[i] == '%' && i + 1 < formatstr.size() && formatstr[i + 1] == 'l')
				{
					m_formatterarray.emplace_back(new Level_formatter(m_level));
				}
				else if (formatstr[i] == '%' && i + 1 < formatstr.size() && formatstr[i + 1] == 'v')
				{
					m_formatterarray.emplace_back(new msg_formatter());
					i++;
				}
				
			
			}
		}

		void Logger::Resetformat(const char* fmt)
		{

		}

		void Logger::SetLevel(Level l)
		{
			m_level = l;
		}

		void Logger::Log(const char* msg, ...)
		{
			va_list args;
			char buffer[1024];
			va_start(args, msg);
			vsnprintf(buffer, sizeof(buffer), msg, args); 
			va_end(args);
			switch (m_level)
			{
				case Level::Trace:
					ConsleCommand::SetFreeColor(1);
					break;
				case Level::Debug:
					ConsleCommand::SetFreeColor(2);
					break;
				case Level::Info:
					ConsleCommand::SetFreeColor(3);
					break;
				case Level::Warn:
					ConsleCommand::SetFreeColor(4);
					break;
				case Level::Error:
					ConsleCommand::SetFreeColor(5);
					break;
				case Level::Fatal:
					ConsleCommand::SetFreeColor(6);
					break;
				default:
					std::cout<<"Invalid Level"<<std::endl;
					break;
			}
			std::cout<<m_name<<": ";
			for (auto& formatter : m_formatterarray)
			{
				formatter->Log(buffer);
			}
			std::cout << std::endl;
			ConsleCommand::SetFreeColor(15);
			}
		
	}
}

