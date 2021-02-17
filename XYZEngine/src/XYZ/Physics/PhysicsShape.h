#pragma once

#include "XYZ/Utils/Math/AABB.h"
#include <glm/glm.hpp>

namespace XYZ {

	enum class ShapeType
	{
		Box,
		Circle,
		Chain,
		NumTypes
	};

	class PhysicsBody;
	struct PhysicsShape
	{
		PhysicsShape(ShapeType type, PhysicsBody* body);

		virtual bool Intersect(const PhysicsShape& shape) const = 0;
		virtual AABB GetAABB() const = 0;
		virtual float CalculateMass(float density) const = 0;
		virtual float CalculateInertia(float mass) const = 0;
		virtual float CalculateTorque(const glm::vec2& force, const glm::vec2& pos) const = 0;
		virtual glm::vec2 CalculateCenter() const { return glm::vec2(0.0f); }

		const PhysicsBody* GetBody() const { return m_Body; }
		ShapeType GetType() const { return m_Type; };
		int32_t GetID() const { return m_ID; }
	private:
		const ShapeType m_Type;
		int32_t m_ID = -1;

	protected:
		glm::vec2 m_Offset = glm::vec2(0.0f);
		const PhysicsBody* m_Body;
		friend class PhysicsWorld;
	};

	struct BoxShape2D : public PhysicsShape
	{
		BoxShape2D(PhysicsBody* body);
		BoxShape2D(PhysicsBody* body, const glm::vec2& min, const glm::vec2& max);

		glm::vec2 Min;
		glm::vec2 Max;           

		virtual bool Intersect(const PhysicsShape& shape) const override;
		virtual AABB GetAABB() const override;
		virtual float CalculateMass(float density) const override;
		virtual float CalculateInertia(float mass) const override;
		virtual float CalculateTorque(const glm::vec2& force, const glm::vec2& pos) const override;
		virtual glm::vec2 CalculateCenter() const override;
	};

	struct CircleShape : public PhysicsShape
	{
		CircleShape(PhysicsBody* body);
		CircleShape(PhysicsBody* body, const glm::vec2& offset, float radius);

		virtual bool Intersect(const PhysicsShape& shape) const override;
		virtual AABB GetAABB() const override;
		virtual float CalculateMass(float density) const override;
		virtual float CalculateInertia(float mass) const override;
		virtual float CalculateTorque(const glm::vec2& force, const glm::vec2& pos) const override;

		float Radius;
	};

	struct Polygon : public PhysicsShape
	{

	};
}
