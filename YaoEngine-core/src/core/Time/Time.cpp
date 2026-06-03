#include"Time.h"
namespace YaoEngine {
	Time::Clock::time_point Time::s_StartTime = Time::Clock::now();
	Time::Clock::time_point Time::s_LastFrameTime = Time::Clock::now();
	float Time::s_DeltaTime = 0.0f;
}