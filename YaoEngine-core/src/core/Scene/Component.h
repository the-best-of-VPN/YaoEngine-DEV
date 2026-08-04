#ifndef YAO_ENGINE_COMPONENT_H
#define YAO_ENGINE_COMPONENT_H

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace YaoEngine
{
    using ComponentID = std::uint32_t;

    struct ComponentInfo
    {
        ComponentID id = 0;
        std::size_t size = 0;
        std::size_t alignment = 0;
    };

    class ComponentRegistry
    {
    public:
        ComponentRegistry() = default;
        ~ComponentRegistry() = default;

        template<class T>
        ComponentInfo Register()
        {
            const auto id = static_cast<ComponentID>(
                m_componentInfos.size()
                );

            ComponentInfo info = MakeComponentInfo<T>(id);

            m_componentInfos.push_back(info);

            return info;
        }

        [[nodiscard]]
        const ComponentInfo& Get(ComponentID id) const
        {
            if (id >= m_componentInfos.size())
            {
                throw std::out_of_range(
                    "Invalid ComponentID"
                );
            }

            return m_componentInfos[id];
        }

        [[nodiscard]]
        std::size_t Count() const noexcept
        {
            return m_componentInfos.size();
        }

    private:
        template<class T>
        static ComponentInfo MakeComponentInfo(ComponentID id)
        {
            return ComponentInfo{
                id,
                sizeof(T),
                alignof(T)
            };
        }

    private:
        std::vector<ComponentInfo> m_componentInfos;
    };
}

#endif // YAO_ENGINE_COMPONENT_H