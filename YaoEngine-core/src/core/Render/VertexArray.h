#pragma once
#include<list>
#include<vector>
#include<string>
#include"IndexBuffer.h"
#include"VertexBuffer.h"
namespace YaoEngine
{
    enum class ShaderDataType
    {
        None = 0,

        Float,
        Float2,
        Float3,
        Float4,

        Mat3,
        Mat4,

        Int,
        Int2,
        Int3,
        Int4,

        Bool
    };
    static unsigned int ShaderDataTypeSize(ShaderDataType type);
  
    struct BufferElement
    {
        std::string Name;

        ShaderDataType Type;

        uint32_t Size;
        size_t Offset;

        bool Normalized;

        BufferElement() = default;

        BufferElement(
            ShaderDataType type,
            const std::string& name,
            bool normalized = false)
            :
            Name(name),
            Type(type),
            Size(ShaderDataTypeSize(type)),
            Offset(0),
            Normalized(normalized)
        {
        }
    };
    class BufferLayout
    {
    public:
        BufferLayout(const std::initializer_list<BufferElement>& elements)
            :
            m_Elements(elements)
        {
            CalculateOffsetsAndStride();
        }

        uint32_t GetStride() const { return m_Stride; }

        const std::vector<BufferElement>& GetElements() const
        {
            return m_Elements;
        }

    private:
        void CalculateOffsetsAndStride()
        {
            size_t offset = 0;
            m_Stride = 0;

            for (auto& element : m_Elements)
            {
                element.Offset = offset;

                offset += element.Size;
                m_Stride += element.Size;
            }
        }
    private:
        std::vector<BufferElement> m_Elements;
        unsigned int m_Stride = 0;
    };
	class VertexArray
	{
	public:
		VertexArray(const VertexBuffer,const  IndexBuffer);
		~VertexArray();

		void Bind(BufferLayout& layout) const;
		void UnBind() const;

		inline unsigned int GetID() const { return m_rendererID; }
		IndexBuffer& GetIndexBuffer() { return m_indexBuffer; }
	private:
		unsigned int m_rendererID;
		IndexBuffer m_indexBuffer;
		VertexBuffer m_vertexBuffer;

	};
}