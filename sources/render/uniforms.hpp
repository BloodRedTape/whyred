#pragma once

#include <core/math/transform.hpp>
#include "camera.hpp"

template <typename ContentStruct>
class UniformBuffer{
private:
    UniquePtr<Buffer> m_UniformBuffer;
    UniquePtr<Buffer> m_StagingBuffer;
    ContentStruct *m_StagingDataPtr{m_StagingBuffer->Map<ContentStruct>()};
public:
    UniformBuffer():
        m_UniformBuffer(
            Buffer::Create(
                sizeof(ContentStruct), 
                BufferMemoryType::DynamicVRAM, 
                BufferUsageBits::UniformBuffer | BufferUsageBits::TransferDestination
            )
        ),
        m_StagingBuffer(
            Buffer::Create(
                sizeof(ContentStruct), 
                BufferMemoryType::UncachedRAM, 
                BufferUsageBits::TransferSource
            )
        )
    {}

    void CmdCopy(CommandBuffer& cmd_buffer, const ContentStruct& content)const{
        *m_StagingDataPtr = content;
        cmd_buffer.Copy(m_StagingBuffer.Get(), m_UniformBuffer.Get(), sizeof(ContentStruct));
    }
    
    operator const Buffer*()const{
        return m_UniformBuffer.Get();
    }
};

struct CameraUniform{
    Matrix4f u_Projection;
    Matrix4f u_View;
    Vector3f u_CameraPosition;

    CameraUniform(const Camera& camera):
        u_Projection(camera.Projection),
        u_View(camera.View()),
        u_CameraPosition(camera.Position)
    {}
};

struct ModelUniform{
    Matrix4f u_Model;
    Matrix4f u_Normal;

    ModelUniform(const Transform& trasform):
        u_Model(trasform.ToMatrix()),
        u_Normal(Math::Rotate<float>(Math::Rad(trasform.Rotation)))
    {}
};

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

struct LightsUniform {
    static constexpr size_t s_MaxLightsCount = 32;

    PointLight u_PointLights[s_MaxLightsCount];
    int        u_PointLightsCount = 0;
    DirLight   u_DirLights[s_MaxLightsCount];
    int        u_DirLightsCount = 0;
    Spotlight  u_Spotlights[s_MaxLightsCount];
    int        u_SpotlightsCount = 0;

    LightsUniform(ConstSpan<PointLight> point_lights, ConstSpan<DirLight> dir_lights, ConstSpan<Spotlight> spotlights){
        SX_ASSERT(point_lights.Size() <= s_MaxLightsCount);
        SX_ASSERT(dir_lights.Size() <= s_MaxLightsCount);
        SX_ASSERT(spotlights.Size() <= s_MaxLightsCount);

        for (const PointLight &light: point_lights)
            u_PointLights[u_PointLightsCount++] = light;

        for (const DirLight &light: dir_lights)
            u_DirLights[u_DirLightsCount++] = light;

        for (const Spotlight &light: spotlights)
            u_Spotlights[u_SpotlightsCount++] = light;
    }
};
