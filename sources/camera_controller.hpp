#pragma once

#include <core/math/vector2.hpp>
#include "render/camera.hpp"

class CameraController {
private:
	Vector2s m_LastMousePosition;
	Camera &m_Camera;
public:
	float Speed = 1.f;

	CameraController(Camera &camera, Vector2s initial_mouse_position):
		m_Camera(camera),
		m_LastMousePosition(initial_mouse_position)
	{}

	void Update(float dt, Vector2s mouse_position);
};