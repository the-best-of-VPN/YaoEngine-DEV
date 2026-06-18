#pragma once
#include"./YaoEngine.h"
#include"./core/CommandLine/Command.h"
#include"../src/core/Log/Log.h"
extern  YaoEngine::YaoEngine* CreateApp();
namespace YaoEngine {
	
	int Main(const CommandLine& command)
	{
		LogSystem::YLog::Init();
		auto app = CreateApp();
		//demo
		//int sd = 0;
		//YAO_LOG_INIT("YaoEngine Init %d", sd)
		//YaoInfo("CommandLine parsed");
		app->run();
		delete app;
		return 0;
	}
}
int main(int args, char**argv)
{
	YaoEngine::CommandLine command(args, argv);
	return Main(command);
}

