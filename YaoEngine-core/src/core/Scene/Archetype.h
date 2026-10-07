#ifndef HHHH_H
#define HHHH_H
#include "Component.h"
#include"Chunk.h"
#include <cstddef>
#include <vector>
namespace YaoEngine
{
    struct ComponentLayout
    {
        ComponentID id = 0;

        std::size_t offset = 0;
        std::size_t size = 0;
        std::size_t alignment = 0;
    };

    struct ArchetypeLayout
    {
        std::size_t capacity = 0;
        std::size_t entityOffset = 0;
        std::size_t totalSize = 0;

        std::vector<ComponentLayout> components;
    };
    class Archetype {
    public:
        Archetype();
        ~Archetype();
        
    private:
        constexpr static std::size_t AlignUp(
            std::size_t value,
            std::size_t alignment)
        {
            const std::size_t remainder = value % alignment;
            if (remainder == 0)
                return value;
            return value + alignment - remainder;
        }
    private:
        size_t m_count;
        ArchetypeLayout m_layer;
        std::vector<Chunk*> m_chunk;
    };
}
#endif