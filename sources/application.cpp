#include "application.hpp"
#include <graphics/api/gpu.hpp>
#include <core/os/clock.hpp>

Application::Application(){
	m_Window.SetEventsHanlder({this, &Application::OnEvent});

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

		m_Renderer.Render(m_Window.CurrentFramebuffer(), m_Camera, m_Instances, m_PointLights, &acquire, &render);

		m_Backend.NewFrame(dt, mouse_position, m_Window.Size());

		OnImGui();

		m_Backend.RenderFrame(m_Window.CurrentFramebuffer(), &render, &present);

		m_Window.PresentCurrentFramebuffer(&present);

		m_Window.DispatchEvents();
	}

	GPU::WaitIdle();
}

void Application::OnImGui() {
	ImGui::Begin("Debug");
	for (int i = 0; i<m_Instances.Size(); i++) {
		Instance &instance = m_Instances[i];

		ImGui::PushID(i);
		ImGui::Text("Mesh: %s", instance.Mesh->Name().Data());
		ImGui::DragFloat3("Position", &instance.Transform.Position[0], 0.1, -20, 20);
		ImGui::DragFloat3("Rotation", &instance.Transform.Rotation[0], 1, -180, 180);
		ImGui::DragFloat3("Scale", &instance.Transform.Scale[0], 0.1, 0, 20);

		ImGui::Separator();
		ImGui::PopID();
	}

	ImGui::PushID(1);
	for (int i = 0; i<m_PointLights.Size(); i++) {
		PointLight &light = m_PointLights[i];

		ImGui::PushID(i);
		ImGui::DragFloat3("Position", &light.Position[0], 0.1, -20, 20);
		ImGui::DragFloat("Radius", &light.Radius, 0.1, 0, 20);
		ImGui::ColorEdit3("Color", &light.Color[0]);
		ImGui::Separator();
		ImGui::PopID();
	}
	ImGui::PopID();
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
