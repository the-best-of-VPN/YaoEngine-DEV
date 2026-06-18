#pragma once
#include<core.h>
#include"IndexBuffer.h"
#include"VertexBuffer.h"
namespace YaoEngine {
	class VertexArray
	{
	public:
		VertexArray(float* vertices, unsigned int* indices, unsigned int indexCount, unsigned int vertexCount, const BufferLayout& layout);
		//VertexArray() = delete;
		VertexArray(const VertexArray&) = delete;
		~VertexArray();
		inline Ref<VertexBuffer> GetVertexBuffer() { return m_VertexBuffer; }
		inline Ref<IndexBuffer> GetIndexBuffer() { return m_IndexBuffer; }
		
		void Bind() const;
		void UnBind() const;

		inline unsigned int GetID() const { return m_RendererID; }
	private:
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
		unsigned int m_RendererID;
	};
}