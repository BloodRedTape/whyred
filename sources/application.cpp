#include "application.hpp"

Application::Application(){
	m_Window.SetEventsHanlder({this, &Application::OnEvent});
}

void Application::Run(){
	Semaphore acquire;
	while (m_Window.IsOpen()) {
		
		m_Window.AcquireNextFramebuffer(&acquire);
		m_Window.PresentCurrentFramebuffer(&acquire);

		m_Window.DispatchEvents();
	}
}

void Application::OnEvent(const Event& e){
	if(e.Type == EventType::WindowClose)
		m_Window.Close();
}
