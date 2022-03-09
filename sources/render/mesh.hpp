#pragma once

#include <core/span.hpp>
#include <core/math/vector3.hpp>
#include <core/unique_ptr.hpp>
#include <graphics/api/buffer.hpp>
#include <graphics/api/command_buffer.hpp>

struct Vertex {
	Vector3f Position;
	Vector3f Normal;
	Vector3f Color;
};

using Index = u32;

class Mesh {
private:
	UniquePtr<Buffer> m_VertexBuffer;
	UniquePtr<Buffer> m_IndexBuffer;
public:
	Mesh(ConstSpan<Vertex> vertices, ConstSpan<Index> indices);

	void Bind(CommandBuffer &buffer);
};