#pragma once

#include <core/unique_ptr.hpp>
#include <core/print.hpp>
#include <core/array.hpp>
#include <core/math/transform.hpp>
#include <graphics/api/texture.hpp>
#include <graphics/api/semaphore.hpp>
#include <graphics/api/buffer.hpp>
#include <graphics/api/command_buffer.hpp>
#include <graphics/api/descriptor_set.hpp>
#include <graphics/api/graphics_pipeline.hpp>
#include <graphics/api/fence.hpp>
#include <graphics/api/shader.hpp>
#include "mesh.hpp"
#include "transform.hpp"
#include "material.hpp"
#include "camera.hpp"

struct Instance {
    Transform Transform = {};
    Mesh     *Mesh      = nullptr;
    Material *Material  = nullptr;
};

class CameraUniform{
private:
    struct Staging{
        Matrix4f u_Projection{1.f};
        Matrix4f u_View{1.f};
    };
    UniquePtr<Buffer> m_UniformBuffer;
    UniquePtr<Buffer> m_StagingBuffer;
    Staging *m_StagingDataPtr{m_StagingBuffer->Map<Staging>()};
public:
    CameraUniform():
        m_UniformBuffer(
            Buffer::Create(
                sizeof(Staging), 
                BufferMemoryType::DynamicVRAM, 
                BufferUsageBits::UniformBuffer | BufferUsageBits::TransferDestination
            )
        ),
        m_StagingBuffer(
            Buffer::Create(
                sizeof(Staging), 
                BufferMemoryType::UncachedRAM, 
                BufferUsageBits::TransferSource
            )
        )
    {}

    void CmdUpdate(CommandBuffer& cmd_buffer, const Camera& camera) {
        using namespace Math;
        m_StagingDataPtr->u_Projection = camera.Projection;
        m_StagingDataPtr->u_View = camera.View();

        cmd_buffer.Copy(m_StagingBuffer.Get(), m_UniformBuffer.Get(), sizeof(Staging));
    }

    operator const Buffer*()const{
        return m_UniformBuffer.Get();
    }
};

class Renderer {
private:
    const RenderPass *m_Pass;
    UniquePtr<CommandPool> m_Pool{
        CommandPool::Create()
    };

    UniquePtr<CommandBuffer, CommandBufferDeleter> m_CmdBuffer{
        m_Pool->Alloc(), {m_Pool.Get()}
    };

    Fence m_RenderFinished;

    const Array<ShaderBinding, 1> m_Bindings {
        ShaderBinding{
            0,
            1,
            ShaderBindingType::UniformBuffer,
            ShaderStageBits::Vertex
        }
    };

    UniquePtr<DescriptorSetLayout> m_SetLayout{
        DescriptorSetLayout::Create(m_Bindings)
    };
    
    static constexpr size_t s_SetPoolSize = 20;
    SingleFrameDescriptorSetPool m_SetPool{{s_SetPoolSize, m_SetLayout.Get()}};

    UniquePtr<GraphicsPipeline> m_GraphicsPipeline{nullptr};
    CameraUniform m_CameraUniform;
public:
	Renderer(const RenderPass *pass);

    void Render(const Framebuffer *fb, const Camera &camera, ConstSpan<Instance> draw_list, const Semaphore *wait, const Semaphore *signal);
};