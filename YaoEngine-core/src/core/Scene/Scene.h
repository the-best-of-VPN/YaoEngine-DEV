#pragma once
#include"./Entity.h"
#include"Component.h"
#include"../../core.h"
#include<vector>
namespace YaoEngine {
	class Scene {
	public:
		Scene();
		~Scene();

		Scope<Entity>& CreateEntity()
		{
			auto m= CreateScope<Entity>(this, nexthandle++);
			m_entities.push_back(std::move(m));
			Scope<Entity>& entity = m_entities.back();
			return entity;
		};
		template<class T>
		void AddComponent(Entity& entity, T component) {

		}


	private:
		std::vector<Scope<Entity>> m_entities;
		handle nexthandle = 0;
	};
}