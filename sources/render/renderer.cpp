#include "render/renderer.hpp"
#include <graphics/api/gpu.hpp>
#include <graphics/api/framebuffer.hpp>
#include <graphics/api/shader.hpp>
#include <core/print.hpp>
#include <core/math/transform.hpp>
#include "utils/fs.hpp"

Renderer::Renderer(const RenderPass* pass):
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

	m_RenderFinished.Signal();
}

void Renderer::Render(const Framebuffer* fb, const Camera& camera, const Scene &scene, const Semaphore* wait, const Semaphore* signal){
	m_RenderFinished.WaitAndReset();
	m_SetPool.NextFrame();
	m_ModelUniformPool.Reset();

	m_CmdBuffer->Begin();
	m_LightsUniform.CmdCopy(*m_CmdBuffer, {scene.PointLights, scene.DirLights, scene.Spotlights});
	m_CameraUniform.CmdCopy(*m_CmdBuffer, camera);
	for (const Instance &instance : scene.Instances) {
		DescriptorSet *set = m_SetPool.Alloc();
		set->UpdateUniformBinding(0, 0, m_CameraUniform);

		UniformBuffer<ModelUniform> *model_uniform = m_ModelUniformPool.NewOrGet();
		model_uniform->CmdCopy(*m_CmdBuffer, instance.Transform);
		set->UpdateUniformBinding(1, 0, *model_uniform);

		set->UpdateUniformBinding(2, 0, m_LightsUniform);
	}


	auto fb_size = fb->Size();
	m_CmdBuffer->SetScissor (0, 0, fb_size.x, fb_size.y);
	m_CmdBuffer->SetViewport(0, 0, fb_size.x, fb_size.y);
	m_CmdBuffer->ClearColor(fb->Attachments()[0], Color::Black);
	m_CmdBuffer->ClearDepthStencil(fb->Attachments()[1], 1.f);
	m_CmdBuffer->Bind(m_GraphicsPipeline.Get());
	m_CmdBuffer->BeginRenderPass(m_Pass, fb);
	{
		for(int i = 0; i<scene.Instances.Size(); i++){
			m_CmdBuffer->Bind(m_SetPool[i]);
			scene.Instances[i].Mesh->CmdDraw(*m_CmdBuffer);
		}
	}
	m_CmdBuffer->EndRenderPass();
	m_CmdBuffer->End();

	GPU::Execute(m_CmdBuffer.Get(), *wait, *signal, m_RenderFinished);
}
