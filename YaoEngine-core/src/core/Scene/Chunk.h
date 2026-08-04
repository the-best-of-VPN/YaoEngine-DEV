#ifndef CHUNK_H
#define CHUNK_H

#include <cassert>
#include <cstddef>
#include <memory>
#include <new>
#include <vector>
namespace YaoEngine {
    class Chunk
    {
    public:
        static constexpr std::size_t ChunkSize = 16 * 1024;
        static constexpr std::size_t ChunkAlignment = 64;

        Chunk()
            : m_memory(static_cast<std::byte*>(
                ::operator new(
                    ChunkSize,
                    std::align_val_t{ ChunkAlignment })))
        {
        }

        ~Chunk()
        {
            ::operator delete(
                m_memory,
                std::align_val_t{ ChunkAlignment });
        }

        Chunk(const Chunk&) = delete;
        Chunk& operator=(const Chunk&) = delete;
        Chunk(Chunk&&) = delete;
        Chunk& operator=(Chunk&&) = delete;

        [[nodiscard]]
        std::byte* Data() noexcept
        {
            return m_memory;
        }

        [[nodiscard]]
        const std::byte* Data() const noexcept
        {
            return m_memory;
        }

    private:
        friend class ChunkPool;

        std::byte* m_memory = nullptr;
        Chunk* m_nextFree = nullptr;
    };

    class ChunkPool
    {
    public:
        static constexpr std::size_t MinCapacity = 256;
        static constexpr std::size_t GrowthCount = 256;

        explicit ChunkPool(std::size_t initialCapacity = MinCapacity)
        {
            Grow(initialCapacity);
        }

        ~ChunkPool() = default;

        ChunkPool(const ChunkPool&) = delete;
        ChunkPool& operator=(const ChunkPool&) = delete;

        [[nodiscard]]
        Chunk* Allocate()
        {
            if (m_freeHead == nullptr)
            {
                Grow(GrowthCount);
            }

            Chunk* chunk = m_freeHead;

            m_freeHead = m_freeHead->m_nextFree;
            chunk->m_nextFree = nullptr;

            --m_freeCount;
            return chunk;
        }

        void Deallocate(Chunk* chunk) noexcept
        {
            assert(chunk != nullptr);

            chunk->m_nextFree = m_freeHead;
            m_freeHead = chunk;

            ++m_freeCount;
        }

        [[nodiscard]]
        std::size_t FreeCount() const noexcept
        {
            return m_freeCount;
        }

        [[nodiscard]]
        std::size_t Capacity() const noexcept
        {
            return m_chunks.size();
        }

    private:
        void Grow(std::size_t count)
        {
            m_chunks.reserve(m_chunks.size() + count);

            for (std::size_t i = 0; i < count; ++i)
            {
                auto chunk = std::make_unique<Chunk>();

                chunk->m_nextFree = m_freeHead;
                m_freeHead = chunk.get();

                m_chunks.emplace_back(std::move(chunk));
                ++m_freeCount;
            }
        }

    private:
        std::vector<std::unique_ptr<Chunk>> m_chunks;

        Chunk* m_freeHead = nullptr;
        std::size_t m_freeCount = 0;
    };
}
#endif