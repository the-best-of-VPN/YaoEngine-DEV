#include "VertexArray.h"
#include<glad/glad.h>
#include<assert.h>
namespace YaoEngine {
	VertexArray::VertexArray(float* vertices, unsigned int* indices, unsigned int indexCount, unsigned int vertexCount, const BufferLayout& layout)
	{
		glCreateVertexArrays(1, &m_RendererID);
		m_VertexBuffer = CreateRef<VertexBuffer>(vertices, vertexCount, layout);
		m_IndexBuffer = CreateRef<IndexBuffer>(indices, indexCount);
		glBindVertexArray(m_RendererID);
		m_VertexBuffer->Bind();
		m_IndexBuffer->Bind();
		unsigned int index = 0;
		for (const auto& element : layout.GetElements())
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(
				index,
				element.GetComponentCount(),
				GL_FLOAT,
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)(intptr_t)element.Offset
			);
			index++;
		}	
	}
	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}
	void VertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}
	void VertexArray::UnBind() const
	{
		glBindVertexArray(0);
	}
}