#pragma once 
namespace YaoEngine {
	enum class AssetType
	{
		None = 0,
		Texture,
		Shader,
		Mesh,
		Material,
		Scene,
		Prefab,
		AudioClip,
	};
	class Asset
	{
	public:
		virtual ~Asset() = default;
		virtual AssetType GetType() const = 0;
	protected:
		AssetType m_type = AssetType::None;
	};
}