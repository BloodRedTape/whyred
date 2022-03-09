#pragma once

#include <core/unique_ptr.hpp>
#include <core/array.hpp>
#include <graphics/api/texture.hpp>
#include <graphics/api/semaphore.hpp>
#include <graphics/api/buffer.hpp>
#include <graphics/api/command_buffer.hpp>
#include <graphics/api/descriptor_set.hpp>
#include <graphics/api/graphics_pipeline.hpp>
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

class Renderer {
private:
    UniquePtr<CommandPool> m_Pool{
        CommandPool::Create()
    };

    UniquePtr<CommandBuffer, CommandBufferDeleter> m_CmdBuffer{
        m_Pool->Alloc(), {m_Pool.Get()}
    };

    const Array<ShaderBinding, 1> m_Bindings {
        ShaderBinding{
            0,
            1,
            ShaderBindingType::Texture,
            ShaderStageBits::Fragment
        }
    };

    UniquePtr<DescriptorSetLayout> m_SetLayout{
        DescriptorSetLayout::Create(m_Bindings)
    };

    UniquePtr<DescriptorSetPool> m_SetPool{
        DescriptorSetPool::Create({1, m_SetLayout.Get()})
    };

    UniquePtr<DescriptorSet, DescriptorSetDeleter> m_Set{
        m_SetPool->Alloc(), {m_SetPool.Get()}
    };

    UniquePtr<GraphicsPipeline> m_GraphicsPipeline{nullptr};
public:
	Renderer(const RenderPass *pass);

    void Render(const Framebuffer *fb, const Camera &camera, ConstSpan<Instance> draw_list, const Semaphore *wait, const Semaphore *signal);
};