#pragma once
#include<string>
#include<Windows.h>
class FileDialogs
{
public:
	static std::string OpenFile(const char* filter);
	static std::string SaveFile(const char* filter);
};



class ConsleCommand {
public:
	static void SetFreeColor(int textColor, int bgColor = 0);
private:
};


