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
	//m_Meshes.Add(Mesh(vertices, indices));

	m_Instances.Emplace(Transform{}, &m_Meshes[0], nullptr);
	//m_Instances.Emplace(Transform{}, &m_Meshes[1], nullptr);
	//m_Instances.Emplace(Transform{}, &m_Meshes[2], nullptr);

}

void Application::Run(){
	Semaphore acquire, present;

	Clock cl;
	while (m_Window.IsOpen()) {
		float dt = cl.Restart().AsSeconds();

		m_Controller.Update(dt, Mouse::RelativePosition(m_Window));
		
		m_Window.AcquireNextFramebuffer(&acquire);

		m_Renderer.Render(m_Window.CurrentFramebuffer(), m_Camera, m_Instances, &acquire, &present);

		m_Window.PresentCurrentFramebuffer(&present);

		m_Window.DispatchEvents();
	}

	GPU::WaitIdle();
}

void Application::OnEvent(const Event& e){
	if(e.Type == EventType::WindowClose)
		m_Window.Close();

	if(e.Type == EventType::KeyPress && e.KeyPress.KeyCode == Key::Q)
		m_Window.Close();
}
