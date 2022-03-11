#include "camera_controller.hpp"
#include <core/os/keyboard.hpp>
#include <core/math/transform.hpp>
#include <core/math/linear.hpp>

void CameraController::Update(float dt, Vector2s mouse_position) {
	using namespace Math;

	Vector3f move_direction;
	
	if(Keyboard::IsKeyPressed(Key::W))
		move_direction.z += 1.f;
	if(Keyboard::IsKeyPressed(Key::S))
		move_direction.z -= 1.f;
	if(Keyboard::IsKeyPressed(Key::D))
		move_direction.x += 1.f;
	if(Keyboard::IsKeyPressed(Key::A))
		move_direction.x -= 1.f;
	if(Keyboard::IsKeyPressed(Key::Space))
		move_direction.y += 1.f;
	if(Keyboard::IsKeyPressed(Key::LeftShift))
		move_direction.y -= 1.f;
	
	if(move_direction.x || move_direction.y || move_direction.z)
		move_direction = Normalize(move_direction) * dt * (Speed + 2 * Keyboard::IsKeyPressed(Key::LeftControl));
		
	Vector4f move_offset = RotateY<float>(Rad(m_Camera.Rotation.y)) * Vector4f(move_direction, 1);

	m_Camera.Position += move_offset.XYZ();

	Vector2s rotate_offset = m_LastMousePosition - mouse_position;
	m_Camera.Rotation += Vector3f(-rotate_offset.y, rotate_offset.x, 0);
	if(m_Camera.Rotation.x > 90)
		m_Camera.Rotation.x = 90;
	if(m_Camera.Rotation.x < -90)
		m_Camera.Rotation.x = -90;
	m_LastMousePosition = mouse_position;
}