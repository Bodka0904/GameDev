#pragma once



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
	template <typename T>
	class Graph
	{
	public:
		size_t AddVertex(const T& data)
		{
			GraphVertex<T> vertex(data, m_Data.size());
			m_Data.push_back(vertex);

			if (m_Data.size() >= m_AdjList.size())
				m_AdjList.resize(m_Data.size());

			return vertex.Index;
		}

		void AddEdge(const Edge& edge)
		{
			if (edge.Source >= m_AdjList.size())
				m_AdjList.resize(edge.Source);
			if (edge.Destination >= m_AdjList.size())
				m_AdjList.resize(edge.Destination);

			m_AdjList[edge.Source].push_back(edge.Destination);
			m_AdjList[edge.Destination].push_back(edge.Source);
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
			bool* visited = new bool[m_AdjList.size()];
			for (size_t i = 0; i < m_AdjList.size(); i++)
				visited[i] = false;

			traverseRecursive(0, 0, nullptr, nullptr, visited, func);

			delete[]visited;
		}
	
		template <typename Func>
		void TraverseChildren(size_t parent, const Func& func)
		{	
			for (auto& vertex : m_AdjList[parent])
			{
				func(m_Data[parent], m_Data[vertex]);
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


	private:
		template <typename Func>
		void traverseRecursive(size_t parent, size_t child, T* next, T* previous, bool* visited, const Func& func)
		{
			visited[parent] = true;
			if (parent != child)
			{
				func(m_Data[parent].Data, m_Data[child].Data, next, previous, m_AdjList[child].empty());
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


				if (!visited[m_AdjList[child][i]])
				{			
					traverseRecursive(child, m_AdjList[child][i], next, previous, visited, func);
				}
				else
				{
					func(m_Data[child].Data, m_Data[m_AdjList[child][i]].Data, next, previous, false);
				}
			}
		}


	private:

		// construct a vector of vectors to represent an adjacency list
		std::vector<std::vector<size_t>> m_AdjList;


		std::vector<GraphVertex<T>> m_Data;
	};

	

}