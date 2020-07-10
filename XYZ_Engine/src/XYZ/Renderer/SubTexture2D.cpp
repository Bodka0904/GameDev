#include "stdafx.h"
#include "SubTexture2D.h"


namespace XYZ {
	SubTexture2D::SubTexture2D(std::shared_ptr<Texture2D> texture, const glm::vec2& coords, const glm::vec2& size)
		: m_Texture(texture)
	{
		glm::vec2 min = { (coords.x * size.x) / texture->GetWidth(), (coords.y * size.y) / texture->GetHeight() };
		glm::vec2 max = { (coords.x + 1) * size.x / texture->GetWidth(), ((coords.y + 1) * size.x) / texture->GetHeight() };
		
		m_TexCoords = { min, max };
	}



	void SubTexture2D::SetCoords(const glm::vec2& coords, const glm::vec2& size)
	{
		glm::vec2 min = { (coords.x * size.x) / m_Texture->GetWidth(), (coords.y * size.y) / m_Texture->GetHeight() };
		glm::vec2 max = { (coords.x + 1) * size.x / m_Texture->GetWidth(), ((coords.y + 1) * size.x) / m_Texture->GetHeight() };

		m_TexCoords = { min, max };
		std::cout << m_TexCoords.x << " " << m_TexCoords.y << " " << m_TexCoords.z << " " << m_TexCoords.w << std::endl;
	}
}