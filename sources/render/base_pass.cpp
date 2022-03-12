#include "render/base_pass.hpp"
#include "utils/fs.hpp"
#include <graphics/api/framebuffer.hpp>

BasePass::BasePass(const RenderPass *pass):
	m_Pass(pass)
{
	{
		VertexAttribute attributes[] = {
			VertexAttribute::Float32x3,
			VertexAttribute::Float32x3,
			VertexAttribute::Float32x3
		};

		const Shader * shaders[] = {
			Shader::Create(ShaderLang::GLSL, ShaderStageBits::Vertex,   ReadEntireFile("shaders/base.vert.glsl")),
			Shader::Create(ShaderLang::GLSL, ShaderStageBits::Fragment, ReadEntireFile("shaders/base.frag.glsl"))
		};

		GraphicsPipelineProperties props;
		props.Shaders = shaders;
		props.VertexAttributes = attributes;
		props.DepthFunction = DepthFunction::Less;
		props.Pass = pass;
		props.Layout = m_SetLayout.Get();

		m_GraphicsPipeline = UniquePtr<GraphicsPipeline>(
			GraphicsPipeline::Create(props)
		);

		for(const Shader *shader: shaders)
			delete shader;
	}
}

void BasePass::CmdRenderPass(CommandBuffer &buffer, const Framebuffer *fb, const Camera &camera, const Scene &scene, const Texture2D *shadow_map, const Camera &shadow_camera) {
	m_SetPool.NextFrame();
	m_ModelUniformPool.Reset();

	m_LightsUniform.CmdCopy(buffer, {scene.PointLights, scene.DirLights, scene.Spotlights});
	m_CameraUniform.CmdCopy(buffer, camera);
	m_ShadowCameraUniform.CmdCopy(buffer, shadow_camera);
	for (const Instance &instance : scene.Instances) {
		DescriptorSet *set = m_SetPool.Alloc();
		set->UpdateUniformBinding(0, 0, m_CameraUniform);

		UniformBuffer<ModelUniform> *model_uniform = m_ModelUniformPool.NewOrGet();
		model_uniform->CmdCopy(buffer, instance.Transform);
		set->UpdateUniformBinding(1, 0, *model_uniform);

		set->UpdateUniformBinding(2, 0, m_LightsUniform);
		set->UpdateUniformBinding(3, 0, m_ShadowCameraUniform);
		set->UpdateTextureBinding(4, 0, shadow_map, m_ShadowMapSampler.Get());
	}

	auto fb_size = fb->Size();
	buffer.SetScissor (0, 0, fb_size.x, fb_size.y);
	buffer.SetViewport(0, 0, fb_size.x, fb_size.y);
	buffer.ClearColor(fb->Attachments()[0], Color::Black);
	buffer.ClearDepthStencil(fb->Attachments()[1], 1.f);
	buffer.Bind(m_GraphicsPipeline.Get());
	buffer.ChangeLayout(shadow_map, TextureLayout::ShaderReadOnlyOptimal);
	buffer.BeginRenderPass(m_Pass, fb);
	{
		for(int i = 0; i<scene.Instances.Size(); i++){
			buffer.Bind(m_SetPool[i]);
			scene.Instances[i].Mesh->CmdDraw(buffer);
		}
	}
	buffer.EndRenderPass();
	buffer.ChangeLayout(shadow_map, TextureLayout::DepthStencilAttachmentOptimal);
}
