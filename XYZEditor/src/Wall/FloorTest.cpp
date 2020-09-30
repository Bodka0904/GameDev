#include "stdafx.h"
#include "FloorTest.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>



static glm::vec3 LineLineIntersection(const glm::vec3& startA, const glm::vec3& endA, const glm::vec3& startB, const glm::vec3& endB)
{
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


static glm::vec3 ResolveRightLeftIntersection(const FloorNode& leftNode, const FloorNode& rightNode, const glm::vec3& startPoint, float height, float thicknessLeft, float thicknessRight)
{
	auto begin = glm::vec3{ startPoint.x,startPoint.z,startPoint.y };
	auto& leftEnd = leftNode.Position;
	auto& rightEnd = rightNode.Position;

	auto topFirstPointLeft = glm::vec3(begin.x, begin.y + height, begin.z);
	auto topFirstPointRight = glm::vec3(begin.x, begin.y + height, begin.z);

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
		begin.z + leftSegmentNormal.z * thicknessLeft / 2.0f,
		0.0f
	);

	glm::vec3 rightEndPointT = glm::vec3(
		rightEnd.x + rightSegmentNormal.x * -thicknessRight / 2.0f,
		rightEnd.y + rightSegmentNormal.z * -thicknessRight / 2.0f,
		0.0f
	);

	glm::vec3 rightStartPointT = glm::vec3(
		begin.x + rightSegmentNormal.x * -thicknessRight / 2.0f,
		begin.z + rightSegmentNormal.z * -thicknessRight / 2.0f,
		0.0f
	);

	return LineLineIntersection(leftEndPointT, leftStartPointT, rightEndPointT, rightStartPointT);
}


namespace XYZ {
	FloorTest::FloorTest()
	{
		m_Mesh = XYZ::Ref<XYZ::Mesh>::Create();
		m_IntersectionMesh = XYZ::Ref<XYZ::Mesh>::Create();
	}
	void FloorTest::SubmitToRenderer()
	{
		XYZ::MeshRenderer::SubmitMesh(glm::mat4(1), m_Mesh);
		XYZ::MeshRenderer::SubmitMesh(glm::mat4(1), m_IntersectionMesh);
		for (auto& line : m_Lines)
		{
			XYZ::Renderer2D::SubmitLine(line.Start, line.End, line.Color);
		}
		
		for (size_t i  = 0; i < m_TestLines.size(); i += 2)
		{
			XYZ::Renderer2D::SubmitLine(m_TestLines[i].Point, m_TestLines[i + 1].Point, { 1,1,1,1 });
		}
		//for (auto& transform : m_GeneratedQuads)
		//{
		//	XYZ::Renderer2D::SubmitQuad(transform, { 0,0,1,1 }, 1, { 1, 0, 0, 1 });
		//}
	}
	size_t FloorTest::CreatePoint(const glm::vec3& point, const std::string& name)
	{
		return m_Graph.AddVertex(FloorNode{ name,point,false });
	}
	size_t FloorTest::CreatePointFromPoint(const glm::vec3& point, size_t parent, const std::string& name)
	{	
		size_t index = m_Graph.AddVertex(FloorNode{ name,point,false });
		m_Graph.AddEdge({ parent,index });
		return index;
	}
	void FloorTest::Connect(size_t parent, size_t child)
	{
		m_Graph.AddEdge({ parent,child });
	}

	void FloorTest::GenerateMesh()
	{
		m_Mesh->TextureID = 1;
		m_IntersectionMesh->TextureID = 1;

		m_Graph.Traverse([this](FloorNode& source, FloorNode& destination, FloorNode* next, FloorNode* previous) {

			std::cout << source.DebugName << " "  << " --> " << destination.DebugName << std::endl;
			auto& parent = source;
			auto& child = destination;

			if (parent.DebugName == "Eighth Point")
			{
				std::cout << std::endl;
			}
			
			if (!child.Traversed)
			{
				generateParentOffset(parent, child, 2.0f, 2.0f);
			}
			if (next)
			{
				glm::vec3 intersection = ResolveRightLeftIntersection(*next, child, parent.Position, 25.0f, 2.0f, 2.0f);

				if (!child.Traversed)
					child.LeftSideChildOffset = intersection;
				else
					parent.LeftSideParentOffset = intersection;

				if (!next->Traversed)
					next->RightSideChildOffset = intersection;
				else
					parent.RightSideParentOffset = intersection;

				parent.Traversed = true;
			}
			

			
		});


		m_Graph.Traverse([this](FloorNode& source, FloorNode& destination, FloorNode* next, FloorNode* previous) {
			
			generateMeshFromGraphTest(source, destination, m_IndexOffset, 25, 2);
			m_IndexOffset += 4;
		
		});	
	}

