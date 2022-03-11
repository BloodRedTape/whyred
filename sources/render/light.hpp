#pragma once

#include <core/math/vector3.hpp>
#include <core/math/vector4.hpp>

struct PointLight{
	Vector3f Position  = {0, 0, 0};
	float    Radius    = 0;
	Vector4f Color     = {1.f, 1.f, 1.f, 1.f};
};

