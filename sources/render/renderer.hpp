#pragma once

#include <core/unique_ptr.hpp>
#include <core/print.hpp>
#include <core/array.hpp>
#include <core/math/transform.hpp>
#include <graphics/api/texture.hpp>
#include <graphics/api/semaphore.hpp>
#include <graphics/api/buffer.hpp>
#include <graphics/api/command_buffer.hpp>

#include "render/mesh.hpp"
#include "render/transform.hpp"
#include "render/camera.hpp"
#include "utils/pool.hpp"
#include "render/uniforms.hpp"
#include "render/base_pass.hpp"

class Renderer{
private:
    UniquePtr<CommandPool> m_Pool{
        CommandPool::Create()
    };

    UniquePtr<CommandBuffer, CommandBufferDeleter> m_CmdBuffer{
        m_Pool->Alloc(), {m_Pool.Get()}
    };

    BasePass m_BasePass;

    Fence m_RenderFinished;
public:
	Renderer(const RenderPass *pass);

    void Render(const Framebuffer *fb, const Camera &camera, const Scene &scene, const Semaphore *wait, const Semaphore *signal);
};