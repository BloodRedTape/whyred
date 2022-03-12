#include "render/shadow_pass.hpp"
#include "utils/fs.hpp"

ShadowPass::ShadowPass(){
	{
		VertexAttribute attributes[] = {
			VertexAttribute::Float32x3,
			VertexAttribute::Float32x3,
			VertexAttribute::Float32x3
		};

		const Shader * shaders[] = {
			Shader::Create(ShaderLang::GLSL, ShaderStageBits::Vertex,   ReadEntireFile("shaders/shadow.vert.glsl")),
			Shader::Create(ShaderLang::GLSL, ShaderStageBits::Fragment, ReadEntireFile("shaders/shadow.frag.glsl"))
		};

		GraphicsPipelineProperties props;
		props.Shaders = shaders;
		props.VertexAttributes = attributes;
		props.CullMode = CullMode::Back;
		props.DepthFunction = DepthFunction::Less;
		props.Pass = m_ShadowMap.RenderPass();
		props.Layout = m_SetLayout.Get();

		m_GraphicsPipeline = UniquePtr<GraphicsPipeline>(
			GraphicsPipeline::Create(props)
		);

		for(const Shader *shader: shaders)
			delete shader;
	}
}

AABB3f SceneBoundingBox(const Scene& scene) {
	AABB3f box = scene.Instances[0].Mesh->BoundingBox();
	
	for (auto instance : scene.Instances) {
		AABB3f mesh_box = instance.Mesh->BoundingBox();

		if(mesh_box.Max.x > box.Max.x)
            box.Max.x = mesh_box.Max.x;
		if(mesh_box.Max.y > box.Max.y)
            box.Max.y = mesh_box.Max.y;
		if(mesh_box.Max.z > box.Max.z)
            box.Max.z = mesh_box.Max.z;

			
		if(mesh_box.Min.x < box.Min.x)
            box.Min.x = mesh_box.Min.x;
		if(mesh_box.Min.y < box.Min.y)
            box.Min.y = mesh_box.Min.y;
		if(mesh_box.Min.z < box.Min.z)
            box.Min.z = mesh_box.Min.z;
    }

	return box;
}

Pair<const Texture2D*, Camera>  ShadowPass::CmdRenderPass(CommandBuffer& buffer,const Camera& camera, const Scene& scene){
	m_SetPool.NextFrame();
	m_ModelUniformPool.Reset();

	OrthographicCamera depth_camera{20, 20, 0, 100};
	depth_camera.Position = scene.Sunlight.Positon;
	depth_camera.Rotation = scene.Sunlight.Rotation;

	m_CameraUniform.CmdCopy(buffer, depth_camera);
	for (const Instance &instance : scene.Instances) {
		DescriptorSet *set = m_SetPool.Alloc();
		set->UpdateUniformBinding(0, 0, m_CameraUniform);

		UniformBuffer<ModelUniform> *model_uniform = m_ModelUniformPool.NewOrGet();
		model_uniform->CmdCopy(buffer, instance.Transform);
		set->UpdateUniformBinding(1, 0, *model_uniform);
	}

	auto fb_size = m_ShadowMap.Size();
	buffer.SetScissor (0, 0, fb_size.x, fb_size.y);
	buffer.SetViewport(0, 0, fb_size.x, fb_size.y);
	buffer.ClearDepthStencil(m_ShadowMap.DepthAttachment(), 1.f);
	buffer.Bind(m_GraphicsPipeline.Get());
	buffer.BeginRenderPass(m_ShadowMap.RenderPass(), m_ShadowMap.Framebuffer());
	{
		for(int i = 0; i<scene.Instances.Size(); i++){
			buffer.Bind(m_SetPool[i]);
			scene.Instances[i].Mesh->CmdDraw(buffer);
		}
	}
	buffer.EndRenderPass();

	return {m_ShadowMap.DepthAttachment(), depth_camera};
}
