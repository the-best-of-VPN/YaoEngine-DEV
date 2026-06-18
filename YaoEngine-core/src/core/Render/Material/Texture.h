#pragma once
#include<filesystem>
#include<core.h>
namespace YaoEngine
{
	struct PixelData {
	};

	class Texture2D;
	class Texture {
	public:
		enum class Type :uint8_t {
			None = 0,
			Texture2D = 1,
			Texture3D = 2,
			Cubemap = 3,
		};
		virtual ~Texture() = default;
		virtual void Bind(unsigned int slot = 0) const = 0;
		virtual void UnBind() const = 0;
		virtual Type GetType() const = 0;
		static Ref<Texture> Create(std::filesystem::path path, Type type);
	};

	class Texture2D :public Texture {
	public:
		Texture2D(std::filesystem::path path);
		Texture2D() = default;
		~Texture2D();

		//static Ref<Texture2D> CreateFallback();

		void Bind(unsigned int slot = 0) const override;
		void UnBind() const override;
		Type GetType() const override;
		unsigned int GetRenderID() const { return m_RenderID; }
		void CreateWhiteTexture();
	private:
		unsigned int m_RenderID=0;
	};
}