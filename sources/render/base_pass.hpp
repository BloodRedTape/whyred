#pragma once

#include <core/array.hpp>
#include <graphics/api/command_buffer.hpp>
#include <graphics/api/descriptor_set.hpp>
#include <graphics/api/graphics_pipeline.hpp>
#include <graphics/api/fence.hpp>
#include <graphics/api/shader.hpp>
#include <core/math/vector3.hpp>
#include "render/camera.hpp"
#include "render/scene.hpp"
#include "render/uniforms.hpp"
#include "utils/pool.hpp"

class BasePass {
public:
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

	struct ShadowCameraUniform{
		Matrix4f u_ShadowProjection;
		Matrix4f u_ShadowView;
	
		ShadowCameraUniform(const Camera& camera):
			u_ShadowProjection(camera.Projection),
			u_ShadowView(camera.View())
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
private:
    const RenderPass *m_Pass = nullptr;

    const Array<ShaderBinding, 5> m_Bindings {
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
        },
        ShaderBinding{
            3,
            1,
            ShaderBindingType::UniformBuffer,
            ShaderStageBits::Fragment
        },
		ShaderBinding{
            4,
            1,
            ShaderBindingType::Texture,
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
    UniformBuffer<ShadowCameraUniform> m_ShadowCameraUniform;
	UniquePtr<Sampler> m_ShadowMapSampler{
		Sampler::Create({})
	};
    UniformBuffer<LightsUniform> m_LightsUniform;
public:
    BasePass(const RenderPass *pass);

	void CmdRenderPass(CommandBuffer &buffer, const Framebuffer *fb, const Camera &camera, const Scene &scene, const Texture2D *shadow_map, const Camera &shadow_camera);
};