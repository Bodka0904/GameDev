#include "EditorLayer.h"


#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>



namespace XYZ {

	static glm::vec2 GetWorldPositionFromInGui(const InGuiWindow &window, const EditorCamera& camera)
	{
		auto [x, y] = Input::GetMousePosition();
		auto [width, height] = Input::GetWindowSize();
	
		x -= (((float)width / 2) + window.Position.x);
	
		float boundWidth = (camera.GetZoomLevel() * camera.GetAspectRatio()) * 2;
		float boundHeight = (camera.GetZoomLevel() * camera.GetAspectRatio()) * 2;
	
		x = (x / window.Size.x) * boundWidth - boundWidth * 0.5f;
		y = boundHeight * 0.5f - (y / (window.Size.y + InGuiWindow::PanelSize)) * boundHeight;
	
		return { x + camera.GetPosition().x ,y + camera.GetPosition().y };
	}

	EditorLayer::~EditorLayer()
	{
	}

	void EditorLayer::OnAttach()
	{
		Renderer::Init();
		NativeScriptEngine::Init();

		NativeScriptEngine::SetOnReloadCallback([this] {
			auto storage = m_Scene->GetECS().GetComponentStorage<NativeScriptComponent>();
			for (int i = 0; i < storage->Size(); ++i)
			{
				(*storage)[i].ScriptableEntity = (ScriptableEntity*)NativeScriptEngine::CreateScriptObject((*storage)[i].ScriptObjectName);
				if ((*storage)[i].ScriptableEntity)
				{

				}
			}
		});

		NativeScriptEngine::SetOnRecompileCallback([this]() {
			auto storage = m_Scene->GetECS().GetComponentStorage<NativeScriptComponent>();
			for (int i = 0; i < storage->Size(); ++i)
			{
				if ((*storage)[i].ScriptableEntity)
				{

				}
			}
		});


		auto& app = Application::Get();
		m_FBO = FrameBuffer::Create({ app.GetWindow().GetWidth(),app.GetWindow().GetHeight() });
		m_FBO->CreateColorAttachment(FrameBufferFormat::RGBA16F);
		m_FBO->CreateDepthAttachment();
		m_FBO->Resize();

		m_Scene = m_AssetManager.GetAsset<Scene>("Assets/Scenes/wallscene.xyz");
		SceneManager::Get().SetActive(m_Scene);

		m_Material = m_AssetManager.GetAsset<Material>("Assets/Materials/material.mat");
		m_Material->SetFlags(XYZ::RenderFlags::TransparentFlag);

		m_SceneHierarchyPanel.SetContext(m_Scene);


		InGui::RenderWindow("Scene", m_FBO->GetColorAttachment(0).RendererID, { 0,0 }, { 200,200 }, 25.0f);
		InGui::End();
		auto flags = InGui::GetWindowFlags("scene");
		flags &= ~EventListener;
		InGui::SetWindowFlags("scene", flags);

		InGui::Begin("Test", { 0,0 }, { 200,200 });
		InGui::End();
		InGui::SetWindowFlags("test", (MenuEnabled | Visible | EventListener));

		{
			//size_t pointOne = m_FloorTest.CreatePoint({ 0,0,0 }, "Point One");
			//size_t pointTwo = m_FloorTest.CreatePoint({ 0,10,0 }, "Point Two");
			//size_t pointThree = m_FloorTest.CreatePoint({ -10,10,0 }, "Point Three");
			//size_t pointFour = m_FloorTest.CreatePoint({ -10,0,0 }, "Point Four");
			//size_t pointFive = m_FloorTest.CreatePoint({ 15,15,0 }, "Point Five");
			//
			//m_FloorTest.Connect(pointOne, pointTwo);	
			//m_FloorTest.Connect(pointTwo, pointThree);		
			//m_FloorTest.Connect(pointTwo, pointFive);
			//m_FloorTest.Connect(pointThree, pointOne);
			//m_FloorTest.Connect(pointFour, pointOne);
			//
			//m_FloorTest.Connect(pointFour, pointThree);
			//m_FloorTest.Connect(pointFour, pointTwo);
			//m_FloorTest.Connect(pointOne, pointFive);

			//size_t parentPoint = m_FloorTest.CreatePoint({ 0,-2,0 }, "Parent");
			//size_t fifthPoint = m_FloorTest.CreatePointFromPoint({ 20,10,0 }, parentPoint, "Fifth Point");
			//size_t fourthPoint = m_FloorTest.CreatePointFromPoint({ 10,10,0 }, parentPoint, "Fourth Point");
			//size_t thirdPoint = m_FloorTest.CreatePointFromPoint({ 0,10,0 }, parentPoint, "Third Point");
			//size_t secondPoint = m_FloorTest.CreatePointFromPoint({ -8,7,0 },parentPoint, "Second Point");
			//size_t firstPoint = m_FloorTest.CreatePointFromPoint({ -5,-25,0 },parentPoint, "First Point");
			//
			//
			//size_t sixthPoint = m_FloorTest.CreatePoint({ -4,12,0 }, "Sixth Point");
			//size_t seventhPoint = m_FloorTest.CreatePoint({ -15,9,0 }, "Seventh Point");
			//size_t eighthPoint = m_FloorTest.CreatePoint({ -15,-3, 0 }, "Eighth Point");
			//size_t ninthPoint = m_FloorTest.CreatePoint({ -7,-3, 0 }, "Ninth Point");
			//size_t tenthPoint = m_FloorTest.CreatePoint({ 20,-30, 0 }, "Tenth Point");
			//size_t eleventhPoint = m_FloorTest.CreatePoint({ -15,-10, 0 }, "Eleventh Point");
			//size_t twelvethPoint = m_FloorTest.CreatePoint({ -30,-10,0 }, "Twelveth Point");
			//
			////m_FloorTest.Connect(secondPoint, parentPoint);
			//m_FloorTest.Connect(secondPoint, sixthPoint);
			//m_FloorTest.Connect(secondPoint, seventhPoint);
			//m_FloorTest.Connect(secondPoint, eighthPoint);
			//m_FloorTest.Connect(secondPoint, ninthPoint);
			//
			////m_FloorTest.Connect(ninthPoint, secondPoint);
			//m_FloorTest.Connect(ninthPoint ,twelvethPoint);
			//m_FloorTest.Connect(ninthPoint, eleventhPoint);
			//m_FloorTest.Connect(ninthPoint, firstPoint);
			//m_FloorTest.Connect(eleventhPoint, twelvethPoint);
			//m_FloorTest.Connect(twelvethPoint, firstPoint);
			//m_FloorTest.Connect(eleventhPoint, firstPoint);
			
			m_FloorTest.GenerateMesh();
		}
	}

