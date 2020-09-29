#pragma once

#include <XYZ.h>

#include "Floor.h"

namespace XYZ {

	class FloorTest
	{
	public:
		FloorTest();

		void SubmitToRenderer();

		size_t CreatePoint(const glm::vec3& point, const std::string& name);
		size_t CreatePointFromPoint(const glm::vec3& point, size_t parent, const std::string& name);
		void Connect(size_t parent, size_t child);

		void GenerateMesh();
		void GenerateMeshTest();

	private:
		void generateMeshFromGraph(const FloorNode& p1,const FloorNode& p2, uint32_t indexOffset, float height, float thickness);
		void generateMeshFromGraphTest(const FloorNode& p1, const FloorNode& p2, uint32_t indexOffset, float height, float thickness);

		void generateParentOffset(const FloorNode& p1, FloorNode& p2,  float height, float thickness);

		void adjustNodeEnd(const FloorNode& p1, FloorNode& p2, float height, float thickness);

	private:
		uint32_t m_IndexOffset = 0;

		Graph<FloorNode> m_Graph;

		XYZ::Ref<XYZ::Mesh> m_Mesh;
		XYZ::Ref<XYZ::Mesh> m_IntersectionMesh;
		std::vector<Line> m_Lines;

		static constexpr unsigned int sc_DefaultNumberOfWallPoints = 4;
		static constexpr float sc_DefaultWallThickness = 2.0f;
		static constexpr float sc_DefaultWallHeight = 25.0f;

		static constexpr size_t sc_InvalidParent = 2000000; // TODO: fix with normal value
	};
}