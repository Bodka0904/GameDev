#pragma once
#include "XYZ/ECS/ECSManager.h"
#include "XYZ/Renderer/RenderComponent.h"
#include "XYZ/Event/GuiEvent.h"
#include "Widget.h"

namespace XYZ {

	class Image : public Widget,
				  public RenderComponent, 
				  Type<Image>
	{
	public:
		Image(
			const glm::vec2& position,
			const glm::vec2& size,
			const glm::vec4& color,
			int32_t textureID,
			Ref<SubTexture2D> subTexture,
			Ref<Material> material,
			SortLayerID layer,
			bool visible
		);

		virtual ~Image() = default;

		void SetSubTexture(Ref<SubTexture2D> subTexture);
		void SetQuad(const Quad & quad);
		void SetColor(const glm::vec4& color);


		virtual const Quad* GetRenderData() const override { return &m_Quad; }
		virtual WidgetType GetWidgetType() override { return WidgetType::Image; }
	private:
		Quad m_Quad;
		Ref<SubTexture2D> m_SubTexture;
	};
}