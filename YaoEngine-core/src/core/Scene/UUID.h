#pragma once

#include <random>
#include <cstdint>

namespace YaoEngine {
	class UUID {
	public:
		UUID() {};
		UUID(uint64_t uuid) {};
		uint64_t Get() const { return m_UUID; }

	private:
		uint64_t m_UUID = 0;

	private:
		static std::mt19937_64 s_Engine;
		static std::uniform_int_distribution<uint64_t>
			s_UniformDistribution;
	};

}