#include "render/transform.hpp"
#include <core/math/transform.hpp>

Matrix4f Transform::ToMatrix()const{
	using namespace Math;
	return Translate(Position) * Rotate<float>(Rad(Rotation)) * Matrix4f{
		Vector4f(Scale.x, 0,       0,       0),
		Vector4f(0,       Scale.y, 0,       0),
		Vector4f(0,       0,       Scale.z, 0),
		Vector4f(0,       0,       0,       1)
	};
}