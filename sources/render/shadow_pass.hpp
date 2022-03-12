#pragma once

#include <core/array.hpp>
#include <graphics/api/command_buffer.hpp>
#include <graphics/api/descriptor_set.hpp>
#include <graphics/api/graphics_pipeline.hpp>
#include <graphics/api/fence.hpp>
#include <graphics/api/shader.hpp>
#include <graphics/render_texture.hpp>
#include "render/camera.hpp"
#include "render/scene.hpp"
#include "render/uniforms.hpp"
#include "utils/pool.hpp"
#include <core/pair.hpp>

class ShadowPass {
public:
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
	
		ModelUniform(const Transform& trasform):
			u_Model(trasform.ToMatrix())
		{}
	};
private:

    const Array<ShaderBinding, 2> m_Bindings {
        ShaderBinding{
            0,
            1,
            ShaderBindingType::UniformBuffer,
            ShaderStageBits::Vertex
        },
        ShaderBinding{
            1,
            1,
            ShaderBindingType::UniformBuffer,
            ShaderStageBits::Vertex
        }
    };
    RenderTexture m_ShadowMap{{4096, 4096}, TextureFormat::Unknown, TextureFormat::Depth32};

    UniquePtr<DescriptorSetLayout> m_SetLayout{
        DescriptorSetLayout::Create(m_Bindings)
    };
    
    static constexpr size_t s_SetPoolSize = 20;
    SingleFrameDescriptorSetPool m_SetPool{{s_SetPoolSize, m_SetLayout.Get()}};
    ReusableObjectsPool<UniformBuffer<ModelUniform>> m_ModelUniformPool;

    UniformBuffer<CameraUniform> m_CameraUniform;

    UniquePtr<GraphicsPipeline> m_GraphicsPipeline{nullptr};
public:
    ShadowPass();

	Pair<const Texture2D *, Camera> CmdRenderPass(CommandBuffer &buffer, const Camera &camera, const Scene &scene);

};