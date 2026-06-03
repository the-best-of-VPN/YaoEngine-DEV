#pragma once

namespace  YaoEngine
{
	class IndexBuffer
	{
	public:
		IndexBuffer(const unsigned int* data, unsigned int count);
		IndexBuffer() = default;
		~IndexBuffer();
		void Bind() const;
		void UnBind() const;
		inline unsigned int GetCount() const { return m_count; }
		inline unsigned int GetID() const { return m_rendererID; }
	private:
		unsigned int m_rendererID;
		unsigned int m_count;
	};
	
}