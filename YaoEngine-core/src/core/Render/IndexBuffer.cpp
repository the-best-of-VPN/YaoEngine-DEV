#include"IndexBuffer.h"
#include<glad/glad.h>
namespace YaoEngine
{
	IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)noexcept
		:m_count(count)
	{
		glCreateBuffers(1, &m_rendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
	}
	IndexBuffer::IndexBuffer(IndexBuffer&& other) noexcept
		:m_rendererID(other.m_rendererID), m_count(other.m_count)
	{
		other.m_rendererID = 0;
		other.m_count = 0;
	}
	IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other) noexcept
	{
		if (this != &other)
		{
			glDeleteBuffers(1, &m_rendererID);
			m_rendererID = other.m_rendererID;
			m_count = other.m_count;
			other.m_rendererID = 0;
			other.m_count = 0;
		}
		return *this;
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