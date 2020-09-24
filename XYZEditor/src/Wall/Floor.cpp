#include "Floor.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>



static glm::vec3 LineLineIntersection(const glm::vec3& startA, const glm::vec3& endA, const glm::vec3& startB, const glm::vec3& endB)
{
	auto text = startA;
	auto test = endA;
	auto testas = startB;
	auto gasf = endB;
	// Line AB represented as a1x + b1y = c1 
	float a1 = endA.y - startA.y;
	float b1 = startA.x - endA.x;
	float c1 = a1 * (startA.x) + b1 * (startA.y);

	// Line CD represented as a2x + b2y = c2 
	float a2 = endB.y - startB.y;
	float b2 = startB.x - endB.x;
	float c2 = a2 * (startB.x) + b2 * (startB.y);

	float determinant = a1 * b2 - a2 * b1;

	if (determinant == 0)
	{
		// The lines are parallel. This is simplified 
		// by returning a pair of FLT_MAX 
		return glm::vec3(FLT_MAX, 0, FLT_MAX);
	}
	else
	{
		float x = (b2 * c1 - b1 * c2) / determinant;
		float y = (a1 * c2 - a2 * c1) / determinant;
		return glm::vec3(x, y, 0);
	}
}

static glm::vec3 NormalFromPoints(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
{
	auto cross = glm::cross((p1 - p2), (p1 - p3));
	return glm::normalize(cross);
}


static std::pair<glm::vec3,glm::vec3> ResolveIntersection(const FloorNode& leftNode, const FloorNode& rightNode, const glm::vec3& startPoint, float height, float thicknessLeft, float thicknessRight)
{
	std::pair<glm::vec3, glm::vec3> result;
	{
		auto& begin = startPoint;
		auto& leftEnd = leftNode.Position;
		auto& rightEnd = rightNode.Position;

		auto topFirstPointLeft = glm::vec3(begin.x, begin.z + height, begin.y);
		auto topFirstPointRight = glm::vec3(begin.x, begin.z + height, begin.y);

		auto leftEndPoint = glm::vec3(leftEnd.x, leftEnd.z, leftEnd.y);
		auto rightEndPoint = glm::vec3(rightEnd.x, rightEnd.z, rightEnd.y);

		glm::vec3 leftSegmentNormal = NormalFromPoints(topFirstPointLeft, leftEndPoint, begin);
		glm::vec3 rightSegmentNormal = NormalFromPoints(topFirstPointRight, rightEndPoint, begin);

		glm::vec3 leftEndPointT = glm::vec3(
			leftEnd.x + leftSegmentNormal.x * thicknessLeft / 2.0f,
			leftEnd.y + leftSegmentNormal.z * thicknessLeft / 2.0f,
			0.0f
		);

		glm::vec3 leftStartPointT = glm::vec3(
			begin.x + leftSegmentNormal.x * thicknessLeft / 2.0f,
			begin.y + leftSegmentNormal.z * thicknessLeft / 2.0f,
			0.0f
		);

		glm::vec3 rightEndPointT = glm::vec3(
			rightEnd.x + rightSegmentNormal.x * -thicknessRight / 2.0f,
			rightEnd.y + rightSegmentNormal.z * -thicknessRight / 2.0f,
			0.0f
		);

		glm::vec3 rightStartPointT = glm::vec3(
			begin.x + rightSegmentNormal.x * -thicknessRight / 2.0f,
			begin.y + rightSegmentNormal.z * -thicknessRight / 2.0f,
			0.0f
		);
		result.first = LineLineIntersection(leftEndPointT, leftStartPointT, rightEndPointT, rightStartPointT);
	}

	{
		auto& begin = startPoint;
		auto& leftEnd = rightNode.Position;
		auto& rightEnd = leftNode.Position;

		auto topFirstPointLeft = glm::vec3(begin.x, begin.z + height, begin.y);
		auto topFirstPointRight = glm::vec3(begin.x, begin.z + height, begin.y);

		auto leftEndPoint = glm::vec3(leftEnd.x, leftEnd.z, leftEnd.y);
		auto rightEndPoint = glm::vec3(rightEnd.x, rightEnd.z, rightEnd.y);

		glm::vec3 leftSegmentNormal = NormalFromPoints(topFirstPointLeft, leftEndPoint, begin);
		glm::vec3 rightSegmentNormal = NormalFromPoints(topFirstPointRight, rightEndPoint, begin);

		glm::vec3 leftEndPointT = glm::vec3(
			leftEnd.x + leftSegmentNormal.x * thicknessLeft / 2.0f,
			leftEnd.y + leftSegmentNormal.z * thicknessLeft / 2.0f,
			0.0f
		);

		glm::vec3 leftStartPointT = glm::vec3(
			begin.x + leftSegmentNormal.x * thicknessLeft / 2.0f,
			begin.y + leftSegmentNormal.z * thicknessLeft / 2.0f,
			0.0f
		);

		glm::vec3 rightEndPointT = glm::vec3(
			rightEnd.x + rightSegmentNormal.x * -thicknessRight / 2.0f,
			rightEnd.y + rightSegmentNormal.z * -thicknessRight / 2.0f,
			0.0f
		);

		glm::vec3 rightStartPointT = glm::vec3(
			begin.x + rightSegmentNormal.x * -thicknessRight / 2.0f,
			begin.y + rightSegmentNormal.z * -thicknessRight / 2.0f,
			0.0f
		);
		result.second = LineLineIntersection(leftEndPointT, leftStartPointT, rightEndPointT, rightStartPointT);
	}
	
	return result;
}

static glm::vec3 ResolveRightLeftIntersection(const FloorNode& leftNode, const FloorNode& rightNode, const glm::vec3& startPoint, float height, float thicknessLeft, float thicknessRight)
{
	auto& begin = startPoint;
	auto& leftEnd = leftNode.Position;
	auto& rightEnd = rightNode.Position;

	auto topFirstPointLeft = glm::vec3(begin.x, begin.z + height, begin.y);
	auto topFirstPointRight = glm::vec3(begin.x, begin.z + height, begin.y);

	auto leftEndPoint = glm::vec3(leftEnd.x, leftEnd.z, leftEnd.y);
	auto rightEndPoint = glm::vec3(rightEnd.x, rightEnd.z, rightEnd.y);

	glm::vec3 leftSegmentNormal = NormalFromPoints(topFirstPointLeft, leftEndPoint, begin);
	glm::vec3 rightSegmentNormal = NormalFromPoints(topFirstPointRight, rightEndPoint, begin);

	glm::vec3 leftEndPointT = glm::vec3(
		leftEnd.x + leftSegmentNormal.x * thicknessLeft / 2.0f,
		leftEnd.y + leftSegmentNormal.z * thicknessLeft / 2.0f,
		0.0f
	);

	glm::vec3 leftStartPointT = glm::vec3(
		begin.x + leftSegmentNormal.x * thicknessLeft / 2.0f,
		begin.y + leftSegmentNormal.z * thicknessLeft / 2.0f,
		0.0f
	);

	glm::vec3 rightEndPointT = glm::vec3(
		rightEnd.x + rightSegmentNormal.x * -thicknessRight / 2.0f,
		rightEnd.y + rightSegmentNormal.z * -thicknessRight / 2.0f,
		0.0f
	);

	glm::vec3 rightStartPointT = glm::vec3(
		begin.x + rightSegmentNormal.x * -thicknessRight / 2.0f,
		begin.y + rightSegmentNormal.z * -thicknessRight / 2.0f,
		0.0f
	);

	return LineLineIntersection(leftEndPointT, leftStartPointT, rightEndPointT, rightStartPointT);
}


Floor::Floor()
{
	m_Mesh = XYZ::Ref<XYZ::Mesh>::Create();
	m_IntersectionMesh = XYZ::Ref<XYZ::Mesh>::Create();
}

int Floor::CreateWall()
{
	int index = m_Walls.Emplace(0, sc_DefaultWallThickness, sc_DefaultWallHeight);
	m_Walls[index].IndexInBuffer = index;
	return index;
}

void Floor::DeleteWall(int index)
{
	m_Walls.Erase(index);
}


void Floor::GenerateMesh()
{
	size_t lastVertexCount = m_Mesh->Vertices.size();
	size_t lastIndexCount = m_Mesh->Indices.size();
	size_t lastLineCount = m_Lines.size();
	
	m_Mesh->Vertices.clear();
	m_Mesh->Indices.clear();
	m_Lines.clear();

	m_Mesh->Vertices.reserve(lastVertexCount);
	m_Mesh->Indices.reserve(lastIndexCount);
	m_Lines.reserve(lastLineCount);
	m_Mesh->TextureID = 1;

	uint32_t offset = 0;
	for (int i = 0; i < m_Walls.Range(); ++i)
	{
		generateMeshFromWall(m_Walls[i], offset);
		offset += 4;
	}
}

void Floor::GenerateMeshFromTree()
{
	size_t lastVertexCount = m_Mesh->Vertices.size();
	size_t lastIndexCount = m_Mesh->Indices.size();
	size_t lastLineCount = m_Lines.size();

	m_Mesh->Vertices.clear();
	m_Mesh->Indices.clear();
	m_Lines.clear();

	m_Mesh->Vertices.reserve(lastVertexCount);
	m_Mesh->Indices.reserve(lastIndexCount);
	m_Lines.reserve(lastLineCount);
	m_Mesh->TextureID = 1;

	m_FloorTree.Propagate([this](XYZ::Node<FloorNode>* parent, XYZ::Node<FloorNode>* child) {
		if (parent)
		{
			generateMeshFromTree(parent->GetData().Position, child->GetData().Position, m_IndexOffset, 25, 2);	
		}
	}); 

	m_IndexOffset = 0;
}

void Floor::GenerateMeshTest()
{
	size_t lastVertexCount = m_Mesh->Vertices.size();
	size_t lastIndexCount = m_Mesh->Indices.size();
	size_t lastGeneratedQuads = m_GeneratedQuads.size();
	size_t lastLineCount = m_Lines.size();

	m_Mesh->Vertices.clear();
	m_Mesh->Indices.clear();
	m_GeneratedQuads.clear();
	m_Lines.clear();

	m_Mesh->Vertices.reserve(lastVertexCount);
	m_Mesh->Indices.reserve(lastIndexCount);
	m_GeneratedQuads.reserve(lastGeneratedQuads);
	m_Lines.reserve(lastLineCount);
	m_Mesh->TextureID = 1;
	m_IntersectionMesh->TextureID = 1;

	m_FloorTree.Propagate([this](XYZ::Node<FloorNode>* parent, XYZ::Node<FloorNode>* child) {
		if (parent)
		{
			if (!parent->GetData().Traversed)
			{
				XYZ::Vertex vertex;
				vertex.Color = { 1,1,1,1 };
				vertex.TexCoord = { 1,0 };
				vertex.Position = { parent->GetData().Position,1.0f };
				m_IntersectionMesh->Vertices.push_back(vertex);
				parent->GetData().Traversed = true;
			}
			if (child->GetNextSiblingIndex() != XYZ::Node<FloorNode>::GetInvalidIndex())
			{
				auto& nextSibling = m_FloorTree[child->GetNextSiblingIndex()];
				glm::vec3 intersection = ResolveRightLeftIntersection(nextSibling, *child, parent->GetData().Position, 25.0f, 2.0f, 2.0f);

				child->GetData().LeftSideOffset = intersection;
				nextSibling.RightSideOffset = intersection;
				
				XYZ::Vertex vertex;
				vertex.Color = { 1,1,1,1 };
				vertex.TexCoord = { 0,1 };
				vertex.Position = { intersection, 1.0f };

				m_IntersectionMesh->Vertices.push_back(vertex);
				
			}
			else if (child->HasPreviousSibling())
			{
				auto& firstChild = m_FloorTree[parent->GetFirstChildIndex()];
				glm::vec3 intersection = ResolveRightLeftIntersection(firstChild, *child, parent->GetData().Position, 25.0f, 2.0f, 2.0f);

				child->GetData().LeftSideOffset = intersection;
				firstChild.RightSideOffset = intersection;

				XYZ::Vertex vertex;
				vertex.Color = { 1,1,1,1 };
				vertex.TexCoord = { 1,0 };
				vertex.Position = { intersection, 1.0f };

				m_IntersectionMesh->Vertices.push_back(vertex);
			}
			else
			{
				// Does not work now, requires graph
				auto intersection = ResolveIntersection(*parent,*child, parent->GetData().Position, 25.0f, 2.0f, 2.0f);
				child->GetData().LeftSideOffset = intersection.first;
				parent->GetData().RightSideOffset = intersection.first;
				
				child->GetData().RightSideOffset = intersection.second;
				parent->GetData().LeftSideOffset = intersection.second;
			}
		}
	});


	m_FloorTree.Propagate([this](XYZ::Node<FloorNode>* parent, XYZ::Node<FloorNode>* child) {
		
		if (parent)
		{
			generateMeshFromTreeTest(parent->GetData(), child->GetData(), m_IndexOffset, 25, 2);
			m_IndexOffset += 4;
		}
		
	});
	
	generateIntersectionMesh();

	m_IndexOffset = 0;
}

void Floor::SubmitToRenderer()
{
	XYZ::MeshRenderer::SubmitMesh(glm::mat4(1), m_Mesh);
	XYZ::MeshRenderer::SubmitMesh(glm::mat4(1), m_IntersectionMesh);
	for (auto& line : m_Lines)
	{
		XYZ::Renderer2D::SubmitLine(line.Start, line.End, line.Color);
	}
	for (auto& transform : m_GeneratedQuads)
	{
		XYZ::Renderer2D::SubmitQuad(transform, { 0,0,1,1 }, 1, { 1, 0, 0, 1 });
	}
}

uint16_t Floor::CreatePoint(const glm::vec3& point, const std::string& name)
{
	uint16_t index = m_FloorTree.InsertNode(FloorNode{ name, point, false });
	if (m_FloorTree.GetSize() == 1)
		m_FloorTree.SetRoot(index);

	return index;
}

uint16_t Floor::CreatePointFromPoint(const glm::vec3& point, uint16_t parent, const std::string& name)
{
	uint16_t index = m_FloorTree.InsertNode(FloorNode{ name, point, false });
	m_FloorTree.SetParent(parent, index);
	return index;
}



std::pair<glm::vec3, glm::vec3> Floor::resolveWallCollisions(const Wall& wall)
{
	return std::pair<glm::vec3, glm::vec3>();
}

void Floor::generateMeshFromWall(const Wall& wall, uint32_t indexOffset)
{
	size_t meshQuadCount = m_Mesh->Vertices.size();
	auto& begin = wall.End[0];
	auto& end = wall.End[1];

	auto topFirstPoint = glm::vec3(begin.Position.x, begin.Position.y + wall.Height, begin.Position.z);

	auto endPoint = glm::vec3(end.Position.x, end.Position.z, end.Position.y);
	auto startPoint = glm::vec3(begin.Position.x, begin.Position.z, begin.Position.y);

	glm::vec3 segmentNormal = NormalFromPoints(topFirstPoint, endPoint, startPoint);

	XYZ::Vertex vertex;
	vertex.Color = { 1,1,1,1 };
	vertex.TexCoord = { 0,0 };

	vertex.Position = glm::vec4(
		begin.Position.x + segmentNormal.x * -wall.Thickness / 2.0f, 
		begin.Position.y + segmentNormal.z * -wall.Thickness / 2.0f, 
		begin.Position.z, 1.0f
	);
	vertex.TexCoord = { 0,0 };
	m_Mesh->Vertices.push_back(vertex);

	vertex.Position = glm::vec4(
		begin.Position.x + segmentNormal.x * wall.Thickness / 2.0f, 
		begin.Position.y + segmentNormal.z * wall.Thickness / 2.0f, 
		begin.Position.z, 1.0f
	);
	vertex.TexCoord = { 1,0 };
	m_Mesh->Vertices.push_back(vertex);

	vertex.Position = glm::vec4(
		end.Position.x + segmentNormal.x * wall.Thickness / 2.0f, 
		end.Position.y + segmentNormal.z * wall.Thickness / 2.0f, 
		end.Position.z, 1.0f
	);
	vertex.TexCoord = { 1,1 };
	m_Mesh->Vertices.push_back(vertex);

	vertex.Position = glm::vec4(
		end.Position.x + segmentNormal.x * -wall.Thickness / 2.0f, 
		end.Position.y + segmentNormal.z * -wall.Thickness / 2.0f, 
		end.Position.z, 1.0f
	);
	vertex.TexCoord = { 0,1 };
	m_Mesh->Vertices.push_back(vertex);


	m_Mesh->Indices.push_back(indexOffset + 0);
	m_Mesh->Indices.push_back(indexOffset + 1);
	m_Mesh->Indices.push_back(indexOffset + 2);
	m_Mesh->Indices.push_back(indexOffset + 2);
	m_Mesh->Indices.push_back(indexOffset + 3);
	m_Mesh->Indices.push_back(indexOffset + 0);
	


	// Debug rendering
	auto& vertices = m_Mesh->Vertices;
	glm::vec3 p1 = { vertices[meshQuadCount].Position.x,vertices[meshQuadCount].Position.y,vertices[meshQuadCount].Position.z };
	glm::vec3 p2 = { vertices[meshQuadCount + 1].Position.x,vertices[meshQuadCount + 1].Position.y,vertices[meshQuadCount + 1].Position.z };
	glm::vec3 p3 = { vertices[meshQuadCount + 2].Position.x,vertices[meshQuadCount + 2].Position.y,vertices[meshQuadCount + 2].Position.z };
	glm::vec3 p4 = { vertices[meshQuadCount + 3].Position.x,vertices[meshQuadCount + 3].Position.y,vertices[meshQuadCount + 3].Position.z };
	
	m_Lines.push_back({{1,1,1,1}, p1,p2 });
	m_Lines.push_back({{1,1,1,1}, p2,p3 });
	m_Lines.push_back({{1,1,1,1}, p3,p4 });
	m_Lines.push_back({{1,1,1,1}, p4,p1 });
	m_Lines.push_back({{1,1,1,1}, p1,p3 });
}

void Floor::generateMeshFromTree(const glm::vec3& p1, const glm::vec3& p2, uint32_t indexOffset, float height, float thickness)
{
	size_t meshQuadCount = m_Mesh->Vertices.size();
	auto& begin = p1;
	auto& end = p2;

	auto topFirstPoint = glm::vec3(begin.x, begin.y + height, begin.z);

	auto endPoint = glm::vec3(end.x, end.z, end.y);
	auto startPoint = glm::vec3(begin.x, begin.z, begin.y);

	glm::vec3 segmentNormal = NormalFromPoints(topFirstPoint, endPoint, startPoint);

	XYZ::Vertex vertex;
	vertex.Color = { 1,1,1,1 };
	vertex.TexCoord = { 0,0 };

	vertex.Position = glm::vec4(
		begin.x + segmentNormal.x * -thickness / 2.0f,
		begin.y + segmentNormal.z * -thickness / 2.0f,
		begin.z, 1.0f
	);
	vertex.TexCoord = { 0,0 };
	m_Mesh->Vertices.push_back(vertex);

	vertex.Position = glm::vec4(
		begin.x + segmentNormal.x * thickness / 2.0f,
		begin.y + segmentNormal.z * thickness / 2.0f,
		begin.z, 1.0f
	);
	vertex.TexCoord = { 1,0 };
	m_Mesh->Vertices.push_back(vertex);

	vertex.Position = glm::vec4(
		end.x + segmentNormal.x * thickness / 2.0f,
		end.y + segmentNormal.z * thickness / 2.0f,
		end.z, 1.0f
	);
	vertex.TexCoord = { 1,1 };
	m_Mesh->Vertices.push_back(vertex);

	vertex.Position = glm::vec4(
		end.x + segmentNormal.x * -thickness / 2.0f,
		end.y + segmentNormal.z * -thickness / 2.0f,
		end.z, 1.0f
	);
	vertex.TexCoord = { 0,1 };
	m_Mesh->Vertices.push_back(vertex);


	m_Mesh->Indices.push_back(indexOffset + 0);
	m_Mesh->Indices.push_back(indexOffset + 1);
	m_Mesh->Indices.push_back(indexOffset + 2);
	m_Mesh->Indices.push_back(indexOffset + 2);
	m_Mesh->Indices.push_back(indexOffset + 3);
	m_Mesh->Indices.push_back(indexOffset + 0);



	// Debug rendering
	auto& vertices = m_Mesh->Vertices;
	glm::vec3 p1l = { vertices[meshQuadCount].Position.x,vertices[meshQuadCount].Position.y,vertices[meshQuadCount].Position.z };
	glm::vec3 p2l = { vertices[meshQuadCount + 1].Position.x,vertices[meshQuadCount + 1].Position.y,vertices[meshQuadCount + 1].Position.z };
	glm::vec3 p3l = { vertices[meshQuadCount + 2].Position.x,vertices[meshQuadCount + 2].Position.y,vertices[meshQuadCount + 2].Position.z };
	glm::vec3 p4l = { vertices[meshQuadCount + 3].Position.x,vertices[meshQuadCount + 3].Position.y,vertices[meshQuadCount + 3].Position.z };

	m_Lines.push_back({ {1,0,0,1}, p1,p2 });

	m_Lines.push_back({{1,1,1,1}, p1l,p2l });
	m_Lines.push_back({{1,1,1,1}, p2l,p3l });
	m_Lines.push_back({{1,1,1,1}, p3l,p4l });
	m_Lines.push_back({{1,1,1,1}, p4l,p1l });
	m_Lines.push_back({{1,1,1,1}, p1l,p3l });
}

void Floor::generateMeshFromTreeTest(const FloorNode& p1, const FloorNode& p2, uint32_t indexOffset, float height, float thickness)
{
	size_t meshQuadCount = m_Mesh->Vertices.size();
	auto& begin = p1.Position;
	auto& end = p2.Position;

	auto topFirstPoint = glm::vec3(begin.x, begin.y + height, begin.z);

	auto endPoint = glm::vec3(end.x, end.z, end.y);
	auto startPoint = glm::vec3(begin.x, begin.z, begin.y);

	glm::vec3 segmentNormal = NormalFromPoints(topFirstPoint, endPoint, startPoint);

	XYZ::Vertex vertex;
	vertex.Color = { 1,1,1,1 };
	vertex.TexCoord = { 0,0 };

	vertex.Position = glm::vec4(
		p2.LeftSideOffset.x,
		p2.LeftSideOffset.y,
		begin.z, 1.0f
	);
	vertex.TexCoord = { 0,0 };
	m_Mesh->Vertices.push_back(vertex);

	vertex.Position = glm::vec4(
		p2.RightSideOffset.x,
		p2.RightSideOffset.y,
		begin.z, 1.0f
	);
	vertex.TexCoord = { 1,0 };
	m_Mesh->Vertices.push_back(vertex);

	vertex.Position = glm::vec4(
		end.x + segmentNormal.x * thickness / 2.0f,
		end.y + segmentNormal.z * thickness / 2.0f,
		end.z, 1.0f
	);
	vertex.TexCoord = { 1,1 };
	m_Mesh->Vertices.push_back(vertex);

	vertex.Position = glm::vec4(
		end.x + segmentNormal.x * -thickness / 2.0f,
		end.y + segmentNormal.z * -thickness / 2.0f,
		end.z, 1.0f
	);
	vertex.TexCoord = { 0,1 };
	m_Mesh->Vertices.push_back(vertex);


	m_Mesh->Indices.push_back(indexOffset + 0);
	m_Mesh->Indices.push_back(indexOffset + 1);
	m_Mesh->Indices.push_back(indexOffset + 2);
	m_Mesh->Indices.push_back(indexOffset + 2);
	m_Mesh->Indices.push_back(indexOffset + 3);
	m_Mesh->Indices.push_back(indexOffset + 0);



	// Debug rendering
	auto& vertices = m_Mesh->Vertices;
	glm::vec3 p1l = { vertices[meshQuadCount].Position.x,vertices[meshQuadCount].Position.y,vertices[meshQuadCount].Position.z };
	glm::vec3 p2l = { vertices[meshQuadCount + 1].Position.x,vertices[meshQuadCount + 1].Position.y,vertices[meshQuadCount + 1].Position.z };
	glm::vec3 p3l = { vertices[meshQuadCount + 2].Position.x,vertices[meshQuadCount + 2].Position.y,vertices[meshQuadCount + 2].Position.z };
	glm::vec3 p4l = { vertices[meshQuadCount + 3].Position.x,vertices[meshQuadCount + 3].Position.y,vertices[meshQuadCount + 3].Position.z };

	

	m_Lines.push_back({ {1,0,0,1}, p1.Position,p2.Position });
	glm::mat4 transform = glm::translate(glm::mat4(1), p1.Position) * glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 0, 1)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 1));
	m_GeneratedQuads.push_back(transform);
	transform = glm::translate(glm::mat4(1), p2.Position) * glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 0, 1)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 1));
	m_GeneratedQuads.push_back(transform);


	m_Lines.push_back({ {1,1,1,1}, p1l,p2l });
	m_Lines.push_back({ {1,1,1,1}, p2l,p3l });
	m_Lines.push_back({ {1,1,1,1}, p3l,p4l });
	m_Lines.push_back({ {1,1,1,1}, p4l,p1l });
	m_Lines.push_back({ {1,1,1,1}, p1l,p3l });
}

void Floor::generateIntersectionMesh()
{
	uint32_t count = 1;
	for (auto& vertex : m_IntersectionMesh->Vertices)
	{
		m_IntersectionMesh->Indices.push_back(m_IndexOffset);
		m_IntersectionMesh->Indices.push_back(m_IndexOffset + count);
		m_IntersectionMesh->Indices.push_back(m_IndexOffset + count + 1);
		count++;
		if (count == 5)
			break;
	}
	m_IntersectionMesh->Indices.push_back(m_IndexOffset);
	m_IntersectionMesh->Indices.push_back(m_IndexOffset + count);
	m_IntersectionMesh->Indices.push_back(m_IndexOffset + 1);
}
