#pragma once
#include"./YaoEngine.h"
extern  YaoEngine::YaoEngine* CreateApp();
namespace YaoEngine {
	
	int Main()
	{
		auto app = CreateApp();
		app->run();
		delete app;
		return 0;
	}
}
int main()
{
	return YaoEngine::Main();
}

