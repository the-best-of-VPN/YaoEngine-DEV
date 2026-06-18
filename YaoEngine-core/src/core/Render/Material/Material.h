#pragma once
#include"Shader.h"
#include"Texture.h"
#include<core.h>
namespace YaoEngine
{
	struct Material2D
	{
		Ref<Shader> shader;
		Ref<Texture> texture;
		static Material2D Create(std::filesystem::path shaderpath, std::filesystem::path texturepath)
		{
			Material2D material;
			material.shader = CreateRef<Shader>(shaderpath);
			material.texture = Texture::Create(texturepath, Texture::Type::Texture2D);
			return material;
		}
	};
}
