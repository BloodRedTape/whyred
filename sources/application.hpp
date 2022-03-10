#pragma once

#include <graphics/render_window.hpp>
#include <imgui/backend.hpp>

#include "render/renderer.hpp"
#include "render/mesh.hpp"
#include "camera_controller.hpp"

class Application {
private:
	RenderWindow m_Window{1280, 720, "PBR", TextureFormat::Depth32};
	ImGuiBackend m_Backend{m_Window.FramebufferPass()};

	Renderer m_Renderer{m_Window.FramebufferPass()};
	
	PerspectiveCamera m_Camera{16.0/9.0, 90, 0, 100};
	CameraController m_Controller{m_Camera, Mouse::RelativePosition(m_Window)};

	List<Mesh> m_Meshes;
	List<Instance> m_Instances;
public:
	Application();

	void Run();

	void OnEvent(const Event &e);
};