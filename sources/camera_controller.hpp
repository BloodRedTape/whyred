#pragma once

#include <core/math/vector2.hpp>
#include "render/camera.hpp"

class CameraController {
private:
	Vector2s m_LastMousePosition;
	Camera &m_Camera;
public:
	float Speed = 1.f;

	CameraController(Camera &camera):
		m_Camera(camera)
	{}

	void Update(float dt, Vector2s mouse_position);
};