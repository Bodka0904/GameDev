#include "stdafx.h"
#include "InGuiFactory.h"

namespace XYZ {
	static glm::vec2 GenerateTextMesh(const char* source, const Ref<Font>& font, const glm::vec4& color, const glm::vec2& pos, const glm::vec2& size, InGuiMesh& mesh, uint32_t textureID)
	{
		if (!source)
			return { 0.0f, 0.0f };

		size_t oldMeshSize = mesh.Quads.size();
		float height = 0.0f;
		float xCursor = 0.0f;
		float yCursor = 0.0f;
		
		uint32_t counter = 0;
		while (source[counter] != '\0')
		{
			auto& character = font->GetCharacter(source[counter]);
			if (xCursor + (float)character.XAdvance >= size.x)
				break;

			glm::vec2 charSize = {
				character.X1Coord - character.X0Coord,
				character.Y1Coord - character.Y0Coord
			};
			if (height < charSize.y) height = charSize.y;

			glm::vec2 charOffset = { character.XOffset, character.YOffset };
			glm::vec2 charPosition = { pos.x + xCursor + charOffset.x, pos.y + yCursor - charOffset.y };
			glm::vec4 charTexCoord = {
				(float)(character.X0Coord) / (float)(font->GetWidth()), (float)(character.Y0Coord) / (float)(font->GetHeight()),
				(float)(character.X1Coord) / (float)(font->GetWidth()), (float)(character.Y1Coord) / (float)(font->GetHeight())
			};		
			mesh.Quads.push_back({ color, charTexCoord, glm::vec3{charPosition, 0.0f}, charSize, textureID });

			xCursor += character.XAdvance;
			counter++;
		}
		return { xCursor, height };
	}

	void InGuiFactory::GenerateWindow(const char* text, InGuiWindow& window, const glm::vec4& color, const InGuiRenderData& renderData)
	{
		window.Mesh.Quads.clear();
		window.Mesh.Lines.clear();
		glm::vec2 textOffset = { 7.0f, 7.0f };
		glm::vec2 textPosition = { window.Position.x + textOffset.x, window.Position.y + InGuiWindow::PanelHeight - textOffset.y };
		glm::vec2 textSize = { window.Size.x - textOffset.x - InGuiWindow::PanelHeight, window.Size.y - textOffset.y };
		textPosition.x = std::floor(textPosition.x);
		textPosition.y = std::floor(textPosition.y);

		if (!IS_SET(window.Flags, InGuiWindowFlags::Collapsed))
		{
			window.Mesh.Quads.push_back(
				{
					color,
					renderData.SubTexture[InGuiRenderData::WINDOW]->GetTexCoords(),
					{window.Position, InGuiWindow::PanelHeight },
					window.Size,
					InGuiRenderData::TextureID
				});
		}
		window.Mesh.Quads.push_back(
			{
				color,
				renderData.SubTexture[InGuiRenderData::BUTTON]->GetTexCoords(),
				{window.Position, 0.0f},
				{window.Size.x, InGuiWindow::PanelHeight },
				InGuiRenderData::TextureID
			});
		window.Mesh.Quads.push_back(
			{
				color,
				renderData.SubTexture[InGuiRenderData::MIN_BUTTON]->GetTexCoords(),
				{window.Position.x + window.Size.x - InGuiWindow::PanelHeight, window.Position.y, 0.0f},
				{InGuiWindow::PanelHeight,InGuiWindow::PanelHeight },
				InGuiRenderData::TextureID
			});	
		GenerateTextMesh(
			text, renderData.Font, renderData.Color[InGuiRenderData::DEFAULT_COLOR],
			textPosition, textSize, window.Mesh, InGuiRenderData::FontTextureID
		);
		
	}
	glm::vec2 InGuiFactory::GenerateQuadWithText(const char* text, InGuiWindow& window, const glm::vec4& color, const glm::vec2& size, const glm::vec2& position, const InGuiRenderData& renderData)
	{
		glm::vec2 textOffset = { 7.0f, 0.0f };
		glm::vec2 textPosition = position + glm::vec2{size.x, size.y / 2.0f} + textOffset;
		glm::vec2 textSize = { window.Size.x - window.Layout.RightPadding, window.Size.y };
		textPosition.x = std::floor(textPosition.x);
		textPosition.y = std::floor(textPosition.y);
		
		window.Mesh.Quads.push_back(
			{
				color,
				renderData.SubTexture[InGuiRenderData::BUTTON]->GetTexCoords(),
				{position, 0.0f},
				size,
				InGuiRenderData::TextureID
			});

		glm::vec2 genSize = GenerateTextMesh(
			text, renderData.Font, renderData.Color[InGuiRenderData::DEFAULT_COLOR],
			textPosition, textSize, window.Mesh, InGuiRenderData::FontTextureID
		);
		float height = size.y;
		if (height < genSize.y)
			height = genSize.y;

		return glm::vec2(size.x + genSize.x + textOffset.x, height);
	}
}