#pragma once
#include"RenderHead.h"

namespace YaoEngine
{
	struct Mesh {
		Ref<VertexArray> VAO;
		Ref<VertexBuffer> VBO;
		Ref<IndexBuffer>  IBO;
		static Mesh Create(std::vector<float>& vertices, std::vector<unsigned int>& indices, BufferLayout& layout) {
			Mesh mesh;
			mesh.VAO = CreateRef<VertexArray>(vertices.data(), indices.data(), indices.size(),vertices.size(), layout);
			mesh.VBO = mesh.VAO->GetVertexBuffer();
			mesh.IBO = mesh.VAO->GetIndexBuffer();
			return mesh;
		}
	};
	class Renderer2D
	{
	public:
		static void Draw(const Mesh& mesh, const Material2D& material) {
			material.texture->Bind();
			material.shader->Bind();
			mesh.VAO->Bind();
			glDrawElements(GL_TRIANGLES, mesh.IBO->GetCount(), GL_UNSIGNED_INT, nullptr);
		}
	};
	class RenderAPI
	{
		inline static void SetClearColor(float r, float g, float b, float a) {
			glClearColor(r, g, b, a);
		}
	};
}