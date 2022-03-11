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

    CameraUniform(const Camera& camera):
        u_Projection(camera.Projection),
        u_View(camera.View())
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