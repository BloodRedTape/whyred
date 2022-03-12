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
#include "camera.hpp"
#include "utils/pool.hpp"
#include "uniforms.hpp"

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

class Renderer{
private:
    const RenderPass *m_Pass;
    UniquePtr<CommandPool> m_Pool{
        CommandPool::Create()
    };

    UniquePtr<CommandBuffer, CommandBufferDeleter> m_CmdBuffer{
        m_Pool->Alloc(), {m_Pool.Get()}
    };

    Fence m_RenderFinished;

    const Array<ShaderBinding, 3> m_Bindings {
        ShaderBinding{
            0,
            1,
            ShaderBindingType::UniformBuffer,
            ShaderStageBits::Vertex | ShaderStageBits::Fragment
        },
        ShaderBinding{
            1,
            1,
            ShaderBindingType::UniformBuffer,
            ShaderStageBits::Vertex
        },
        ShaderBinding{
            2,
            1,
            ShaderBindingType::UniformBuffer,
            ShaderStageBits::Fragment
        }
    };

    UniquePtr<DescriptorSetLayout> m_SetLayout{
        DescriptorSetLayout::Create(m_Bindings)
    };
    
    static constexpr size_t s_SetPoolSize = 20;
    SingleFrameDescriptorSetPool m_SetPool{{s_SetPoolSize, m_SetLayout.Get()}};

    ReusableObjectsPool<UniformBuffer<ModelUniform>> m_ModelUniformPool;

    UniquePtr<GraphicsPipeline> m_GraphicsPipeline{nullptr};
    UniformBuffer<CameraUniform> m_CameraUniform;
    UniformBuffer<LightsUniform> m_LightsUniform;
public:
	Renderer(const RenderPass *pass);

    void Render(const Framebuffer *fb, const Camera &camera, const Scene &scene, const Semaphore *wait, const Semaphore *signal);
};