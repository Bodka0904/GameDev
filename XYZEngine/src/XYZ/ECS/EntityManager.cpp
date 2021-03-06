#include "stdafx.h"
#include "EntityManager.h"

namespace XYZ {
	EntityManager::EntityManager()
		:
		m_EntitiesInUse(0)
	{
	}
	uint32_t EntityManager::CreateEntity()
	{
		m_EntitiesInUse++;
		XYZ_ASSERT(m_EntitiesInUse < MAX_ENTITIES, "Too many entities in existence.");
		uint32_t entity = m_Signatures.Insert(Signature(0));
		if (m_Valid.size() <= entity)
			m_Valid.resize((size_t)entity + 1);
		m_Valid[entity] = true;
		return entity;		
	}
	Signature& EntityManager::GetSignature(uint32_t entity)
	{
		XYZ_ASSERT(entity < MAX_ENTITIES, "Entity out of range.");
		return m_Signatures[entity];
	}
	const Signature& EntityManager::GetSignature(uint32_t entity) const
	{
		XYZ_ASSERT(entity < MAX_ENTITIES, "Entity out of range.");
		return m_Signatures[entity];
	}
	void EntityManager::DestroyEntity(uint32_t entity)
	{
		XYZ_ASSERT(entity < MAX_ENTITIES, "Entity out of range.");

		// Put the destroyed ID at the back of the queue
		//Restart bitset to zero;
		m_Valid[entity] = false;
		m_Signatures.Erase(entity);
		m_EntitiesInUse--;
	}
	void EntityManager::SetSignature(uint32_t entity, Signature signature)
	{
		XYZ_ASSERT(entity < MAX_ENTITIES, "Entity out of range.");

		// Put this entity's signature into the array
		m_Signatures[entity] = signature;
	}
}