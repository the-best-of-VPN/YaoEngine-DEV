#include"./VertexBuffer.h"
#include<glad/glad.h>
#include<utility>
namespace YaoEngine
{
	VertexBuffer::VertexBuffer(const void* data, unsigned int size, const BufferLayout& layout)
		: m_layout(layout)
	{
		glCreateBuffers(1, &m_rendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferData(GL_ARRAY_BUFFER, size*sizeof(float), data, GL_STATIC_DRAW);
	}

	VertexBuffer::VertexBuffer(unsigned int size, const BufferLayout& layout)
		: m_layout(layout)
	{
		glCreateBuffers(1, &m_rendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
		: m_rendererID(other.m_rendererID), m_layout(std::move(other.m_layout))
	{
		other.m_rendererID = 0;
	}
	VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept
	{
		if (this != &other)
		{
			glDeleteBuffers(1, &m_rendererID);
			m_rendererID = other.m_rendererID;
			m_layout = std::move(other.m_layout);
			other.m_rendererID = 0;
		}
		return *this;
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
	void VertexBuffer::SetData(const void* data, unsigned int size) const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}
}
