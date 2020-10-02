#pragma once
#include "FreeList.h"


namespace XYZ {


	// data structure to store graph edges

	struct Edge 
	{
		size_t Source;
		size_t Destination;
	};


	template <typename T>
	struct GraphVertex
	{
		GraphVertex(const T& data, size_t index)
			:
			Data(data), Index(index)
		{}

		T Data;
		size_t Index;
	};

	// class to represent a graph object
	template <typename T, bool DoubleSidedConnection>
	class Graph
	{
	public:
		size_t AddVertex(const T& data)
		{
			GraphVertex<T> vertex(data, 0);
			int index = m_Data.Insert(vertex);
			vertex.Index = index;

			if (m_Data.Range() >= m_AdjList.size())
				m_AdjList.resize(m_Data.Range());

			return index;
		}

		void AddEdge(const Edge& edge)
		{
			if (edge.Source >= m_AdjList.size())
				m_AdjList.resize(edge.Source);
			if (edge.Destination >= m_AdjList.size())
				m_AdjList.resize(edge.Destination);

			m_AdjList[edge.Source].push_back(edge.Destination);
			
			if (DoubleSidedConnection)
				m_AdjList[edge.Destination].push_back(edge.Source);
		}

		void RemoveEdge(const Edge& edge)
		{		
			{
				auto it = std::find(m_AdjList[edge.Source].begin(), m_AdjList[edge.Source].end(), edge.Destination);
				if (it != m_AdjList[edge.Source].end())
					m_AdjList[edge.Source].erase(it);

				std::cout << edge.Destination << std::endl;
			}
			if (DoubleSidedConnection)
			{
			
				auto it = std::find(m_AdjList[edge.Destination].begin(), m_AdjList[edge.Destination].end(), edge.Source);
				if (it != m_AdjList[edge.Destination].end())
					m_AdjList[edge.Destination].erase(it);
			}
		}

		void RemoveData(size_t index)
		{	
			if (DoubleSidedConnection)
			{
				for (auto it : m_AdjList[index])
				{
					auto del = std::find(m_AdjList[it].begin(), m_AdjList[it].end(), index);
					if (del != m_AdjList[it].end())
						m_AdjList[it].erase(del);
				}
			}
			m_AdjList[index].clear();

			m_Data.Erase(index);
		}

		template <typename Func>
		void Traverse(const Func& func)
		{
			uint32_t counter = 0;
			for (auto& adj : m_AdjList)
			{
				for (size_t i = 0; i < adj.size(); ++i)
				{
					T* next = nullptr;
					T* previous = nullptr;
					if (i < adj.size() - 1)
						next = &m_Data[adj[i + 1]].Data;
					else if (i != 0)
						next = &m_Data[adj[0]].Data;
					if (i > 0)
						previous = &m_Data[adj[i - 1]].Data;
					
					func(m_Data[counter].Data, m_Data[adj[i]].Data, next, previous);
				}
				
				counter++;
			}
		}
		template <typename Func>
		void TraverseRecursive(const Func& func)
		{
			if (m_Visited.size() != m_AdjList.size())
				m_Visited.resize(m_AdjList.size());

			for (auto it : m_Visited)
				it = false;

			for (size_t i = 0; i < m_AdjList.size(); ++i)
			{
				if (!m_Visited[i])
					traverseRecursive(i, i, nullptr, nullptr, func);
			}
		}

		
	
		template <typename Func>
		void TraverseChildren(size_t parent, const Func& func)
		{	
			uint32_t counter = 0;
			auto& adj = m_AdjList[parent];
			for (auto& vertex : m_AdjList[parent])
			{
				T* next = nullptr;
				T* previous = nullptr;
				if (counter < adj.size() - 1)
					next = &m_Data[adj[counter + 1]].Data;
				else if (counter != 0)
					next = &m_Data[adj[0]].Data;
				if (counter > 0)
					previous = &m_Data[adj[counter - 1]].Data;

				func(m_Data[parent].Data, m_Data[vertex].Data, parent, vertex, next, previous, adj.size() == 1);
				
				counter++;
			}
		}

		const GraphVertex<T>& GetFirstChild(size_t parent) const
		{
			return m_AdjList[parent][0];
		}

		GraphVertex<T>& GetFirstChild(size_t parent)
		{
			return m_Data[m_AdjList[parent][0]];
		}

		const GraphVertex<T>& GetLastChild(size_t parent) const
		{
			return m_Data[m_AdjList[parent].back()];
		}

		GraphVertex<T>& GetLastChild(size_t parent)
		{
			return m_Data[m_AdjList[parent].back()];
		}

		const GraphVertex<T>& GetVertex(size_t index) const
		{
			return m_Data[index];
		}

		GraphVertex<T>& GetVertex(size_t index)
		{
			return m_Data[index];
		}

		std::vector<std::vector<size_t>>& GetAdjList() { return m_AdjList; }

		FreeList<GraphVertex<T>>& GetData() { return m_Data; }
	private:
		template <typename Func>
		void traverseRecursive(size_t parent, size_t child, T* next, T* previous ,const Func& func)
		{
			m_Visited[parent] = true;		
			if (parent != child)
			{
				func(m_Data[parent].Data, m_Data[child].Data, parent, child, next, previous, m_AdjList[child].empty());			
			}	
			for (size_t i = 0; i < m_AdjList[child].size(); ++i)
			{
				if (m_AdjList[child].size() - 1 > i)
					next = &m_Data[m_AdjList[child][i + 1]].Data;
				else if (i != 0)
					next = &m_Data[m_AdjList[child][0]].Data;
				if (i > 0)
					previous = &m_Data[m_AdjList[child][i - 1]].Data;
				else
					previous = &m_Data[m_AdjList[child][m_AdjList[child].size() - 1]].Data;


				if (!m_Visited[m_AdjList[child][i]])
				{
					traverseRecursive(child, m_AdjList[child][i], next, previous, func);
				}
				else
				{
					func(m_Data[child].Data, m_Data[m_AdjList[child][i]].Data, child, m_AdjList[child][i], next, previous, m_AdjList[child].size() == 1);
				}
			}		
		}
	

	private:

		// construct a vector of vectors to represent an adjacency list
		std::vector<std::vector<size_t>> m_AdjList;
		std::vector<bool> m_Visited;

		FreeList<GraphVertex<T>> m_Data;
	};

	

}