	void FloorTest::GenerateMeshTest()
	{
		m_Mesh->TextureID = 1;
		m_IntersectionMesh->TextureID = 1;	

		m_Graph.TraverseRecursive([this](FloorNode& source, FloorNode& destination, FloorNode* next, FloorNode* previous, bool end){
			
			auto& parent = source;
			auto& child = destination;
			
			
			std::cout << source.DebugName << "   " << destination.DebugName << std::endl;

			if (next && previous)
			{
				glm::vec3 intersection = ResolveRightLeftIntersection(*next, destination, source.Position, 25.0f, 2.0f, 2.0f);
				child.LeftSideChildOffset = intersection;
				intersection = ResolveRightLeftIntersection(destination, *previous, source.Position, 25.0f, 2.0f, 2.0f);
				child.RightSideChildOffset = intersection;		
	
				generateMeshPoints(source, destination, m_IndexOffset, 25.0f, 2.0f);
				if (end)
				{
					generateEndPoints(source, destination, m_IndexOffset, 25.0f, 2.0f);		
				}
				m_IndexOffset += 4;
			}
		});	
	}


	void FloorTest::generateMeshFromGraph(const FloorNode& p1,const FloorNode& p2, uint32_t indexOffset, float height, float thickness)
	{
		size_t meshQuadCount = m_Mesh->Vertices.size();
		auto& begin = p1.Position;
		auto& end = p2.Position;

		auto topFirstPoint = glm::vec3(begin.x, begin.z + height, begin.y);

		auto endPoint = glm::vec3(end.x, end.z, end.y);
		auto startPoint = glm::vec3(begin.x, begin.z, begin.y);

		glm::vec3 segmentNormal = NormalFromPoints(topFirstPoint, endPoint, startPoint);

		XYZ::Vertex vertex;
		vertex.Color = { 1,1,1,1 };
		vertex.TexCoord = { 0,0 };

	
		vertex.Position = glm::vec4(
			p2.LeftSideChildOffset.x,
			p2.LeftSideChildOffset.y,
			begin.z, 1.0f
		);
		vertex.TexCoord = { 0,0 };
		m_Mesh->Vertices.push_back(vertex);

		vertex.Position = glm::vec4(
			p2.RightSideChildOffset.x,
			p2.RightSideChildOffset.y,
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
		//m_GeneratedQuads.push_back(transform);
		transform = glm::translate(glm::mat4(1), p2.Position) * glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 0, 1)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 1));
		//m_GeneratedQuads.push_back(transform);


		m_Lines.push_back({ {1,1,1,1}, p1l,p2l });
		m_Lines.push_back({ {1,1,1,1}, p2l,p3l });
		m_Lines.push_back({ {1,1,1,1}, p3l,p4l });
		m_Lines.push_back({ {1,1,1,1}, p4l,p1l });
		m_Lines.push_back({ {1,1,1,1}, p1l,p3l });
		
	
	}
	void FloorTest::generateMeshFromGraphTest(const FloorNode& p1, const FloorNode& p2, uint32_t indexOffset, float height, float thickness)
	{
		size_t meshQuadCount = m_Mesh->Vertices.size();
		auto& begin = p1.Position;
		auto& end = p2.Position;

		auto topFirstPoint = glm::vec3(begin.x, begin.z + height, begin.y);

		auto endPoint = glm::vec3(end.x, end.z, end.y);
		auto startPoint = glm::vec3(begin.x, begin.z, begin.y);

		glm::vec3 segmentNormal = NormalFromPoints(topFirstPoint, endPoint, startPoint);

		XYZ::Vertex vertex;
		vertex.Color = { 1,1,1,1 };
		vertex.TexCoord = { 0,0 };


		vertex.Position = glm::vec4(
			p2.LeftSideChildOffset.x,
			p2.LeftSideChildOffset.y,
			begin.z, 1.0f
		);
		vertex.TexCoord = { 0,0 };
		m_Mesh->Vertices.push_back(vertex);

		vertex.Position = glm::vec4(
			p2.RightSideChildOffset.x,
			p2.RightSideChildOffset.y,
			begin.z, 1.0f
		);
		vertex.TexCoord = { 1,0 };
		m_Mesh->Vertices.push_back(vertex);


		vertex.Position = glm::vec4(
			p2.LeftSideParentOffset.x,
			p2.LeftSideParentOffset.y,
			end.z, 1.0f
		);
		vertex.TexCoord = { 1,1 };
		m_Mesh->Vertices.push_back(vertex);

		vertex.Position = glm::vec4(
			p2.RightSideParentOffset.x,
			p2.RightSideParentOffset.y,
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
		//m_GeneratedQuads.push_back(transform);
		transform = glm::translate(glm::mat4(1), p2.Position) * glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 0, 1)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 1));
		//m_GeneratedQuads.push_back(transform);


		m_Lines.push_back({ {1,1,1,1}, p1l,p2l });
		m_Lines.push_back({ {1,1,1,1}, p2l,p3l });
		m_Lines.push_back({ {1,1,1,1}, p3l,p4l });
		m_Lines.push_back({ {1,1,1,1}, p4l,p1l });
		m_Lines.push_back({ {1,1,1,1}, p1l,p3l });
	}
	void FloorTest::generateParentOffset(const FloorNode& p1, FloorNode& p2, float height, float thickness)
	{
		size_t meshQuadCount = m_Mesh->Vertices.size();
		auto& begin = p1.Position;
		auto& end = p2.Position;

		auto topFirstPoint = glm::vec3(begin.x, begin.z + height, begin.y);

		auto endPoint = glm::vec3(end.x, end.z, end.y);
		auto startPoint = glm::vec3(begin.x, begin.z, begin.y);

		glm::vec3 segmentNormal = NormalFromPoints(topFirstPoint, endPoint, startPoint);

		XYZ::Vertex vertex;
		vertex.Color = { 1,1,1,1 };
		vertex.TexCoord = { 0,0 };

		vertex.Position = glm::vec4(
			end.x + segmentNormal.x * thickness / 2.0f,
			end.y + segmentNormal.z * thickness / 2.0f,
			end.z, 1.0f
		);
		vertex.TexCoord = { 1,1 };
		p2.LeftSideParentOffset = vertex.Position;

		vertex.Position = glm::vec4(
			end.x + segmentNormal.x * -thickness / 2.0f,
			end.y + segmentNormal.z * -thickness / 2.0f,
			end.z, 1.0f
		);
		vertex.TexCoord = { 0,1 };
		p2.RightSideParentOffset = vertex.Position;
	}

	void FloorTest::generateMeshPoints(const FloorNode& p1, const FloorNode& p2, uint32_t indexOffset, float height, float thickness)
	{
		size_t meshQuadCount = m_Mesh->Vertices.size();
		auto& begin = p1.Position;
		auto& end = p2.Position;

		auto topFirstPoint = glm::vec3(begin.x, begin.z + height, begin.y);

		auto endPoint = glm::vec3(end.x, end.z, end.y);
		auto startPoint = glm::vec3(begin.x, begin.z, begin.y);

		glm::vec3 segmentNormal = NormalFromPoints(topFirstPoint, endPoint, startPoint);

		XYZ::Vertex vertex;
		vertex.Color = { 1,1,1,1 };
		vertex.TexCoord = { 0,0 };


		vertex.Position = glm::vec4(
			p2.LeftSideChildOffset.x,
			p2.LeftSideChildOffset.y,
			begin.z, 1.0f
		);
		vertex.TexCoord = { 1,1 };
		m_Mesh->Vertices.push_back(vertex);
		m_TestLines.push_back({ vertex.Position });

		vertex.Position = glm::vec4(
			p2.RightSideChildOffset.x,
			p2.RightSideChildOffset.y,
			begin.z, 1.0f
		);

		vertex.TexCoord = { 0,1 };
		m_Mesh->Vertices.push_back(vertex);
		m_TestLines.push_back({ vertex.Position });

		m_Mesh->Indices.push_back(m_IndexOffset + 0);
		m_Mesh->Indices.push_back(m_IndexOffset + 1);
		m_Mesh->Indices.push_back(m_IndexOffset + 2);
		m_Mesh->Indices.push_back(m_IndexOffset + 2);
		m_Mesh->Indices.push_back(m_IndexOffset + 3);
		m_Mesh->Indices.push_back(m_IndexOffset + 0);


		m_Lines.push_back({ {1,0,0,1}, p1.Position,p2.Position });	
	}

	void FloorTest::generateEndPoints(const FloorNode& p1, const FloorNode& p2, uint32_t indexOffset, float height, float thickness)
	{
		size_t meshQuadCount = m_Mesh->Vertices.size();
		auto& begin = p1.Position;
		auto& end = p2.Position;

		auto topFirstPoint = glm::vec3(begin.x, begin.z + height, begin.y);

		auto endPoint = glm::vec3(end.x, end.z, end.y);
		auto startPoint = glm::vec3(begin.x, begin.z, begin.y);

		glm::vec3 segmentNormal = NormalFromPoints(topFirstPoint, endPoint, startPoint);

		XYZ::Vertex vertex;
		vertex.Color = { 1,1,1,1 };
		vertex.TexCoord = { 0,0 };
		
		vertex.Position = glm::vec4(
			end.x + segmentNormal.x * thickness / 2.0f,
			end.y + segmentNormal.z * thickness / 2.0f,
			end.z, 1.0f
		);
		vertex.TexCoord = { 0,0 };
		m_Mesh->Vertices.push_back(vertex);
		m_TestLines.push_back({ vertex.Position });


		vertex.Position = glm::vec4(
			end.x + segmentNormal.x * -thickness / 2.0f,
			end.y + segmentNormal.z * -thickness / 2.0f,
			end.z, 1.0f
		);
		vertex.TexCoord = { 1,0 };
		m_Mesh->Vertices.push_back(vertex);
		m_TestLines.push_back({ vertex.Position });
		

	}
	
}