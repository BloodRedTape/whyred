#pragma once

#include <core/unique_ptr.hpp>
#include <graphics/api/buffer.hpp>
#include <graphics/api/command_buffer.hpp>

class Material {
private:
	UniquePtr<Buffer> m_Parameters;
public:
	Material();

	void CmdUpdate(CommandBuffer &cmd_buffer);

	void ImmediateUpdate();
};