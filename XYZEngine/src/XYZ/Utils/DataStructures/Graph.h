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

			m_AdjList[edge.Source].push_back(edge.Destination);
		}


		template <typename Func>
		void Traverse(const Func& func)
		{
			uint32_t counter = 0;
			for (auto& adj : m_AdjList)
			{
				for (size_t i = 0; i < adj.size(); ++i)
				{
					GraphVertex<T>* next = nullptr;
					GraphVertex<T>* previous = nullptr;
					if (i < adj.size() - 1)
						next = &m_Data[adj[i + 1]];
					if (i > 0)
						previous = &m_Data[adj[i - 1]];

					func(m_Data[counter], m_Data[adj[i]], next, previous);
				}
				
				counter++;
			}
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
		// construct a vector of vectors to represent an adjacency list
		std::vector<std::vector<size_t>> m_AdjList;


		std::vector<GraphVertex<T>> m_Data;
	};

	

}