#include "camera.hpp"
#include <core/math/functions.hpp>

PerspectiveCamera::PerspectiveCamera(float aspect, float fov, float near, float far){
    using namespace Math;

	Projection[0][0] = 1.f/(aspect*Tan(Rad(fov/2)));
    Projection[1][1] =-1.f/Tan(Rad(fov/2));
    Projection[2][2] = 2.f/(far - near);
    Projection[3][3] = 1.f;
    //Projection[2][3] = (far - near)/2.f - far;
    //Projection[3][2] = -1.f;
}
