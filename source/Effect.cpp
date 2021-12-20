#include "pch.h"
#include "Effect.h"

#include <sstream>

Effect::Effect(ID3D11Device* pDevice, const std::wstring& path)
	: m_pEffect(nullptr)
	, m_pTechnique(nullptr)
{
	m_pEffect = LoadEffect(pDevice, path);
	if (m_pEffect == nullptr || !m_pEffect->IsValid())
	{
		std::cout << "Effect is invalid";
		return;
	}
	
	m_pTechnique = m_pEffect->GetTechniqueByName("DefaultTechnique");
	if (m_pTechnique == nullptr || !m_pTechnique->IsValid())
	{
		std::cout << "Technique is invalid";
		return;
	}
}
Effect::~Effect()
{
	m_pTechnique->Release();
	m_pEffect->Release();
}

const ID3DX11Effect* Effect::GetEffect() const
{
	return m_pEffect;
}
const ID3DX11EffectTechnique* Effect::GetTechnique() const
{
	return m_pTechnique;
}

ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& path)
{
	HRESULT result = S_OK;
	ID3D10Blob* pErrorBlob = nullptr;
	ID3DX11Effect* pEffect;

	DWORD shaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile(path.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&pEffect,
		&pErrorBlob);

	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			auto* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());

			std::wstringstream ss;
			for (unsigned int i{ 0 }; i < pErrorBlob->GetBufferSize(); i++)
				ss << pErrors[i];

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << std::endl;
		}
		else
		{
			std::wstringstream ss;
			std::wcout << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << path << std::endl;
			return nullptr;
		}
	}

	return pEffect;
}
