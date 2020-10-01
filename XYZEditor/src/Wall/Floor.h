#pragma once

#include <XYZ.h>

#include "Wall.h"

struct Line
{
	glm::vec4 Color;
	glm::vec3 Start;
	glm::vec3 End;
};


struct TestLine
{
	glm::vec3 Point;
};

struct FloorNode
{
	FloorNode(const std::string& name, const glm::vec3& pos,bool traversed)
		: 
		DebugName(name),
		Position(pos),
		RightSideParentOffset(0),
		LeftSideParentOffset(0),
		RightSideChildOffset(0),
		LeftSideChildOffset(0),
		Traversed(traversed)
	{}
	std::string DebugName;
	glm::vec3 Position;

	glm::vec3 RightSideParentOffset;
	glm::vec3 LeftSideParentOffset;

	glm::vec3 RightSideChildOffset;
	glm::vec3 LeftSideChildOffset;


	bool Traversed;

	bool Generated = false;
	size_t Index = 0;
};

class Floor
{
public:
	Floor();

	void GenerateMeshTest();

	void SubmitToRenderer();

	uint16_t CreatePoint(const glm::vec3& point, const std::string& name);
	uint16_t CreatePointFromPoint(const glm::vec3& point, uint16_t parent, const std::string& name);



private:

	void generateMeshFromTree(const FloorNode& p1, FloorNode& p2, uint32_t indexOffset, float height, float thickness);

	void generateIntersectionMesh();

private:
	std::vector<glm::vec3> m_GeneratedPoints;
	std::vector<glm::mat4> m_GeneratedQuads;

	XYZ::Tree<FloorNode> m_FloorTree;
	

	uint32_t m_IndexOffset = 0;

	XYZ::Ref<XYZ::Mesh> m_Mesh;
	XYZ::Ref<XYZ::Mesh> m_IntersectionMesh;
	std::vector<Line> m_Lines;


	static constexpr unsigned int sc_DefaultNumberOfWallPoints = 4;
	static constexpr float sc_DefaultWallThickness = 2.0f;
	static constexpr float sc_DefaultWallHeight = 25.0f;
};