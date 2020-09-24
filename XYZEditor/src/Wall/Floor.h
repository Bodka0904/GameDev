#pragma once

#include <XYZ.h>

#include "Wall.h"

struct Line
{
	glm::vec4 Color;
	glm::vec3 Start;
	glm::vec3 End;
};

struct FloorNode
{
	FloorNode(const std::string& name, const glm::vec3& pos,bool traversed)
		: 
		DebugName(name),
		Position(pos),
		RightSideOffset(0),
		LeftSideOffset(0),
		Traversed(traversed)
	{}
	std::string DebugName;
	glm::vec3 Position;
	glm::vec3 RightSideOffset;
	glm::vec3 LeftSideOffset;
	bool Traversed;
};

class Floor
{
public:
	Floor();

	int CreateWall();
	void DeleteWall(int index);
	void GenerateMesh();
	void GenerateMeshFromTree();
	void GenerateMeshTest();

	void SubmitToRenderer();

	uint16_t CreatePoint(const glm::vec3& point, const std::string& name);
	uint16_t CreatePointFromPoint(const glm::vec3& point, uint16_t parent, const std::string& name);

	const Wall& GetWall(int index) const { return m_Walls[index]; }
	Wall& GetWall(int index) { return m_Walls[index]; }

private:
	std::pair<glm::vec3, glm::vec3> resolveWallCollisions(const Wall& wall);
	void generateMeshFromWall(const Wall& wall, uint32_t indexOffset);
	void generateMeshFromTree(const glm::vec3& p1, const glm::vec3& p2, uint32_t indexOffset, float height, float thickness);
	void generateMeshFromTreeTest(const FloorNode& p1, const FloorNode& p2, uint32_t indexOffset, float height, float thickness);
private:
	XYZ::FreeList<Wall> m_Walls;
	std::vector<glm::vec3> m_GeneratedPoints;

	XYZ::Tree<FloorNode> m_FloorTree;
	uint32_t m_IndexOffset = 0;

	XYZ::Ref<XYZ::Mesh> m_Mesh;
	std::vector<Line> m_Lines;

	static constexpr unsigned int sc_DefaultNumberOfWallPoints = 4;
	static constexpr float sc_DefaultWallThickness = 2.0f;
	static constexpr float sc_DefaultWallHeight = 25.0f;
};