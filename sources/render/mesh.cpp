#include "render/mesh.hpp"
#include <assimp/BaseImporter.h>
#include <assimp/postprocess.h>
#include <assimp/version.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/vector3.h>
#include <core/list.hpp>

Mesh::Mesh(ConstSpan<Vertex> vertices, ConstSpan<Index> indices):
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
	)
{}

void Mesh::Bind(CommandBuffer& buffer){
	buffer.BindVertexBuffer(m_VertexBuffer.Get());
	buffer.BindIndexBuffer(m_IndexBuffer.Get(), IndicesType::Uint32);
}

static Vector3f ToVector3(aiVector3D vector){
    return {vector.x, vector.y, vector.z};
}

Mesh Mesh::LoadFromFile(const char* filepath) {
    Assimp::Importer imp;
    const aiScene *scene = imp.ReadFile(filepath, aiProcess_Triangulate | aiProcess_GenNormals);
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

    for(int i = 0; i<scene->mNumMeshes; i++){
        for(int k = 0; k<scene->mMeshes[i]->mNumVertices; k++){
            vertices.Add(
                {
                    ToVector3(scene->mMeshes[i]->mVertices[k]),
                    ToVector3(scene->mMeshes[i]->mNormals[k]),
                    Vector3f{1, 1, 1}
                }
            );
        }

        for(int j = 0; j<scene->mMeshes[i]->mNumFaces; j++)
            for(int h = 0; h<scene->mMeshes[i]->mFaces[j].mNumIndices;h++)
                indices.Add(scene->mMeshes[i]->mFaces[j].mIndices[h]);
    }

    return Mesh(vertices, indices);
}
