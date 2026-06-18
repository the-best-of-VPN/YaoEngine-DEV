#include"Texture.h"
#include<glad/glad.h>
#include<Log/Log.h>
#include"../stb_image/stb_image.h"
namespace YaoEngine
{
	
	Texture2D::Texture2D(std::filesystem::path path)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &nrChannels, 0);
		if (!data)
		{
			Yaoerror("Failed to load texture: %s, using fallback", path.string().c_str());
			CreateWhiteTexture();
			return;
		}

		GLenum internalFormat, dataFormat;
		if (nrChannels == 4) {
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else {
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RenderID);
		glTextureStorage2D(m_RenderID, 1, internalFormat, width, height);

		glTextureParameteri(m_RenderID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RenderID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(m_RenderID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RenderID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureSubImage2D(m_RenderID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}


	Texture2D::~Texture2D()
	{
		glDeleteTextures(1, &m_RenderID);
	}
	void Texture2D::Bind(unsigned int slot) const
	{
		glBindTextureUnit(slot, m_RenderID);
	}
	void Texture2D::UnBind() const
	{
		glBindTextureUnit(0, 0);
	}
	Texture::Type Texture2D::GetType() const
	{
		return Type::Texture2D;
	}
	Ref<Texture> Texture::Create(std::filesystem::path path, Type type)
	{
		switch (type)
		{
		case Type::Texture2D:
			return CreateRef<Texture2D>(path);
		default:
			Yaoerror("Unsupported texture type: %d", static_cast<int>(type));
			return nullptr;
		}
	}
	void Texture2D::CreateWhiteTexture()
	{
		unsigned char data[4] = { 255, 255, 255, 255 };

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RenderID);
		glTextureStorage2D(m_RenderID, 1, GL_RGBA8, 1, 1);

		glTextureParameteri(m_RenderID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RenderID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(m_RenderID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RenderID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureSubImage2D(
			m_RenderID,
			0,
			0, 0,
			1, 1,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			data
		);
	}
}