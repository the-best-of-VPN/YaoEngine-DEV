#pragma once
namespace  YaoEngine
{
	class IndexBuffer
	{
	public:
		IndexBuffer(const unsigned int* data, unsigned int count)noexcept;
		IndexBuffer() = default;
		IndexBuffer(IndexBuffer&& other) noexcept;
		IndexBuffer& operator=(IndexBuffer&& other) noexcept;
		IndexBuffer(const IndexBuffer&) = delete;
		IndexBuffer& operator=(const IndexBuffer&) = delete;
		~IndexBuffer();
		
		void Bind() const;
		void UnBind() const;
		
		inline unsigned int GetCount() const { return m_count; }
		inline unsigned int GetID() const { return m_rendererID; }
		
	private:
		unsigned int m_rendererID=0;
		unsigned int m_count=0;
	};
	
}