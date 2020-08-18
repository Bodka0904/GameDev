#pragma once

#include <glm/glm.hpp>

namespace XYZ {
/*! @class HashGrid2D
*	@brief 2D HashGrid for storing elements
*/
	template<typename T>
	class HashGrid2D
	{
	public:
		HashGrid2D(int cellSize, int tableSize)
			: m_CellSize(cellSize), m_TableSize(tableSize)
		{
			m_Table.resize(m_TableSize);
		}
		
		/** Inserts an element into the hashgrid */
		void Insert(const T& element, const glm::vec2& pos, const glm::vec2& size)
		{
			for (int i = (int)std::floor(pos.x); i < (int)std::ceil(pos.x) + (int)std::ceil(size.x); ++i)
			{
				for (int j = (int)std::floor(pos.y); j < (int)std::ceil(pos.y) + (int)std::ceil(size.y); ++j)
				{
					size_t index = ((size_t)floor(abs(i) / m_CellSize) + (size_t)floor(abs(j) / m_CellSize)) % m_TableSize;
					m_Table[index].elements.push_back(element);
				}
			}
		}

		/** Removes an element from the hashgrid */
		void Remove(const T& element, const glm::vec2& pos, const glm::vec2& size)
		{
			for (int i = (int)std::floor(pos.x); i < (int)std::ceil(pos.x) + (int)std::ceil(size.x); ++i)
			{
				for (int j = (int)std::floor(pos.y); j < (int)std::ceil(pos.y) + (int)std::ceil(size.y); ++j)
				{
					size_t index = ((size_t)floor(abs(i) / m_CellSize) + (size_t)floor(abs(j) / m_CellSize)) % m_TableSize;
					auto it = std::find(m_Table[index].elements.begin(), m_Table[index].elements.end(), element);
					if (it != m_Table[index].elements.end())
						m_Table[index].elements.erase(it);
				}
			}
		}
		
		/** Returns the element count */
		size_t GetElements(T** buffer, const glm::vec2& pos, const glm::vec2& size)
		{
			std::vector<size_t> indices;
			size_t count = 0;
			for (int i = (int)std::floor(pos.x); i < (int)std::ceil(pos.x) + (int)std::ceil(size.x); ++i)
			{
				for (int j = (int)std::floor(pos.y); j < (int)std::ceil(pos.y) + (int)std::ceil(size.y); ++j)
				{
					size_t index = ((size_t)floor(abs(i) / m_CellSize) + (size_t)floor(abs(j) / m_CellSize)) % m_TableSize;
					count += m_Table[index].elements.size();
					indices.push_back(index);
				}
			}

			if (count)
			{
				*buffer = new T[count * sizeof(T)];
				T* ptr = *buffer;
				for (auto it : indices)
				{
					size_t elementsCount = m_Table[it].elements.size();
					memcpy(ptr, m_Table[it].elements.data(), elementsCount * sizeof(T));
					ptr += elementsCount;
				}
			}
			return count;
		}

		void Clear()
		{
			for (auto& cell : m_Table)
			{
				cell.elements.clear();
			}
		}
	private:
		struct Cell
		{
			std::vector<T> elements;
		};


		int m_CellSize;
		int m_TableSize;

		std::vector<Cell> m_Table;
	};
}