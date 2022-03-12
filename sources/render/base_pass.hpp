#pragma once

#include <core/array.hpp>
#include <graphics/api/command_buffer.hpp>
#include <graphics/api/descriptor_set.hpp>
#include <graphics/api/graphics_pipeline.hpp>
#include <graphics/api/fence.hpp>
#include <graphics/api/shader.hpp>
#include "render/camera.hpp"
#include "render/scene.hpp"
#include "render/uniforms.hpp"
#include "utils/pool.hpp"

class BasePass {
private:
    const RenderPass *m_Pass = nullptr;

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
    BasePass(const RenderPass *pass);

	void CmdRenderPass(CommandBuffer &buffer, const Framebuffer *fb, const Camera &camera, const Scene &scene);
};