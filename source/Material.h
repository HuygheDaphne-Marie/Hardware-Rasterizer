#pragma once
#include <vector>

class Material
{
public:
	Material(ID3D11Device* pDevice, const std::wstring& path);
	Material(Material& other) = delete;
	Material(Material&& other) = delete;
	Material operator=(Material& other) = delete;
	Material operator=(Material&& other) = delete;
	~Material();

	ID3DX11Effect* GetEffect() const;
	ID3DX11EffectTechnique* GetTechnique() const;

	void GotoNextTechnique();

private:
	ID3DX11Effect* m_pEffect;
	std::vector<ID3DX11EffectTechnique*> m_pTechniques{}; // Point, Linear, Anisotropic
	unsigned int m_CurrentTechniqueIndex{0};

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& path);
	ID3DX11EffectTechnique* LoadTechnique(const std::string& techniqueName) const;
};

