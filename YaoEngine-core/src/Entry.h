#pragma once
#include"./YaoEngine.h"
#include"./core/CommandLine/Command.h"
#include"../src/core/Log/Log.h"
extern  YaoEngine::YaoEngine* CreateApp();
namespace YaoEngine {
	
	int Main(const CommandLine &command)
	{
		LogSystem::YLog::Init();
		auto app = CreateApp();
		YaoTrace("YaoEngine Trace Log Initialized")
		app->run();
		delete app;
		return 0;
	}
}
int main(int args, char* argv[])
{
	YaoEngine::CommandLine command(args, argv);
	return YaoEngine::Main(command);
}

