#pragma once

#include <core/math/vector3.hpp>
#include <core/math/matrix4.hpp>

struct Transform {
	Vector3f Position{0, 0, 0};
	Vector3f Rotation{0, 0, 0};
	Vector3f Scale   {1, 1, 1};

	Matrix4f ToMatrix()const;
};