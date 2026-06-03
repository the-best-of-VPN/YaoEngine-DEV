#pragma once

#include <chrono>

namespace YaoEngine {

	class Time
	{
	public:
		static void Update() {
			auto now = Clock::now();

			s_DeltaTime =
				std::chrono::duration<float>(now - s_LastFrameTime).count();

			s_LastFrameTime = now;
		}

		static float DeltaTime() {
			return s_DeltaTime;
		}

		static float TotalTime() {
			return std::chrono::duration<float>(
				Clock::now() - s_StartTime).count();
		}

	private:
		using Clock = std::chrono::steady_clock;

		static Clock::time_point s_StartTime;
		static Clock::time_point s_LastFrameTime;

		static float s_DeltaTime;
	};

}