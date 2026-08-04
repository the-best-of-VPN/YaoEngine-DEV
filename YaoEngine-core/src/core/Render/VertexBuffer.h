#pragma once
#include"../Log/Log.h"
#include<cstdint>
#include<string>
#include<utility>
#include<vector>
namespace YaoEngine {
	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};
	namespace Utils {
		static unsigned int ShaderDataTypeSize(ShaderDataType type)
		{
			switch (type)
			{
			case ShaderDataType::Float:		return 4;
			case ShaderDataType::Float2:	return 4 * 2;
			case ShaderDataType::Float3:	return 4 * 3;
			case ShaderDataType::Float4:	return 4 * 4;
			case ShaderDataType::Mat3:		return 4 * 3 * 3;
			case ShaderDataType::Mat4:		return 4 * 4 * 4;
			case ShaderDataType::Int:		return 4;
			case ShaderDataType::Int2:		return 4 * 2;
			case ShaderDataType::Int3:		return 4 * 3;
			case ShaderDataType::Int4:		return 4 * 4;
			case ShaderDataType::Bool:		return 1;
			}
			return 0;
		}
	}
	class BufferElement
	{
	public:
		std::string Name;
		ShaderDataType Type;
		unsigned int Size;
		unsigned int Offset;
		bool Normalized;
		BufferElement() = default;
		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: Name(name), Type(type), Size(Utils::ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
		{
		}
		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
			case ShaderDataType::Float:  return 1;
			case ShaderDataType::Float2: return 2;
			case ShaderDataType::Float3: return 3;
			case ShaderDataType::Float4: return 4;
			case ShaderDataType::Mat3:   return 3 * 3;
			case ShaderDataType::Mat4:   return 4 * 4;
			case ShaderDataType::Int:    return 1;
			case ShaderDataType::Int2:   return 2;
			case ShaderDataType::Int3:   return 3;
			case ShaderDataType::Int4:   return 4;
			case ShaderDataType::Bool:   return 1;
			}

			return 0;
		}
	};
	class BufferLayout
	{
	public:
		BufferLayout() = default;
		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_elements(elements)
		{
			CalculateOffsetsAndStride();
		}
		inline const std::vector<BufferElement>& GetElements() const { return m_elements; }
		inline unsigned int GetStride() const { return m_stride; }
	private:
		void CalculateOffsetsAndStride() {
			unsigned int offset = 0;
			m_stride = 0;
			for (auto& element : m_elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_stride += element.Size;
			}
		}
		std::vector<BufferElement> m_elements;
		unsigned int m_stride;
	};	
	class VertexBuffer
	{
	public:
		VertexBuffer(const void* data, unsigned int size, const BufferLayout& layout);
		VertexBuffer(unsigned int size, const BufferLayout& layout);
		VertexBuffer() = default;
		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;
		VertexBuffer(VertexBuffer&& other) noexcept;
		VertexBuffer& operator=(VertexBuffer&& other) noexcept;
		~VertexBuffer();

		void Bind() const;
		void UnBind() const;
		void SetData(const void* data, unsigned int size) const;
		void SetLayout(const BufferLayout& layout) { m_layout = layout; }
		void SetLayout(BufferLayout&& layout) { m_layout = std::move(layout); }
		BufferLayout GetLayout() const { return m_layout; }

		inline unsigned int GetID() const { return m_rendererID; }
	private:
		unsigned int m_rendererID = 0;
		BufferLayout m_layout;
	};
}
