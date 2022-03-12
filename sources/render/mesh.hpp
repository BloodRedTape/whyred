#pragma once

#include <core/span.hpp>
#include <core/math/vector3.hpp>
#include <core/math/aabb3.hpp>
#include <core/unique_ptr.hpp>
#include <core/string.hpp>
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
	String m_Name;
	AABB3f m_BoundingBox;
public:
	Mesh(ConstSpan<Vertex> vertices, ConstSpan<Index> indices, const AABB3f &bounding_box, String name = "");

	Mesh(Mesh &&) = default;

	Mesh &operator=(Mesh &&) = default;

	void CmdDraw(CommandBuffer &buffer);

	size_t IndicesCount()const{
		return m_IndexBuffer->Size() / sizeof(Index);
	}

	const String& Name()const {
		return m_Name;
	}

	const AABB3f &BoundingBox()const {
		return m_BoundingBox;
	}

	static Mesh LoadFromFile(const char *filepath);
};