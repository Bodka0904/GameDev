#pragma once

#include "Scene.h"

namespace XYZ {
	class SceneEntity
	{
	public:
		SceneEntity()
			:
			m_Scene(nullptr),
			m_ID(NULL_ENTITY)
		{
		}

		SceneEntity(const SceneEntity& other)
			:
			m_Scene(other.m_Scene),
			m_ID(other.m_ID)
		{}

		SceneEntity(uint32_t id, Scene* scene)
			:
			m_Scene(scene),
			m_ID(id)
		{
		}

		template<typename T>
		T& GetGroupComponent()
		{
			return m_Scene->m_ECS.GetGroupComponent<T>(m_ID);
		}

		template<typename T>
		const T& GetGroupComponent() const
		{
			return m_Scene->m_ECS.GetGroupComponent<T>(m_ID);
		}


		template<typename T>
		T& GetStorageComponent()
		{
			return m_Scene->m_ECS.GetStorageComponent<T>(m_ID);
		}

		template<typename T>
		const T& GetStorageComponent() const
		{
			return m_Scene->m_ECS.GetStorageComponent<T>(m_ID);
		}

		template<typename T>
		T& GetComponent()
		{
			return m_Scene->m_ECS.GetComponent<T>(m_ID);
		}

		template <typename T>
		const T& GetComponent() const
		{
			return m_Scene->m_ECS.GetComponent<T>(m_ID);
		}

		template <typename T>
		T& AddComponent(const T& component)
		{
			return m_Scene->m_ECS.AddComponent<T>(m_ID, component);
		}
		template <typename T>
		void RemoveComponent()
		{
			m_Scene->m_ECS.RemoveComponent<T>(m_ID);
		}

		template <typename T>
		bool HasComponent() const
		{
			return m_Scene->m_ECS.Contains<T>(m_ID);
		}

		void Destroy()
		{
			m_Scene->DestroyEntity(*this);
		}

		size_t NumberOfTypes() const { return m_Scene->m_ECS.GetNumberOfRegisteredComponentTypes(); }

		SceneEntity& operator =(const SceneEntity& other)
		{
			m_Scene = other.m_Scene;
			m_ID = other.m_ID;
			return *this;
		}

		bool operator ==(const SceneEntity& other) const
		{
			return (m_ID == other.m_ID && m_Scene == other.m_Scene);
		}

		operator bool() const
		{
			return m_Scene && m_ID != NULL_ENTITY;
		}

		operator uint32_t () const { return m_ID; }

	private:
		Scene*   m_Scene;
		uint32_t m_ID;


		friend class Scene;
		friend class ScriptEngine;
	};
}