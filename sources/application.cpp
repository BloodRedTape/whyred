#include "application.hpp"
#include <graphics/api/gpu.hpp>
#include <core/os/clock.hpp>
#include "ui.hpp"

Application::Application(){
	m_Window.SetEventsHandler({this, &Application::OnEvent});

	Vertex vertices[] = {
		{{ 0.5f, 0.5f, 0.5f}, {}, {}},
		{{ 0.5f,-0.5f, 0.5f}, {}, {}},
		{{-0.5f,-0.5f, 0.5f}, {}, {}},
		{{-0.5f, 0.5f, 0.5f}, {}, {}},
	};

	Index indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	m_Meshes.Add(Mesh::LoadFromFile("content/cube.fbx"));
	m_Meshes.Add(Mesh::LoadFromFile("content/sphere.fbx"));
	m_Meshes.Add(Mesh::LoadFromFile("content/plane.fbx"));
	m_Meshes.Add(Mesh::LoadFromFile("content/monkey.fbx"));

	m_Instances.Emplace(Transform{{-3, 0,-2}}, &m_Meshes[0]);
	m_Instances.Emplace(Transform{{3, 0, 0}}, &m_Meshes[1]);
	m_Instances.Emplace(Transform{{0,-1, 0}, {-90, 0, 0}, {20, 1, 20}}, &m_Meshes[2]);
	m_Instances.Emplace(Transform{{0, 2, -5}, {-90, 0, 0}, {3, 3, 3}}, &m_Meshes[3]);

	m_PointLights.Add({{0, 2, 0}, 3, Color::Red});
	m_PointLights.Add({{3, 2, 0}, 2, Color::Blue});

	m_DirLights.Add({Color(1.f, 0.5f, 0.f, 0.2f), {-1, 1, 0}});

	m_Spotlights.Add({
		{0, 4.3, 0},
		40,
		Color::Red,
		{0, -1, 0},
		1
	});
}

void Application::Run(){
	Semaphore acquire, render, present;

	Clock cl;
	while (m_Window.IsOpen()) {
		float dt = cl.Restart().AsSeconds();

		Vector2s mouse_position = Mouse::RelativePosition(m_Window);
		
		if(m_IsControlled)
			m_Controller.Update(dt, mouse_position);
	
		m_Window.AcquireNextFramebuffer(&acquire);

		Scene scene;
		scene.Instances = m_Instances;
		scene.PointLights = m_PointLights;
		scene.DirLights = m_DirLights;
		scene.Spotlights = m_Spotlights;
		scene.Sunlight = m_Sunlight;

		m_Renderer.Render(m_Window.CurrentFramebuffer(), m_Camera, scene, &acquire, &render);

		m_Backend.NewFrame(dt, mouse_position, m_Window.Size());

		OnImGui();

		m_Backend.RenderFrame(m_Window.CurrentFramebuffer(), &render, &present);

		m_Window.PresentCurrentFramebuffer(&present);

		m_Window.DispatchEvents();

	}

	GPU::WaitIdle();
}

void Application::OnImGui() {
	//ImGui::ShowDemoWindow();

	ImGui::Begin("Stats");
	{
		ImGui::Text("GraphicsResources: %d", GraphicsResource::Count());
	}
	ImGui::End();
	ImGui::Begin("Scene");
	{
		ImVec2 size = ImGui::GetContentRegionAvail();
		if(ImGui::CollapsingHeader("Instances")){
			ImGui::BeginChild("Instances", ImVec2(size.x, int(size.y/2 - ImGui::GetItemRectSize().y - ImGui::GetStyle().ItemSpacing.y * 2)));
			{
				ImGui::DragInstances(m_Instances);
			}
			ImGui::EndChild();
		}

		if(ImGui::CollapsingHeader("PointLights")){
			ImGui::BeginChild("PointLights", ImVec2(size.x, int(size.y/2 - ImGui::GetItemRectSize().y - ImGui::GetStyle().ItemSpacing.y * 2)));
			{
				static PointLight new_light;
				if(ImGui::NewPointLight(new_light))
					m_PointLights.Add(new_light);
				ImGui::DragPointLights(m_PointLights);
			}
			ImGui::EndChild();
		}

		if(ImGui::CollapsingHeader("DirLights")){
			ImGui::BeginChild("DirLights", ImVec2(size.x, int(size.y/2 - ImGui::GetItemRectSize().y - ImGui::GetStyle().ItemSpacing.y * 2)));
			{
				ImGui::DragDirLights(m_DirLights);
			}
			ImGui::EndChild();
		}

		if(ImGui::CollapsingHeader("Spotlights")){
			ImGui::BeginChild("Spotlights", ImVec2(size.x, int(size.y/2 - ImGui::GetItemRectSize().y - ImGui::GetStyle().ItemSpacing.y * 2)));
			{
				ImGui::DragSpotlights(m_Spotlights);
			}
			ImGui::EndChild();
		}

		ImGui::DragFloat3("SunPosition", &m_Sunlight.Positon[0]);
		ImGui::DragFloat3("SunPotation", &m_Sunlight.Rotation[0]);
	}
	ImGui::End();
}

void Application::OnEvent(const Event& e){
	Println("Event: %", (int)e.Type);
	if(e.Type == EventType::WindowClose)
		m_Window.Close();

	if(e.Type == EventType::KeyPress && e.KeyPress.KeyCode == Key::Q)
		m_Window.Close();

	if(e.Type == EventType::KeyPress && e.KeyPress.KeyCode == Key::Escape){
		m_IsControlled = false;
		Mouse::SetVisible(true);
	}

	if (e.Type == EventType::MouseButtonPress && !ImGui::IsWindowHovered(ImGuiFocusedFlags_AnyWindow)) {
		m_Controller.Reset(Mouse::RelativePosition(m_Window));
		m_IsControlled = true;
		Mouse::SetVisible(false);
	}

	m_Backend.HandleEvent(e);
}
