#include "render/renderer.hpp"
#include <graphics/api/gpu.hpp>
#include <graphics/api/framebuffer.hpp>
#include <graphics/api/shader.hpp>
#include <core/print.hpp>
#include <core/math/transform.hpp>
#include "utils/fs.hpp"

Renderer::Renderer(const RenderPass* pass):
	m_BasePass(pass)
{
	m_RenderFinished.Signal();
}

void Renderer::Render(const Framebuffer* fb, const Camera& camera, const Scene &scene, const Semaphore* wait, const Semaphore* signal){
	m_RenderFinished.WaitAndReset();
	
	m_CmdBuffer->Begin();
	m_BasePass.CmdRenderPass(*m_CmdBuffer, fb, camera, scene);
	m_CmdBuffer->End();

	GPU::Execute(m_CmdBuffer.Get(), *wait, *signal, m_RenderFinished);
}
