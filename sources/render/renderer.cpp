#include "render/renderer.hpp"
#include <graphics/api/gpu.hpp>
#include <graphics/api/framebuffer.hpp>
#include <graphics/api/shader.hpp>
#include <core/print.hpp>
#include "utils.hpp"

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

void Renderer::Render(const Framebuffer* fb, const Camera& camera, ConstSpan<Instance> draw_list, const Semaphore* wait, const Semaphore* signal){
	m_RenderFinished.WaitAndReset();

	Println("Render: %", draw_list.Size());

	m_SetPool.NextFrame();

	m_CmdBuffer->Begin();
	auto fb_size = fb->Size();
	m_CmdBuffer->SetScissor (0, 0, fb_size.x, fb_size.y);
	m_CmdBuffer->SetViewport(0, 0, fb_size.x, fb_size.y);
	m_CameraUniform.CmdUpdate(*m_CmdBuffer, camera);
	m_CmdBuffer->ClearColor(fb->Attachments()[0], Color::LightBlue);
	m_CmdBuffer->ClearDepthStencil(fb->Attachments()[1], 1.f);
	m_CmdBuffer->Bind(m_GraphicsPipeline.Get());
	m_CmdBuffer->BeginRenderPass(m_Pass, fb);
	{
		for(Instance instance: draw_list){
			DescriptorSet *set = m_SetPool.Alloc();
			set->UpdateUniformBinding(0, 0, m_CameraUniform);
			m_CmdBuffer->Bind(set);
			instance.Mesh->CmdDraw(*m_CmdBuffer);
			Println("Count: %", instance.Mesh->IndicesCount());
		}
	}
	m_CmdBuffer->EndRenderPass();
	m_CmdBuffer->End();

	GPU::Execute(m_CmdBuffer.Get(), *wait, *signal, m_RenderFinished);
}
