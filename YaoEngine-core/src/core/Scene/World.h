#ifndef WORLD_H
#define WORLD_H
#include <cstdint>
#include"Archetype.h"
namespace YaoEngine {
	struct Entity
	{
		uint32_t index;
		uint32_t generation;
	};
	struct EntityLocation
	{
		Archetype* archetype = nullptr;
		uint32_t row = 0;
	};
	class World {
	public:
		World();
		~World();

		Entity CreateEntity();
	};
}
#endif