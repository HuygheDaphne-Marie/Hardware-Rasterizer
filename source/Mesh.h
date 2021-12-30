#pragma once
#include <vector>
#include "Vertex.h"
#include "Effect.h"
#include "ECamera.h"
#include "Texture.h"

class Mesh
{
public:
	Mesh(ID3D11Device* pDevice, const std::vector<IVertex>& vertices, const std::vector<uint32_t>& indices, const std::wstring& effectPath);
	Mesh(Mesh& other) = delete;
	Mesh(Mesh&& other) = delete;
	Mesh operator=(Mesh& other) = delete;
	Mesh operator=(Mesh&& other) = delete;
	~Mesh();

	void Render(ID3D11DeviceContext* pDeviceContext, Elite::Camera* pCamera);

	void SetDiffuseTexture(Texture* pDiffuseTexture);

private:
	Effect m_Effect;

	ID3D11Buffer* m_pVertexBuffer;
	ID3D11InputLayout* m_pVertexLayout;

	uint32_t m_AmountIndices;
	ID3D11Buffer* m_pIndexBuffer;
	
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable;
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;

	Texture* m_pDiffuse{ nullptr };
};

