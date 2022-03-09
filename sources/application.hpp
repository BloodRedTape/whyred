#pragma once

#include <graphics/render_window.hpp>
#include <imgui/backend.hpp>

#include "render/renderer.hpp"

class Application {
private:
	RenderWindow m_Window{1280, 720, "PBR"};
	ImGuiBackend m_Backend{m_Window.FramebufferPass()};

	Renderer m_Renderer{m_Window.FramebufferPass()};
public:
	Application();

	void Run();

	void OnEvent(const Event &e);
};