#pragma once

#include <core/math/vector3.hpp>
#include <core/math/matrix4.hpp>

struct Camera {
	Vector3f Position{};
	Vector3f Rotation{};
	Matrix4f Projection{1.f};

	Matrix4f View()const;

	Vector3f Project(Vector3f point)const{
		return (Projection * Vector4f(point, 1)).XYZ();
	}
};

struct PerspectiveCamera: Camera{
	PerspectiveCamera(float aspect, float fov, float near, float far);
};
struct OrthographicCamera: Camera{
	OrthographicCamera(float width, float height, float near, float far);
};