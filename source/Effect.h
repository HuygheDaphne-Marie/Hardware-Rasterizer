#pragma once
#include <vector>

class Effect
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& path);
	Effect(Effect& other) = delete;
	Effect(Effect&& other) = delete;
	Effect operator=(Effect& other) = delete;
	Effect operator=(Effect&& other) = delete;
	~Effect();

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

