#pragma once
class Effect
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& path);
	Effect(Effect& other) = delete;
	Effect(Effect&& other) = delete;
	Effect operator=(Effect& other) = delete;
	Effect operator=(Effect&& other) = delete;
	~Effect();

	const ID3DX11Effect* GetEffect() const;
	const ID3DX11EffectTechnique* GetTechnique() const;

private:
	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTechnique;

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& path);
};

