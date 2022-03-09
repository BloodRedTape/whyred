#include "render/renderer.hpp"
#include <graphics/api/gpu.hpp>
#include <graphics/api/framebuffer.hpp>

Renderer::Renderer(const RenderPass* pass){

}

void Renderer::Render(const Framebuffer* fb, const Camera& camera, ConstSpan<Instance> draw_list, const Semaphore* wait, const Semaphore* signal){
	m_CmdBuffer->Begin();
	m_CmdBuffer->ClearColor(fb->Attachments()[0], Color::Black);
	m_CmdBuffer->End();

	GPU::Execute(m_CmdBuffer.Get(), *wait, *signal);
}
