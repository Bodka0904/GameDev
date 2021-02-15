#pragma once
#include "XYZ/Core/MemoryPool.h"
#include "XYZ/Core/Timestep.h"
#include "DynamicTree.h"
#include "PhysicsBody.h"
#include "PhysicsShape.h"

#include <glm/glm.hpp>

namespace XYZ {

	class PhysicsWorld
	{
	public:
		PhysicsWorld(const glm::vec2& gravity);

		void Update(Timestep ts);

		PhysicsBody* CreateBody(const glm::vec2& position, float rotation);
		BoxShape2D* AddBox2DShape(PhysicsBody* body, const glm::vec2& min, const glm::vec2& max, float density);
		CircleShape* AddCircleShape(PhysicsBody* body, const glm::vec2& offset, float radius, float density);

	private:
		void broadPhase(Timestep ts);

	private:
		DynamicTree m_Tree;
		MemoryPool m_Pool;

		std::vector<PhysicsBody*> m_Bodies;

		glm::vec2 m_Gravity;

		std::vector<std::pair<int32_t, int32_t>> m_IntersectingNodes;
	};
}