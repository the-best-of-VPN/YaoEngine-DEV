#pragma once
namespace YaoEngine {
	class VertexBuffer
	{
	public:
		VertexBuffer(const void* data, unsigned int size);
		VertexBuffer() = default;
		~VertexBuffer();

		void Bind() const;
		void UnBind() const;

		inline unsigned int GetID() const { return m_rendererID; }
	private:
		unsigned int m_rendererID;
	};
}