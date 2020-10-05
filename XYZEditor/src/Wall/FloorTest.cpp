#include "stdafx.h"
#include "FloorTest.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace XYZ{
	static bool Collide(const glm::vec2& pointPos, const glm::vec2& mousePos)
	{
			return (
			mousePos.x > pointPos.x - 0.5f / 2 && mousePos.x < pointPos.x + 0.5f / 2 &&
			mousePos.y > pointPos.y - 0.5f / 2 && mousePos.y < pointPos.y + 0.5f / 2
				);
	}

	

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



	FloorTest::FloorTest()
	{
		m_Mesh = XYZ::Ref<XYZ::Mesh>::Create();
		m_IntersectionMesh = XYZ::Ref<XYZ::Mesh>::Create();
	}
	void FloorTest::SubmitToRenderer()
	{
		XYZ::MeshRenderer::SubmitMesh(glm::mat4(1), m_Mesh);
		XYZ::MeshRenderer::SubmitMesh(glm::mat4(1), m_IntersectionMesh);
	
		for (auto& transform : m_Quads)
		{
			Renderer2D::SubmitQuad(transform, { 0,0,1,1 }, 1, { 1, 0, 0, 1 });
		}
		
		for (size_t i  = 0; i < m_Lines.size(); i += 2)
		{
			XYZ::Renderer2D::SubmitLine(m_Lines[i].Point, m_Lines[i + 1].Point, m_Lines[i].Color);
		}
	}
	int FloorTest::CreatePoint(const glm::vec3& point, const std::string& name)
	{
		int index = m_Graph.AddVertex(FloorNode{ name,point });
		m_RenderData.resize((size_t)m_Graph.GetData().Range() * (size_t)m_Graph.GetData().Range());
		return index;
	}
	int FloorTest::CreatePointFromPoint(const glm::vec3& point, size_t parent, const std::string& name)
	{	
		int index = m_Graph.AddVertex(FloorNode{ name,point });
		m_Graph.AddEdge({ parent,(size_t)index });
		m_RenderData.resize((size_t)m_Graph.GetData().Range() * (size_t)m_Graph.GetData().Range());
		return index;
	}
	void FloorTest::DestroyPoint(size_t index)
	{
		m_Graph.RemoveData(index);
	}
	void FloorTest::SetPointPosition(const glm::vec3& position, size_t index)
	{
		m_Graph.GetVertex(index).Data.Position = position;
	}
	void FloorTest::Connect(size_t parent, size_t child)
	{
		m_Graph.AddEdge({ parent,child });
	}

	void FloorTest::Disconnect(size_t parent, size_t child)
	{
		m_Graph.RemoveEdge({ parent,child });
	}

	bool FloorTest::GetPoint(const glm::vec2& position, size_t& point)
	{		
		bool found = false;
		m_Graph.Traverse([&](GraphVertex<FloorNode>& source, GraphVertex<FloorNode>& destination, FloorNode* next, FloorNode* previous) {

			if (point == source.Index || point == destination.Index)
			{}
			else if (Collide(source.Data.Position, position) && !found)
			{
				point = source.Index;
				found = true;
				return;
			}
			else if (Collide(destination.Data.Position, position) && !found)
			{
				point = destination.Index;
				found = true;
				return;
			}
		});
		
		return found;
	}

	void FloorTest::GenerateMesh()
	{
		m_Mesh->TextureID = 1;
		m_IntersectionMesh->TextureID = 1;
		m_Mesh->Vertices.clear();
		m_Mesh->Indices.clear();

		m_Quads.clear();
		m_Lines.clear();
		m_IndexOffset = 0;

		auto& list = m_Graph.GetAdjList();
		for (size_t parent = 0; parent < list.size(); ++parent)
		{
			if (!list[parent].empty())
			{
				glm::vec2 start = m_Graph.GetVertex(parent).Data.Position;
				glm::vec2 end = m_Graph.GetVertex(list[parent][0]).Data.Position;
				glm::vec2 defaultV = end - start;
				std::sort(list[parent].begin(), list[parent].end(), [&](size_t a, size_t b) {

					auto& p1 = m_Graph.GetVertex(parent).Data.Position;
					auto& p00 = m_Graph.GetVertex(a).Data.Position;
					auto& p01 = m_Graph.GetVertex(b).Data.Position;

					glm::vec2 dir0 = p1 - p00;
					glm::vec2 dir1 = p1 - p01;

					float angle0 = 0.0f;
					{
						float dot = dir0.x * defaultV.x + dir0.y * defaultV.y;
						float det = dir0.x * defaultV.y - dir0.y * defaultV.x;
						angle0 = atan2(det, dot);
					}
					float angle1 = 0.0f;
					{
						float dot = dir1.x * defaultV.x + dir1.y * defaultV.y;
						float det = dir1.x * defaultV.y - dir1.y * defaultV.x;
						angle1 = atan2(det, dot);
					}

					return angle0 > angle1;
				});
			}
		}


		
		m_Graph.TraverseRecursive([&](FloorNode& source, FloorNode& destination,size_t parentIndex, size_t childIndex, FloorNode* next, FloorNode* previous, bool end){
			
			auto& parent = source;
			auto& child = destination;	
			
			m_Lines.push_back({ {1,0,0,1}, source.Position });
			m_Lines.push_back({ {1,0,0,1}, destination.Position });

			auto& transform = glm::translate(glm::mat4(1), glm::vec3(source.Position)) * glm::rotate(glm::mat4(1), 0.0f, glm::vec3(0, 0, 1)) * glm::scale(glm::mat4(1), glm::vec3(0.5f, 0.5f, 1.0f));
			m_Quads.push_back(transform);
			transform = glm::translate(glm::mat4(1), glm::vec3(destination.Position)) * glm::rotate(glm::mat4(1), 0.0f, glm::vec3(0, 0, 1)) * glm::scale(glm::mat4(1), glm::vec3(0.5f, 0.5f, 1.0f));
			m_Quads.push_back(transform);

			if (next && previous)
			{
				glm::vec3 leftIntersection = ResolveRightLeftIntersection(*next, destination, source.Position, 25.0f, 2.0f, 2.0f);		
				glm::vec3 rightIntersection = ResolveRightLeftIntersection(destination, *previous, source.Position, 25.0f, 2.0f, 2.0f);

				if (!end) 
					generatePoints(leftIntersection, rightIntersection, parentIndex, childIndex);
				else
					generateEndPoints(destination, source, parentIndex, childIndex, 25.0f, 2.0f);
			}
			else
			{
				generateEndPoints(destination, source, parentIndex, childIndex, 25.0f, 2.0f);
			}
		});	


		m_Graph.TraverseRecursive([&](FloorNode& source, FloorNode& destination, size_t parentIndex, size_t childIndex, FloorNode* next, FloorNode* previous, bool end) {

			if (parentIndex > childIndex)
			{
				size_t tmp = parentIndex;
				parentIndex = childIndex;
				childIndex = tmp;
			}

			auto& current = m_RenderData[m_Graph.GetData().Range() * parentIndex + childIndex];
			//current = m_Data[parentIndex][childIndex];
			if (!current.Traversed)
			{
				auto& swapped = m_RenderData[m_Graph.GetData().Range() * childIndex + parentIndex];

		
				m_Mesh->Vertices.push_back(swapped.Points[0]);
				m_Mesh->Vertices.push_back(swapped.Points[1]);
	
				m_Mesh->Vertices.push_back(current.Points[0]);
				m_Mesh->Vertices.push_back(current.Points[1]);

				m_Mesh->Indices.push_back(m_IndexOffset + 0);
				m_Mesh->Indices.push_back(m_IndexOffset + 1);
				m_Mesh->Indices.push_back(m_IndexOffset + 2);
				m_Mesh->Indices.push_back(m_IndexOffset + 2);
				m_Mesh->Indices.push_back(m_IndexOffset + 3);
				m_Mesh->Indices.push_back(m_IndexOffset + 0);
				m_IndexOffset += 4;

				current.Traversed = true;
				swapped.Traversed = true;
			}
		});
	}


	void FloorTest::generatePoints(const glm::vec3& leftIntersection, const glm::vec3& rightIntersection, size_t parentIndex, size_t childIndex)
	{
		XYZ::Vertex vertex;
		vertex.Color = { 1,1,1,1 };
		vertex.TexCoord = { 0,0 };


		vertex.Position = glm::vec4(
			leftIntersection.x,
			leftIntersection.y,
			0.0f, 
			1.0f
		);
		vertex.TexCoord = { 1,1 };
		m_RenderData[(size_t)m_Graph.GetData().Range() * parentIndex + childIndex].Points[0] = vertex;
		m_RenderData[(size_t)m_Graph.GetData().Range() * parentIndex + childIndex].Traversed = false;


		m_Lines.push_back({ {1,1,1,1}, vertex.Position });
		vertex.Position = glm::vec4(
			rightIntersection.x,
			rightIntersection.y,
			0.0f,
			1.0f
		);
		vertex.TexCoord = { 0,1 };
		m_RenderData[(size_t)m_Graph.GetData().Range() * parentIndex + childIndex].Points[1] = vertex;
		m_RenderData[(size_t)m_Graph.GetData().Range() * parentIndex + childIndex].Traversed = false;


		m_Lines.push_back({ {1,1,1,1}, vertex.Position });
	}
	void FloorTest::generateEndPoints(const FloorNode& p1, const FloorNode& p2, uint32_t parentIndex, uint32_t childIndex, float height, float thickness)
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
		m_RenderData[(size_t)m_Graph.GetData().Range() * parentIndex + childIndex].Points[0] = vertex;
		m_RenderData[(size_t)m_Graph.GetData().Range() * parentIndex + childIndex].Traversed = false;

		m_Lines.push_back({ {1,1,1,1}, vertex.Position });


		vertex.Position = glm::vec4(
			end.x + segmentNormal.x * -thickness / 2.0f,
			end.y + segmentNormal.z * -thickness / 2.0f,
			end.z, 1.0f
		);
		vertex.TexCoord = { 1,0 };
		m_RenderData[(size_t)m_Graph.GetData().Range() * parentIndex + childIndex].Points[1] = vertex;
		m_RenderData[(size_t)m_Graph.GetData().Range() * parentIndex + childIndex].Traversed = false;

	

		m_Lines.push_back({ {1,1,1,1}, vertex.Position });

	}
	
}