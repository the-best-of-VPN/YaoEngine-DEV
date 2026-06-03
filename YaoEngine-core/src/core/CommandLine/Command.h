#pragma once
namespace YaoEngine {
	enum class CommandType {
		None=0,
		Help,
		Headless,
		Editorless,
		WindowsMax,
	};
	class CommandLine
	{
	public:
		CommandLine(int, char**);
		~CommandLine() = default;

		void Parse(int, char**);
	};
	


}