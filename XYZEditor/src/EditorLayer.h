#pragma once

#include <XYZ.h>

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/EntityComponentsPanel.h"

#include "Tools/EditorCamera.h"


#include "Wall/Floor.h"
#include "Wall/FloorTest.h"


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace XYZ {

	class EditorLayer : public Layer
	{
	public:
		virtual ~EditorLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnEvent(Event& event) override;
		virtual void OnInGuiRender(Timestep ts) override;

	private:
		bool onWindowResized(WindowResizeEvent& event);
		bool onMouseButtonPress(MouseButtonPressEvent& event);
		bool onKeyPress(KeyPressedEvent& event);
		bool onKeyRelease(KeyReleasedEvent& event);
	private:
		SceneHierarchyPanel m_SceneHierarchyPanel;
		EntityComponentPanel m_EntityComponentPanel;

		EditorCamera m_EditorCamera;
		Ref<Scene> m_Scene;
	

		Ref<FrameBuffer> m_FBO;
		AssetManager m_AssetManager;

		Floor m_Floor;
		FloorTest m_FloorTest;

		int m_WallOne;
		int m_WallTwo;
		int m_WallThree;
	private:	
		bool m_ActiveWindow = false;
		bool m_MenuOpen = false;
		Ref<Material> m_Material;
		Ref<Texture2D> m_CharacterTexture;

		Ref<SubTexture2D> m_CharacterSubTexture;
	};
}