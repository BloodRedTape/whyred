#pragma once

#include <core/math/vector3.hpp>
#include <core/math/matrix4.hpp>

struct Camera {
	Vector3f Position{};
	Vector3f Rotation{};
	Matrix4f Projection{1.f};

	Matrix4f View()const;
};

struct PerspectiveCamera: Camera{
	PerspectiveCamera(float aspect, float fov, float near, float far);
};