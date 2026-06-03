#include"./UUID.h"
namespace YaoEngine{
	std::mt19937_64 UUID::s_Engine(std::random_device{}());

	std::uniform_int_distribution<uint64_t>
		UUID::s_UniformDistribution;
}