#include"./VertexBuffer.h"
#include<glad/glad.h>
namespace YaoEngine
{
	VertexBuffer::VertexBuffer(const void* data, unsigned int size)
	{
		glCreateBuffers(1, &m_rendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}
	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &m_rendererID);
	}
	void VertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
	}
	void VertexBuffer::UnBind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}