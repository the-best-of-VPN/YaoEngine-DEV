#pragma once
namespace YaoEngine {
	class Scene;
	using handle = uint32_t;
	class Entity {
	public:
		friend class Scene;
	private:
		Entity(Scene* scene,handle id) : m_scene(scene), m_id(id) {
		}
		Entity(const Entity& other) = delete;
		~Entity() = default;


		const handle& GetID() const { return m_id; }
		Scene* m_scene=nullptr;
		handle m_id;
	};
}