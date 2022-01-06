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

	// Todo: Is this ass?
	void SetDiffuseTexture(Texture* pDiffuseTexture);
	void SetNormalMap(Texture* pNormalMap);
	void SetSpecularMap(Texture* pSpecularMap);
	void SetGlossinessMap(Texture* pGlossinessMap);

	Effect& GetEffect();

private:
	Effect m_Effect;

	ID3D11Buffer* m_pVertexBuffer;
	ID3D11InputLayout* m_pVertexLayout;

	uint32_t m_AmountIndices;
	ID3D11Buffer* m_pIndexBuffer;

	// Matrices
	ID3DX11EffectMatrixVariable* m_pWorldViewProjVariable;
	ID3DX11EffectMatrixVariable* m_pWorldMatrixVariable;
	ID3DX11EffectMatrixVariable* m_pViewInverseMatrixVariable;

	// Textures/Maps
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable;

	Texture* m_pDiffuse{ nullptr };
	Texture* m_pNormalMap{ nullptr };
	Texture* m_pSpecularMap{ nullptr };
	Texture* m_pGlossinessMap{ nullptr };

	void SetEffectMatrix(const std::string& matrixVariableName, ID3DX11EffectMatrixVariable* pMeshMatrix, const Elite::FMatrix4& pMeshMatrixValue);
	void SetEffectShaderResource(const std::string& shaderVariableName, ID3DX11EffectShaderResourceVariable* pMeshShaderResource, Texture* pMeshShaderResourceValue);
};

