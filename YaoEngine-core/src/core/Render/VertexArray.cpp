#include"./VertexArray.h"
#include<glad/glad.h>


namespace YaoEngine
{
    static unsigned int ShaderDataTypeSize(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::Float:  return 4;
        case ShaderDataType::Float2: return 4 * 2;
        case ShaderDataType::Float3: return 4 * 3;
        case ShaderDataType::Float4: return 4 * 4;

        case ShaderDataType::Mat3: return 4 * 3 * 3;
        case ShaderDataType::Mat4: return 4 * 4 * 4;
        }

        return 0;
    }

	VertexArray::VertexArray(const VertexBuffer vertexBuffer,const IndexBuffer  indexBuffer)
	{
		m_vertexBuffer=VertexBuffer(vertexBuffer);
		m_indexBuffer=IndexBuffer(indexBuffer);
		glCreateVertexArrays(1, &m_rendererID);
	}
	VertexArray::~VertexArray() {
		glDeleteVertexArrays(1, &m_rendererID);
	}

	void VertexArray::Bind(BufferLayout& layout) const
	{
		for (auto& a : layout.GetElements())
		{
			glEnableVertexAttribArray(a.Offset);
			glVertexAttribPointer(a.Offset, a.Size / ShaderDataTypeSize(a.Type), GL_FLOAT, a.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)a.Offset);
		}
		m_vertexBuffer.Bind();

		glBindVertexArray(m_rendererID);
		m_indexBuffer.Bind();
	}
	void VertexArray::UnBind() const
	{
		glBindVertexArray(0);
		m_vertexBuffer.UnBind();
		m_indexBuffer.UnBind();
	}




}