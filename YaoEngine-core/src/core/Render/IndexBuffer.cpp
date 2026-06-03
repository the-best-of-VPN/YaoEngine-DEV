#include"IndexBuffer.h"
#include<glad/glad.h>
namespace YaoEngine
{
	IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
		:m_count(count)
	{
		glCreateBuffers(1, &m_rendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
	}
	IndexBuffer::~IndexBuffer()
	{
		glDeleteBuffers(1, &m_rendererID);
	}
	void IndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
	}
	void IndexBuffer::UnBind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}