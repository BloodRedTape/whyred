#include "render/mesh.hpp"
#include <assimp/BaseImporter.h>
#include <assimp/postprocess.h>
#include <assimp/version.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/vector3.h>
#include <core/list.hpp>
#include <string>
#include <core/os/file.hpp>
#include <cassert>

Mesh::Mesh(ConstSpan<Vertex> vertices, ConstSpan<Index> indices, const AABB3f &bounding_box, String name):
	m_VertexBuffer(
		Buffer::Create(
			vertices.Pointer(), vertices.Size() * sizeof(Vertex), 
			BufferMemoryType::DynamicVRAM, 
			BufferUsageBits::VertexBuffer | BufferUsageBits::TransferDestination
		)
	),
	m_IndexBuffer(
		Buffer::Create(
			indices.Pointer(), indices.Size() * sizeof(Index), 
			BufferMemoryType::DynamicVRAM, 
			BufferUsageBits::IndexBuffer| BufferUsageBits::TransferDestination
		)
	),
    m_BoundingBox(bounding_box),
    m_Name(Move(name))
{}

void Mesh::CmdDraw(CommandBuffer& buffer){
	buffer.BindVertexBuffer(m_VertexBuffer.Get());
	buffer.BindIndexBuffer(m_IndexBuffer.Get(), IndicesType::Uint32);
    buffer.DrawIndexed(IndicesCount());
}

static Vector3f ToVector3(aiVector3D vector){
    return {vector.x, vector.y, vector.z};
}

Mesh Mesh::LoadFromFile(const char* filepath) {
    std::string path = filepath;

    if(!File::Exists(path.c_str()))
        path = "../../../" + path;

    SX_ASSERT(File::Exists(path.c_str()));

    Assimp::Importer imp;
    const aiScene *scene = imp.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals);
    assert(scene->mNumMeshes == 1);

    int vertices_count = 0;
    int indices_count = 0;

    for(int i = 0; i<scene->mNumMeshes; i++){
        vertices_count += scene->mMeshes[i]->mNumVertices;
        
        for(int j = 0; j<scene->mMeshes[i]->mNumFaces; j++)
            indices_count += scene->mMeshes[i]->mFaces[j].mNumIndices;
    }

    List<Vertex> vertices;
    List<Index> indices;
    vertices.Reserve(vertices_count);
    indices.Reserve(indices_count);

    SX_ASSERT(scene->mNumMeshes);

    AABB3f aabb{ToVector3(*scene->mMeshes[0]->mVertices),ToVector3(*scene->mMeshes[0]->mVertices)};

    for(int k = 0; k<scene->mMeshes[0]->mNumVertices; k++){
        Vector3f position = ToVector3(scene->mMeshes[0]->mVertices[k]);

        if(position.x > aabb.Max.x)
            aabb.Max.x = position.x;
        if(position.y > aabb.Max.y)
            aabb.Max.y = position.y;
        if(position.z > aabb.Max.z)
            aabb.Max.z = position.z;

        if(position.x < aabb.Min.x)
            aabb.Min.x = position.x;
        if(position.y < aabb.Min.y)
            aabb.Min.y = position.y;
        if(position.z < aabb.Min.z)
            aabb.Min.z = position.z;
            
        vertices.Add({
            position,
            ToVector3(scene->mMeshes[0]->mNormals[k]),
            Vector3f{1, 1, 1}
        });
    }

    for(int j = 0; j<scene->mMeshes[0]->mNumFaces; j++)
        for(int h = 0; h<scene->mMeshes[0]->mFaces[j].mNumIndices;h++)
            indices.Add(scene->mMeshes[0]->mFaces[j].mIndices[h]);

    return Mesh(vertices, indices, aabb, filepath);
}
