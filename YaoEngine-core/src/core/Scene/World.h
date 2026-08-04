#ifndef WORLD_H
#define WORLD_H
#include <cstdint>
#include"Archetype.h"

struct Entity
{
	uint32_t index;
	uint32_t generation;
};
struct EntityLocat {
	
};
class World {
public:
	World();
	~World();

	Entity CreateEntity();
};
#endif