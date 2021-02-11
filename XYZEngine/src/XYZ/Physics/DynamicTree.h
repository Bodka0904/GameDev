#pragma once
#include "XYZ/Utils/Math/AABB.h"
#include "XYZ/Utils/Math/Ray.h"
#include "XYZ/Utils/DataStructures/FreeList.h"
#include <stack>

namespace XYZ {

#define NULL_NODE (-1)
	struct Node
	{
		AABB Box;
		uint32_t ObjectIndex;

		int32_t ParentIndex = NULL_NODE;
		int32_t FirstChild = NULL_NODE;
		int32_t SecondChild = NULL_NODE;
		int32_t Height = 0;

		bool IsLeaf() const { return FirstChild == NULL_NODE; }
	};

	using CollisionCallback = std::function<bool(int32_t)>;

	class DynamicTree
	{
	public:
		bool RayCast(const Ray& ray, uint32_t& result);
		void Query(const CollisionCallback& callback, const AABB& aabb);

		int32_t Insert(uint32_t objectIndex, const AABB& box);
		void Move(int32_t index, const glm::vec2& displacement);
		void Remove(int32_t index);

	

		// Debug
		void SubmitToRenderer();

	private:
		void insertLeaf(int32_t index);
		void removeLeaf(int32_t leaf);
		int32_t balance(int32_t index);

	private:
		FreeList<Node> m_Nodes;

		int32_t m_RootIndex = NULL_NODE;
	};
}