#pragma once

#include <core/math/vector3.hpp>
#include <core/math/functions.hpp>
#include <core/math/linear.hpp>

inline Vector3f FromUnitSphereToUnitCube(Vector3f point) {
	using namespace Math;
	return Clamp(Normalize(point) * 2.f, -1.f, 1.f);
}

inline Vector3f RotationFromDirection(Vector3f direction) {
	float y_rad = atan2(direction.z, direction.x);
	float x_rad = atan2(direction.y, Vector2f(direction.z, direction.x).Length());
	return {Deg(x_rad), Deg(y_rad), 0};
}
