#pragma once 
namespace YaoEngine {
	namespace LogSystem {
		enum class Level {
			None = -1,
			Trace,
			Debug,
			Info,
			Warn,
			Error,
			Fatal
		};

		inline const char* LevelToString(Level l) {
			switch (l) {
			case Level::Trace: return "TRACE";
			case Level::Debug: return "DEBUG";
			case Level::Info: return "INFO";
			case Level::Warn: return "WARN";
			case Level::Error: return "ERROR";
			case Level::Fatal: return "FATAL";
			}
			return "UNKNOWN";
		}
	}

	enum class Timetype {
		None = -1,
		Year,
		Month,
		Day,
		Hour,
		Min,
		Seconds,
	};
}