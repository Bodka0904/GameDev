#include "stdafx.h"
#include "ScenePanel.h"



namespace XYZ {

	std::pair<glm::vec3, glm::vec3> ScenePanel::castRay(float mx, float my) const
	{
		glm::vec4 mouseClipPos = { mx, my, -1.0f, 1.0f };

		auto inverseProj = glm::inverse(m_EditorCamera.GetProjectionMatrix());
		auto inverseView = glm::inverse(glm::mat3(m_EditorCamera.GetViewMatrix()));

		glm::vec4 ray = inverseProj * mouseClipPos;
		glm::vec3 rayPos = m_EditorCamera.GetPosition();
		glm::vec3 rayDir = inverseView * glm::vec3(ray);

		return { rayPos, rayDir };
	}

	std::pair<float, float> ScenePanel::getMouseViewportSpace() const
	{
		auto [mx, my] = Input::GetMousePosition();
		auto& window = InGui::GetWindow(m_PanelID);
		mx -= window.Position.x;
		my -= window.Position.y;

		auto viewportWidth = window.Size.x;
		auto viewportHeight = window.Size.y;

		return { (mx / viewportWidth) * 2.0f - 1.0f, ((my / viewportHeight) * 2.0f - 1.0f) * -1.0f };
	}

	ScenePanel::ScenePanel(uint32_t panelID)
		:
		m_PanelID(panelID)
	{
		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
		uint32_t windowWidth = Application::Get().GetWindow().GetWidth();
		uint32_t windowHeight = Application::Get().GetWindow().GetHeight();
		m_EditorCamera.SetViewportSize((float)windowWidth, (float)windowHeight);

		InGui::ImageWindow(m_PanelID, "Scene", glm::vec2(0.0f), glm::vec2(200.0f), m_SubTexture);
		InGui::End();

		m_ViewportSize = InGui::GetWindow(m_PanelID).Size;
	}
	void ScenePanel::SetContext(Ref<Scene> context)
	{
		m_Context = context;
	}
	void ScenePanel::SetSubTexture(Ref<SubTexture> subTexture)
	{
		m_SubTexture = subTexture;
	}

	void ScenePanel::OnUpdate(Timestep ts)
	{
		if (IS_SET(InGui::GetWindow(m_PanelID).Flags, InGuiWindowFlags::Hoovered))
		{
			m_EditorCamera.OnUpdate(ts);		
		}

		glm::vec2 newViewportSize = InGui::GetWindow(m_PanelID).Size;
		if (m_Context.Raw() && m_ViewportSize != newViewportSize)
		{
			m_ViewportSize = newViewportSize;
			SceneRenderer::SetViewportSize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_Context->SetViewportSize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}
	}
	void ScenePanel::OnInGuiRender()
	{
		if (InGui::ImageWindow(m_PanelID, "Scene", glm::vec2(0.0f), glm::vec2(200.0f), m_SubTexture))
		{
			
		}
		InGui::End();
	}
	void ScenePanel::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowResizeEvent>(Hook(&ScenePanel::onWindowResize, this));
		dispatcher.Dispatch<MouseButtonPressEvent>(Hook(&ScenePanel::onMouseButtonPress, this));
		if (IS_SET(InGui::GetWindow(m_PanelID).Flags, InGuiWindowFlags::Hoovered))
		{
			m_EditorCamera.OnEvent(event);
		}
	}
	bool ScenePanel::onWindowResize(WindowResizeEvent& event)
	{
		return false;
	}
	bool ScenePanel::onMouseButtonPress(MouseButtonPressEvent& event)
	{
		if (event.IsButtonPressed(MouseCode::MOUSE_BUTTON_LEFT) && !Input::IsKeyPressed(KeyCode::KEY_LEFT_ALT))
		{
			if (IS_SET(InGui::GetWindow(m_PanelID).Flags, InGuiWindowFlags::Hoovered))
			{
				if (m_Context.Raw())
				{
					m_Context->SetSelectedEntity(NULL_ENTITY);
					auto [mouseX, mouseY] = getMouseViewportSpace();
					auto [origin, direction] = castRay(mouseX, mouseY);
	
					for (uint32_t entityID : m_Context->GetEntities())
					{
						SceneEntity entity(entityID, m_Context.Raw());
						TransformComponent& transformComponent = entity.GetComponent<TransformComponent>();
						glm::mat4 entityTransform = transformComponent.GetTransform();
						
						Ray ray = { origin,direction };
						AABB aabb(
							transformComponent.Translation - (transformComponent.Scale / 2.0f),
							transformComponent.Translation + (transformComponent.Scale / 2.0f)
						);

						if (ray.IntersectsAABB(aabb))
						{
							m_Context->SetSelectedEntity(entityID);
							return true;
						}				
					}
				}			
			}
		}
		return false;
	}
}