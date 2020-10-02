#pragma once

#include <XYZ.h>


namespace XYZ {


	struct Line
	{
		glm::vec4 Color;
		glm::vec3 Point;
	};

	struct FloorNode
	{
		FloorNode(const std::string& name, const glm::vec3& pos)
			:
			DebugName(name),
			Position(pos)
		{}

		std::string DebugName;

		glm::vec3 Position;
	};

	struct Data
	{
		Vertex Points[2];
		bool Traversed = false;
	};

	class FloorTest
	{
	public:
		FloorTest();

		void SubmitToRenderer();

		size_t CreatePoint(const glm::vec3& point, const std::string& name);
		size_t CreatePointFromPoint(const glm::vec3& point, size_t parent, const std::string& name);

		void SetPointPosition(const glm::vec3& position, size_t index);

		void Connect(size_t parent, size_t child);
		void Disconnect(size_t parent, size_t child);
		
		bool GetPoint(const glm::vec2& position, size_t& point);

		// Recurisve
		void GenerateMesh();
	private:
		void generatePoints(const glm::vec3& leftIntersection, const glm::vec3& rightIntersection, size_t parentIndex, size_t childIndex);
		void generateEndPoints(const FloorNode& p1, const FloorNode& p2, uint32_t parentIndex, uint32_t childIndex, float height, float thickness);
	
	
	private:
		uint32_t m_IndexOffset = 0;

		Graph<FloorNode, true> m_Graph;

		std::vector<Data> m_RenderData;


		XYZ::Ref<XYZ::Mesh> m_Mesh;
		XYZ::Ref<XYZ::Mesh> m_IntersectionMesh;

		std::vector<Line> m_Lines;

		static constexpr unsigned int sc_DefaultNumberOfWallPoints = 4;
		static constexpr float sc_DefaultWallThickness = 2.0f;
		static constexpr float sc_DefaultWallHeight = 25.0f;

		static constexpr size_t sc_InvalidParent = 2000000; // TODO: fix with normal value
	};
}