	void EditorLayer::OnDetach()
	{
		NativeScriptEngine::Shutdown();
		Renderer::Shutdown();
	}
	void EditorLayer::OnUpdate(Timestep ts)
	{
		RenderCommand::SetClearColor(glm::vec4(0.2, 0.2, 0.2, 1));
		RenderCommand::Clear();
		NativeScriptEngine::Update(ts);
			
	
		glm::vec2 winSize = { Input::GetWindowSize().first, Input::GetWindowSize().second };
	
		//m_FBO->Bind();	
		RenderCommand::SetClearColor(glm::vec4(0.2, 0.2, 0.5, 1));
		RenderCommand::Clear();
		m_Scene->OnUpdate(ts);
		m_Scene->OnRenderEditor({ m_EditorCamera.GetViewProjectionMatrix(),winSize });
		
		Renderer2D::BeginScene({ m_EditorCamera.GetViewProjectionMatrix() });
		Renderer2D::SetMaterial(m_Material);
		MeshRenderer::BeginScene({ m_EditorCamera.GetViewProjectionMatrix() });
		MeshRenderer::SetMaterial(m_Material);
		

		if (m_EditingPoint)
		{
			auto [mx, my] = Input::GetMousePosition();
			auto [width, height] = Input::GetWindowSize();
			auto pos = m_EditorCamera.GetPosition();
			float cameraWidth = m_EditorCamera.GetAspectRatio() * m_EditorCamera.GetZoomLevel() * 2;
			glm::vec2 mousePos = { mx,my };
			
			mousePos.x = (mousePos.x / width) * cameraWidth - cameraWidth * 0.5f;
			mousePos.y = cameraWidth * 0.5f - (mousePos.y / height) * cameraWidth;
			
			mousePos.x += pos.x;
			mousePos.y += pos.y;

			m_FloorTest.SetPointPosition({ mousePos.x,mousePos.y,0.0f }, m_EditedPoint);
			m_FloorTest.GenerateMesh();
		}

		m_FloorTest.SubmitToRenderer();
		
		MeshRenderer::Flush();
		MeshRenderer::EndScene();
		Renderer2D::Flush();
		Renderer2D::FlushLines();
		Renderer2D::EndScene();

		m_FBO->Unbind();
		

		

		//if (m_ActiveWindow)
		{
			m_EditorCamera.OnUpdate(ts);		
		}
	
	}
	void EditorLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowResizeEvent>(Hook(&EditorLayer::onWindowResized, this));
		dispatcher.Dispatch<MouseButtonPressEvent>(Hook(&EditorLayer::onMouseButtonPress, this));
		dispatcher.Dispatch<MouseButtonReleaseEvent>(Hook(&EditorLayer::onMouseButtonRelease, this));
		dispatcher.Dispatch<KeyPressedEvent>(Hook(&EditorLayer::onKeyPress, this));
		dispatcher.Dispatch<KeyReleasedEvent>(Hook(&EditorLayer::onKeyRelease, this));
		m_EditorCamera.OnEvent(event);
	}

	void EditorLayer::OnInGuiRender(Timestep ts)
	{
		
		m_SceneHierarchyPanel.OnInGuiRender();
		m_EntityComponentPanel.OnInGuiRender();
		if (InGui::RenderWindow("Scene", m_FBO->GetColorAttachment(0).RendererID, { 0,0 }, { 200,200 }, 25.0f))
		{
			m_ActiveWindow = true;
			InGui::Selector();
		}
		else
		{
			m_ActiveWindow = false;
		}
		InGui::End();


		if (InGui::NodeWindow("Node panel", { -100,-100 }, { 200,200 }, ts))
		{
		
		}
		InGui::NodeWindowEnd();

		if (InGui::Begin("Test Panel", { 0,0 }, { 200,200 }))
		{

		}
		InGui::End();

		if (InGui::Begin("Test", { 0,0 }, { 200,200 }))
		{
			
		}
		if (InGui::MenuBar("File", 70, m_MenuOpen))
		{
			if (InGui::MenuItem("Load Scene", { 150,25 }))
			{
				auto& app = Application::Get();
				std::cout << "WTF" << std::endl;
				std::string filepath = app.OpenFile("(*.xyz)\0*.xyz\0");
				if (!filepath.empty())
				{
					m_Scene = m_AssetManager.GetAsset<Scene>(filepath);
				}
				m_MenuOpen = false;
			}
			else if (InGui::MenuItem("Create Script", { 150,25 }))
			{
				auto& app = Application::Get();
				std::string filepath = app.CreateNewFile("(*.cpp)\0*.cpp\0");
				if (!filepath.empty())
				{
					PerModuleInterface::g_pRuntimeObjectSystem->AddToRuntimeFileList(filepath.c_str());
				}
				m_MenuOpen = false;
			}
			else if (InGui::MenuItem("Load Script", { 150,25 }))
			{
				auto& app = Application::Get();
				std::string filepath = app.OpenFile("(*.cpp)\0*.cpp\0");
				if (!filepath.empty())
				{
					PerModuleInterface::g_pRuntimeObjectSystem->AddToRuntimeFileList(filepath.c_str());
				}
				m_MenuOpen = false;
			}

		}
		InGui::MenuBar("Settings", 100, m_MenuOpen);
		InGui::MenuBar("Settingass", 120, m_MenuOpen);
		InGui::MenuBar("Settingasdas", 120, m_MenuOpen);
		if (InGui::Button("Compile", { 100,25 }))
		{
			PerModuleInterface::g_pRuntimeObjectSystem->CompileAll(true);
		}
		InGui::End();
	}

	bool EditorLayer::onWindowResized(WindowResizeEvent& event)
	{
		auto specs = m_FBO->GetSpecification();
		specs.Width = event.GetWidth();
		specs.Height = event.GetHeight();
		m_FBO->SetSpecification(specs);
		m_FBO->Resize();

		return false;
	}
	bool EditorLayer::onMouseButtonPress(MouseButtonPressEvent& event)
	{
		auto [mx, my] = Input::GetMousePosition();
		auto [width, height] = Input::GetWindowSize();
		auto pos = m_EditorCamera.GetPosition();
		float cameraWidth = m_EditorCamera.GetAspectRatio() * m_EditorCamera.GetZoomLevel() * 2;
		glm::vec2 mousePos = { mx,my };

		mousePos.x = (mousePos.x / width) * cameraWidth - cameraWidth * 0.5f;
		mousePos.y = cameraWidth * 0.5f - (mousePos.y / height) * cameraWidth;

		mousePos.x += pos.x;
		mousePos.y += pos.y;

		if (event.IsButtonPressed(MouseCode::XYZ_MOUSE_BUTTON_LEFT))
		{
			glm::vec2 relativeMousePos = GetWorldPositionFromInGui(InGui::GetWindow("scene"), m_EditorCamera);
			m_SceneHierarchyPanel.SelectEntity(relativeMousePos);

			size_t point = 0;
			if (m_FloorTest.GetPoint(mousePos, point))
			{
				m_StartPoint = point;
				m_EditedPoint = m_FloorTest.CreatePointFromPoint({ mousePos,0.0f }, point, "Edited Point");
				m_EditingPoint = true;
			}
			else
			{
				m_StartPoint = m_FloorTest.CreatePoint({ mousePos,0.0f }, "New Point");
				m_EditedPoint = m_FloorTest.CreatePointFromPoint({ mousePos,0.0f }, m_StartPoint, "Edited Point");
				m_EditingPoint = true;
			}
		}
		else if (event.IsButtonPressed(MouseCode::XYZ_MOUSE_BUTTON_RIGHT))
		{
			size_t point = 0;
			if (m_FloorTest.GetPoint(mousePos, point))
			{
				m_FloorTest.DestroyPoint(point);
				m_FloorTest.GenerateMesh();
			}
		}
		
		return false;
	}
	bool EditorLayer::onMouseButtonRelease(MouseButtonReleaseEvent& event)
	{
		auto [mx, my] = Input::GetMousePosition();
		auto [width, height] = Input::GetWindowSize();
		auto pos = m_EditorCamera.GetPosition();
		float cameraWidth = m_EditorCamera.GetAspectRatio() * m_EditorCamera.GetZoomLevel() * 2;
		glm::vec2 mousePos = { mx,my };

		mousePos.x = (mousePos.x / width) * cameraWidth - cameraWidth * 0.5f;
		mousePos.y = cameraWidth * 0.5f - (mousePos.y / height) * cameraWidth;

		mousePos.x += pos.x;
		mousePos.y += pos.y;


		// You are creating whole new points which u connect in same place , it is required to keep saved starting and ending point in order to make it work
		if (event.IsButtonReleased(MouseCode::XYZ_MOUSE_BUTTON_LEFT))
		{
			size_t point = 0;
			if (m_FloorTest.GetPoint(mousePos, point) && point != m_StartPoint && point != m_EditedPoint)
			{
				m_FloorTest.Connect(m_StartPoint, point);
				m_FloorTest.DestroyPoint(m_EditedPoint);

				Stopwatch timer;
				m_FloorTest.GenerateMesh();
			}
			m_EditingPoint = false;
			m_EditedPoint = 0;
			m_StartPoint = 0;
		}
		return false;
	}
	bool EditorLayer::onKeyPress(KeyPressedEvent& event)
	{
		
		return false;
	}
	bool EditorLayer::onKeyRelease(KeyReleasedEvent& event)
	{
		
		return false;
	}
}