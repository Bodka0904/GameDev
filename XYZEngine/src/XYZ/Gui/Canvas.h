#pragma once
#include "XYZ/ECS/Component.h"

#include <glm/glm.hpp>

namespace XYZ {

	enum class CanvasRenderMode
	{
		ScreenSpace,
		RenderModes
	};

	struct Canvas : public Type<Canvas>
	{
		Canvas(CanvasRenderMode mode, const glm::vec4& color)
			:
			RenderMode(mode),
			Color(color)
		{}

		CanvasRenderMode RenderMode;
		glm::vec4 Color;
	};
}