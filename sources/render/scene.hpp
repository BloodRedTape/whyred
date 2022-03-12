#pragma once

#include <core/span.hpp>
#include <core/math/vector4.hpp>
#include "render/mesh.hpp"
#include "render/transform.hpp"

struct alignas(sizeof(Vector4f)) PointLight{
	Vector3f Position  = {0, 0, 0};
	float    Radius    = 0;
	Vector4f Color     = {1.f, 1.f, 1.f, 1.f};
};

struct alignas(sizeof(Vector4f)) DirLight {
	Vector4f Color     = {1.f, 1.f, 1.f, 1.f};
	Vector3f Direction = {0, 0, 0};
};

struct alignas(sizeof(Vector4f)) Spotlight {
    Vector3f Position = {0, 0, 0};
    float    CutoffAngle = 40;
	Vector4f Color     = {1.f, 1.f, 1.f, 1.f};
    Vector3f Direction = {0, 0, 1};
    float    DimAngle = 5;
};

struct Instance {
    Transform Transform = {};
    Mesh     *Mesh      = nullptr;
};

struct Scene {
    ConstSpan<Instance>   Instances;
    ConstSpan<PointLight> PointLights;
    ConstSpan<DirLight>   DirLights;
    ConstSpan<Spotlight>  Spotlights;
};
