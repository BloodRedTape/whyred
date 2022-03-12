#include "camera.hpp"
#include <core/math/functions.hpp>
#include <core/math/transform.hpp>

Matrix4f Camera::View()const{
    using namespace Math;
    return Rotate<float>(-Vector3f{Rad(Rotation.x), Rad(Rotation.y), Rad(Rotation.z)}) * Translate(-Position);
}

PerspectiveCamera::PerspectiveCamera(float aspect, float fov, float near, float far){
    using namespace Math;

	Projection[0][0] = 1.f/(aspect*Tan(Rad(fov/2)));
    Projection[1][1] =-1.f/Tan(Rad(fov/2));
    Projection[2][2] = (far + near)/(far - near);
    Projection[2][3] = 2*(far * near)/(far - near);
    Projection[3][2] = 1.f;
    Projection[3][3] = 1;
}

OrthographicCamera::OrthographicCamera(float width, float height, float near, float far){
    Projection[0][0] = 2.f / width;
    Projection[1][1] =-2.f / height;
    Projection[2][2] = 1.f / (far - near);
    Projection[2][3] = near / (far - near);
    Projection[3][3] = 1.f;
}
