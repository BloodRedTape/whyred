#include "application.hpp"

Application::Application(){
	m_Window.SetEventsHanlder({this, &Application::OnEvent});

	m_Meshes.Add(Mesh::LoadFromFile("content/cube.fbx"));

	m_Instances.Emplace(Transform{}, &m_Meshes[0], nullptr);
}

void Application::Run(){
	Semaphore acquire, present;
	while (m_Window.IsOpen()) {
		
		m_Window.AcquireNextFramebuffer(&acquire);

		m_Renderer.Render(m_Window.CurrentFramebuffer(), {}, {}, &acquire, &present);

		m_Window.PresentCurrentFramebuffer(&present);

		m_Window.DispatchEvents();
	}
}

void Application::OnEvent(const Event& e){
	if(e.Type == EventType::WindowClose)
		m_Window.Close();
}
