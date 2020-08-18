#pragma once
#include "Component.h"
#include "ComponentStorage.h"
#include "XYZ/Event/Event.h"


namespace XYZ {
	class SystemManager;

	/*! @class System
	* @brief Interface for systems
	*/
	class System
	{
		friend class SystemManager;
	public:
		/**
		* Add entity to the system
		* @param[in] entity
		*/
		virtual void Add(uint32_t entity) {};

		/**
		* Remove entity from the system
		* @param[in] entity
		*/
		virtual void Remove(uint32_t entity) {};
		
		/**
		* Remove/Add entity from the system
		* @param[in] entity
		*/
		virtual void EntityUpdated(uint32_t entity) {};
		

		/**
		* Check if system contains entity
		* @param[in] entity
		* @return 
		*/
		virtual bool Contains(uint32_t entity) = 0;

	protected:
		Signature m_Signature;

		struct Component
		{
			ActiveComponent* ActiveComponent;
			uint32_t Ent;

			bool operator()(const Component& a, const Component& b)
			{
				return (a.Ent < b.Ent);
			}
			bool operator ==(const uint32_t other) const
			{
				return Ent == other;
			}
			bool operator <(const uint32_t other) const
			{
				return Ent < other;
			}
			bool operator >(const uint32_t other) const
			{
				return Ent > other;
			}
		};
	protected:
		template <typename T>
		static uint16_t GetID()
		{
			static uint16_t compComponent = getNextID();
			return compComponent;
		}
	private:
		static uint16_t getNextID()
		{
			static uint16_t nextComponent = 0;
			return ++nextComponent;
		}
	};

}